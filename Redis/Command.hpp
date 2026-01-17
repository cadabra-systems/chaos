/**
 * @file Command.hpp
 * @date 2014-01-01
 * @copyright Cadabra Systems
 * @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_Redis_Command_hpp
#define Chaos_Redis_Command_hpp

#include "Redis.hpp"
#include "Error.hpp"

#include "../Singleton.hpp"

#include <memory>
#include <cstdint>
#include <variant>
#include <utility>
#include <functional>
#include <type_traits>

/**
 * @todo pass-key idiom in procedure/function
 */
namespace chaos { namespace redis {
	class procedure : public std::enable_shared_from_this<procedure>
	{
	/** @name Classes */
	/** @{ */
	public:
		struct return_type
		{
		};

		enum class state
		{
			discard,
			progress,
			error,
			success
		};
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		procedure() = default;
		procedure(procedure&&) = delete;
		procedure(procedure const&) = delete;
		virtual ~procedure() = 0;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		inline bool resolve(reply_ptr&& reply)
		{
			return resolve(reply.get());
		}
		virtual bool resolve(redisReply* reply) = 0;
		virtual bool resolve(std::nullptr_t) = 0;
		virtual bool resolve(error&& value) = 0;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		procedure& operator=(procedure&&) = delete;
		procedure& operator=(procedure const&) = delete;
	/** @} */

	/** @name Factories */
	/** @{ */
	public:
		virtual state execute(redisContext* context) = 0;

	protected:
		state execute(redisContext* context, const char* command_name, ...);
	/** @{ */

	/** @name Getters */
	/** @{ */
	public:
		virtual state get_state() const = 0;
		virtual const error& get_error() const = 0;
	/** @} */

	/** @name States */
	/** @{ */
	public:
		virtual bool is_active() const = 0;
		virtual bool is_ready() const = 0;
		virtual bool is_fail() const = 0;
	/** @} */
	};

	template <typename T>
	class function : public procedure
	{
	/** @name Aliases */
	/** @{ */
	public:
		using progress_type = bool;
		using error_type = error;
		using return_type = T;
		using value_type = typename std::conditional<not std::is_void<T>::value, T, procedure::return_type>::type;
		using delegate_type = typename std::conditional<not std::is_void<T>::value, std::function<void(const value_type&)>, std::function<void()>>::type;
		using variant_type = std::variant<bool, error, value_type>;
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		function()
		:
			_value(false)/*,
			_callback(nullptr)*/
		{

		}

		function(function<T>&&) = delete;
		function(function<T> const&) = delete;

		virtual ~function() override
		{
//			delete _callback;
		}
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		variant_type _value;
		delegate_type _delegate;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		virtual bool resolve(value_type&& value)
		{
			if (!std::holds_alternative<bool>(_value)) {
				return false;
			}
			_value = value;
			if (_delegate) {
				if constexpr (std::is_same<T, void>::value) {
					_delegate();
				} else {
					_delegate(std::as_const(value));
				}
			}
			return true;
		}

		virtual bool resolve(std::nullptr_t) override
		{
			if (!std::holds_alternative<bool>(_value)) {
				return false;
			}
//			_delegate(_value = false);
			return true;
		}

		virtual bool resolve(error&& value) override
		{
			if (!std::holds_alternative<bool>(_value) && std::get<bool>(_value) != true) {
				return false;
			}
//			_delegate(_value = std::move(value));
			return true;
		}

		virtual bool resolve(redisReply* reply) override
		{
			if (!reply) {
				return false;
			} else if (REDIS_REPLY_NIL == reply->type) {
				if constexpr (std::is_same<T, void>::value) {
					resolve(procedure::return_type{});
				} else {
					return false;
				}
			} else if (REDIS_REPLY_ERROR == reply->type) {
				resolve(error{reply});
			} else if (REDIS_REPLY_STATUS == reply->type) {
				if (strcmp(reply->str, "OK") == 0) {
					if constexpr (not std::is_same<T, void>::value) {
						return false;
					} else {
						resolve(procedure::return_type{});
					}
				} else if (strcmp(reply->str, "QUEUED") == 0) {
					_value = true;
				} else if (strcmp(reply->str, "PONG") == 0) {
					if constexpr (std::is_same<T, void>::value) {
						resolve(procedure::return_type{});
					} else if constexpr (std::is_same<T, std::string>::value) {
						resolve(std::string(reply->str, reply->len));
					} else {
						return false;
					}
				} else {
					return false;
				}
			} else if (REDIS_REPLY_STRING == reply->type) {
				if constexpr (std::is_same<T, std::string>::value) {
					resolve(std::string(reply->str, reply->len));
				} else {
					return false;
				}
			} else if (REDIS_REPLY_VERB == reply->type) {
				/**
				 * reply->str: the string value (char*)
				 * reply->len: the string length (size_t)
				 * reply->vtype: content type (char[3]) (txt, mkd, hel)
				 */
				return false;
			} else if (REDIS_REPLY_DOUBLE == reply->type) {
				if constexpr (std::is_same<T, double>::value) {
					resolve(reply->dval);
				} else {
					return false;
				}
			} else if (REDIS_REPLY_INTEGER == reply->type) {
				if constexpr (std::is_same<T, std::int32_t>::value) {
					resolve(reply->integer);
				} else {
					return false;
				}
			} else if (REDIS_REPLY_BIGNUM == reply->type) {

			} else if (REDIS_REPLY_BOOL == reply->type) {
				if constexpr (std::is_same<T, bool>::value) {
					_value = (reply->integer != 0);
				} else {
					return false;
				}
			} else if (REDIS_REPLY_ARRAY == reply->type) {
/*
				for (size_t i = 0; i < reply->elements; i++) {
					 redisReply *r = reply->element[i];
					 if (r->type == REDIS_REPLY_STATUS || r->type == REDIS_REPLY_STRING) {
						 printf("  [%zu]: %s\n", i, r->str);
					 } else if (r->type == REDIS_REPLY_ERROR) {
						 printf("  [%zu]: ERROR: %s\n", i, r->str);
					 } else {
						 printf("  [%zu]: (другой тип: %d)\n", i, r->type);
					 }
				 }
*/
				return false;
			} else if (REDIS_REPLY_SET == reply->type) {
				/**
				 * reply->elements: number of elements (size_t)
				 * reply->element: array elements (redisReply)
				 */
				return false;
			} else if (REDIS_REPLY_MAP == reply->type) {
				/**
				 * reply->elements: number of elements (size_t)
				 * reply->element: array elements (redisReply)
				 */
				return false;
			} else if (REDIS_REPLY_PUSH == reply->type) {
				/**
				 * reply->elements: number of elements (size_t)
				 * reply->element: array elements (redisReply)
				 */
				return false;
			}
			return true;
		}
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		function<T>& operator=(function<T>&&) = delete;
		function<T>& operator=(function<T> const&) = delete;
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		virtual state get_state() const override
		{
			if (std::holds_alternative<error>(_value)) {
				return state::error;
			} else if (std::holds_alternative<value_type>(_value)) {
				return state::success;
			} else if (std::get<bool>(_value)) {
				return state::progress;
			}
			return state::discard;
		}
		virtual const error& get_error() const override
		{
			return std::holds_alternative<error>(_value) ? std::get<error>(_value) : singleton<error>::instance();
		}

		const value_type& get_value() const
		{
			return std::holds_alternative<value_type>(_value) ? std::get<value_type>(_value) : singleton<value_type>::instance();
		}
	/** @} */

	/** @name Mutators */
	/** @{ */
	public:
		void reset_delegate(delegate_type value)
		{
			_delegate = value;
		}
	/** @} */

	/** @name States */
	/** @{ */
	public:
		virtual bool is_active() const override
		{
			return std::holds_alternative<bool>(_value) && std::get<bool>(_value);
		}

		virtual bool is_ready() const override
		{
			return std::holds_alternative<value_type>(_value);
		}

		virtual bool is_fail() const override
		{
			return std::holds_alternative<error>(_value);
		}
	/** @} */
	};

	template <typename T>
	class command
	{
	/** @name Classes */
	/** @{ */
	public:
		enum class status : std::size_t
		{
			progress = 0,
			fault = 1,
			success = 2
		};
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		command() = default;

		command(std::shared_ptr<function<T>>&& function)
		:
			_function(function)
		{

		}

		command(command<T>&& origin)
		:
			_function(std::move(origin))
		{

		}

		command(const command<T>& origin)
		:
			_function(origin)
		{

		}

		~command() = default;
	/** @} */

	/** @name Properties*/
	/** @{ */
	private:
		const std::shared_ptr<function<T>> _function;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		command<T>* operator=(command<T>&& rhs)= delete;
		command<T>* operator=(const command<T>& rhs) = delete;

		const typename function<T>::state operator&() const
		{
			return _function->get_state();
		}

		const typename function<T>::value_type& operator*() const
		{
			return _function->get_value();
		}

		const error& operator->() const
		{
			return _function->get_error();
		}

		void operator()(typename function<T>::delegate_type&& delegate)
		{
			_function && (_function->reset_delegate(delegate), true);
		}

		operator bool() const
		{
			return _function && _function->is_ready();
		}

		bool operator!() const
		{
			return !_function || _function->is_fail();
		}
	/** @} */
	};
} }

#endif
