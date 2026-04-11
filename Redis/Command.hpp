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
#include "../ArrayType.hpp"

#include <memory>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>
#include <variant>
#include <utility>
#include <functional>
#include <type_traits>

namespace chaos { namespace redis {
	/**
	 * @todo pass-key idiom in procedure/function
	 */
	class procedure : public std::enable_shared_from_this<procedure>
	{
	/** @name Classes */
	/** @{ */
	public:
		struct return_type
		{
		};

		enum class state : bool
		{
			idle,
			busy
		};

		enum class status
		{
			discard,
			progress,
			fault,
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
		virtual status execute(redisContext* context) = 0;

	protected:
		status execute(redisContext* context, const char* command_name, ...);
		status execute(redisContext* context, int argc, const char** argv, const std::size_t* argvlen);
	/** @{ */

	/** @name Getters */
	/** @{ */
	public:
		virtual status get_status() const = 0;
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
		using variant_type = std::variant<state, error, value_type>;
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		function()
		:
			_value(state::idle)/*,
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
			if (!std::holds_alternative<state>(_value)) {
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
			if (!std::holds_alternative<state>(_value)) {
				return false;
			}
//			_value = value;
			return true;
		}

		virtual bool resolve(error&& value) override
		{
			if (!std::holds_alternative<state>(_value) && std::get<state>(_value) != state::busy) {
				return false;
			}
			_value = value;
			return true;
		}

		virtual bool resolve(redisReply* reply) override
		{
			if (!reply) {
				/// @xxx Это может быть ошибкой случившейся на frame выше в callstack
				return false;
			} else if (REDIS_REPLY_NIL == reply->type) {
				if constexpr (std::is_same<T, void>::value) {
					/// @note Waiting for OK within REDIS_REPLY_STATUS instead
					resolve(error{error::code::other, "Unexpected nil value"});
				} else {
					/// @note Null value
//					resolve(procedure::return_type{});
				}
			} else if (REDIS_REPLY_ERROR == reply->type) {
				resolve(error{reply});
			} else if (REDIS_REPLY_STATUS == reply->type) {
				if (strcmp(reply->str, "OK") == 0) {
					if constexpr (std::is_same<T, bool>::value) {
						resolve(true);
					} else if constexpr (not std::is_same<T, void>::value) {
						return false;
					} else {
						resolve(procedure::return_type{});
					}
				} else if (strcmp(reply->str, "QUEUED") == 0) {
					_value = state::busy;
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
				if constexpr (std::is_same<T, std::string>::value) {
					/// @todo reply->vtype: content type (char[3]) (txt, mkd, hel)
					resolve(std::string(reply->str, reply->len));
				} else {
					return false;
				}
			} else if (REDIS_REPLY_DOUBLE == reply->type) {
				if constexpr (std::is_same<T, double>::value) {
					resolve(reply->dval);
				} else {
					return false;
				}
			} else if (REDIS_REPLY_INTEGER == reply->type) {
				if constexpr (std::is_same<T, std::int32_t>::value) {
					resolve(reply->integer);
				} else if constexpr (std::is_same<T, bool>::value) {
					resolve(reply->integer != 0);
				} else {
					return false;
				}
			} else if (REDIS_REPLY_BIGNUM == reply->type) {
				if constexpr (std::is_same<T, std::int64_t>::value) {
					try {
						_value = std::stoll(std::string(reply->str, reply->len));
					} catch (...) {
						/// @??? error
						return false;
					}
				} else {
					return false;
				}
			} else if (REDIS_REPLY_BOOL == reply->type) {
				if constexpr (std::is_same<T, bool>::value) {
					_value = (reply->integer != 0);
				} else {
					return false;
				}
			} else if (REDIS_REPLY_ARRAY == reply->type) {
				if constexpr (array_type<T>::value) {
					T result;
					for (std::size_t i = 0; i < reply->elements; ++i) {
						redisReply* item(reply->element[i]);
						if (!item) {
							return false;
						} else if constexpr (std::is_same_v<typename array_type<T>::element_type, std::string>) {
							if (item->type == REDIS_REPLY_STRING) {
								result.emplace_back(item->str, item->len);
								continue;
							} else if (item->type == REDIS_REPLY_VERB) {
								/// @todo reply->vtype: content type (char[3]) (txt, mkd, hel)
								result.emplace_back(item->str, item->len);
								continue;
							}
						} else if constexpr (std::is_same_v<typename array_type<T>::element_type, std::int64_t>) {
							if (item->type == REDIS_REPLY_INTEGER) {
								result.emplace_back(static_cast<std::int64_t>(item->integer));
								continue;
							} else if (item->type == REDIS_REPLY_BIGNUM) {
								try {
									result.emplace_back(std::stoll(std::string(item->str, item->len)));
									continue;
								} catch (...) {}
							}
						} else if constexpr (std::is_same_v<typename array_type<T>::element_type, std::int32_t>) {
							if (item->type == REDIS_REPLY_INTEGER) {
								result.emplace_back(static_cast<std::int32_t>(item->integer));
								continue;
							}
						} else if constexpr (std::is_same_v<typename array_type<T>::element_type, bool>) {
							if (item->type == REDIS_REPLY_BOOL || item->type == REDIS_REPLY_INTEGER) {
								result.emplace_back(item->integer != 0);
								continue;
							}
						} else if constexpr (std::is_same_v<typename array_type<T>::element_type, double>) {
							if (item->type == REDIS_REPLY_DOUBLE) {
								result.emplace_back(item->dval);
								continue;
							}
						}
						return false;
					}
					resolve(std::move(result));
				} else {
					return false;
				}
			} else if (REDIS_REPLY_SET == reply->type) {
				/// @todo
				/**
				 * reply->elements: number of elements (size_t)
				 * reply->element: array elements (redisReply)
				 */
				return false;
			} else if (REDIS_REPLY_MAP == reply->type) {
				/// @todo
				/**
				 * reply->elements: number of elements (size_t)
				 * reply->element: array elements (redisReply)
				 */
				return false;
			} else if (REDIS_REPLY_PUSH == reply->type) {
				/// @todo
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
		virtual status get_status() const override
		{
			if (std::holds_alternative<error>(_value)) {
				return status::fault;
			} else if (std::holds_alternative<value_type>(_value)) {
				return status::success;
			} else if (std::get<state>(_value) == state::busy) {
				return status::progress;
			}
			return status::discard;
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
			return std::holds_alternative<state>(_value) && std::get<state>(_value) == state::busy;
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
	class cursor : public function<std::vector<T>>
	{
	/** @name Properties */
	/** @{ */
	private:
		unsigned long long _offset = 0;
		std::vector<T> _accumulator;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		virtual bool resolve(redisReply* reply) override
		{
			if (!reply || reply->type != REDIS_REPLY_ARRAY || reply->elements != 2) {
				return function<std::vector<T>>::resolve(reply);
			} else if (!reply->element[0] || reply->element[0]->type != REDIS_REPLY_STRING) {
				return function<std::vector<T>>::resolve(reply);
			}
			_offset = strtoull(reply->element[0]->str, nullptr, 10);
			/// @xxx Almost a copy/paste from function::resolve()
			if (reply->element[1] && reply->element[1]->type == REDIS_REPLY_ARRAY) {
				for (std::size_t i = 0; i < reply->element[1]->elements; ++i) {
					redisReply* item(reply->element[1]->element[i]);
					if (!item) {
						return false;
					} else if constexpr (std::is_same_v<T, std::string>) {
						if (item->type == REDIS_REPLY_STRING) {
							_accumulator.emplace_back(item->str, item->len);
							continue;
						} else if (item->type == REDIS_REPLY_VERB) {
							/// @todo reply->vtype: content type (char[3]) (txt, mkd, hel)
							_accumulator.emplace_back(item->str, item->len);
							continue;
						}
					} else if constexpr (std::is_same_v<T, std::int64_t>) {
						if (item->type == REDIS_REPLY_INTEGER) {
							_accumulator.emplace_back(static_cast<std::int64_t>(item->integer));
							continue;
						} else if (item->type == REDIS_REPLY_BIGNUM) {
							try {
								_accumulator.emplace_back(std::stoll(std::string(item->str, item->len)));
								continue;
							} catch (...) {}
						}
					} else if constexpr (std::is_same_v<T, std::int32_t>) {
						if (item->type == REDIS_REPLY_INTEGER) {
							_accumulator.emplace_back(static_cast<std::int32_t>(item->integer));
							continue;
						}
					} else if constexpr (std::is_same_v<T, bool>) {
						if (item->type == REDIS_REPLY_BOOL || item->type == REDIS_REPLY_INTEGER) {
							_accumulator.emplace_back(item->integer != 0);
							continue;
						}
					} else if constexpr (std::is_same_v<T, double>) {
						if (item->type == REDIS_REPLY_DOUBLE) {
							_accumulator.emplace_back(item->dval);
							continue;
						}
					}
					_offset = 0;
					function<std::vector<T>>::resolve(error{error::code::other, "Unexpected element type in cursor reply"});
					return false;
				}
			}
			if (_offset == 0) {
				function<std::vector<T>>::resolve(std::move(_accumulator));
			}
			return true;
		}

		void flush()
		{
			_offset = 0;
			function<std::vector<T>>::resolve(std::move(_accumulator));
		}

		void cancel()
		{
			_offset = 0;
			_accumulator.clear();
			function<std::vector<T>>::resolve(nullptr);
		}
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		virtual procedure::status get_status() const override
		{
			const procedure::status base(function<std::vector<T>>::get_status());
			return (base == procedure::status::fault) ? procedure::status::fault : _offset != 0 ? procedure::status::progress : base;
		}

		const unsigned long long& get_offset() const
		{
			return _offset;
		}

		const std::vector<T>& get_accumulator() const
		{
			return _accumulator;
		}
	/** @} */
	};

	template <typename T>
	class command
	{
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

		const typename function<T>::status operator&() const
		{
			return _function->get_status();
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
