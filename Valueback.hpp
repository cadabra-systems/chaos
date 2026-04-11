/**
 * @file Valueback.hpp
 * @date 2014-01-01
 * @copyright Cadabra Systems
 * @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_Valueback_hpp
#define Chaos_Valueback_hpp

#include <memory>

#include <functional>

namespace chaos {
	template <typename T>
	class valueback : public std::enable_shared_from_this<valueback<T>>
	{
	/** @name Classes */
	/** @{ */
	private:
		struct secret
		{
		friend class valueback;
		public:
			template<typename... TArgs>
			explicit secret(TArgs&&... args) : value(std::forward<TArgs>(args)...) {}
			~secret() = default;

		private:
			T value;
		};
	/** @} */

	/** @name Statics */
	/** @{ */
	public:
		template <typename ...TArgs>
		static std::shared_ptr<valueback<T>> make_shared(TArgs&& ...args)
		{
			return std::make_shared<valueback<T>>(secret(std::forward<TArgs>(args)...));
		}
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		valueback(secret&& secret)
		:
			_value(secret.value)
		{

		}
		valueback(valueback<T>&& origin) = delete;
		valueback(const valueback<T>&) = delete;
		virtual ~valueback() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		T _value;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		valueback& operator=(valueback<T>&& origin) = delete;
		valueback& operator=(const valueback<T>&) = delete;
	/** @} */

	/** @name Factories */
	/** @{ */
	public:
		std::reference_wrapper<T> make_reference()
		{
			return std::ref(_value);
		}

		std::reference_wrapper<const T> make_reference() const
		{
			return std::cref(_value);
		}

		std::function<void(const T&)> make_delegate()
		{
			std::weak_ptr<valueback<T>> weak(std::enable_shared_from_this<valueback<T>>::weak_from_this());
			return [weak](const T& value)
			{
				if (std::shared_ptr<valueback<T>> shared = weak.lock()) {
					shared->_value = value;
				}
			};
		}

		std::function<void()> make_delegate(const T& default_value)
		{
			std::weak_ptr<valueback<T>> weak(std::enable_shared_from_this<valueback<T>>::weak_from_this());
			return [weak, default_value]()
			{
				if (std::shared_ptr<valueback<T>> shared = weak.lock()) {
					shared->_value = default_value;
				}
			};
		}

		std::function<void()> make_delegate(T&& default_value)
		{
			std::weak_ptr<valueback<T>> weak(std::enable_shared_from_this<valueback<T>>::weak_from_this());
			return [weak, default_value]()
			{
				if (std::shared_ptr<valueback<T>> shared = weak.lock()) {
					shared->_value = default_value;
				}
			};
		}

		template<typename... FArgs>
		std::function<void(FArgs...)> make_delegate(T default_value)
		{
			std::weak_ptr<valueback<T>> weak(std::enable_shared_from_this<valueback<T>>::weak_from_this());
			return [weak, default_value](FArgs&&...)
			{
				if (auto shared = weak.lock()) {
					shared->_value = default_value;
				}
			};
		}

		template<typename... FArgs>
		std::function<void(FArgs...)> make_delegate(T&& default_value)
		{
			std::weak_ptr<valueback<T>> weak(std::enable_shared_from_this<valueback<T>>::weak_from_this());
			return [weak, default_value = std::move(default_value)](FArgs&&...)
			{
				if (auto shared = weak.lock()) {
					shared->_value = default_value;
				}
			};
		}
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		T& get()
		{
			return _value;
		}

		const T& get() const
		{
			return _value;
		}
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
		void set(const T& value)
		{
			_value = value;
		}

		void set(T&& value)
		{
			_value = std::move(value);
		}
	/** @} */
	};
}

#endif
