/**
 @file Heisenbug.hpp
 @date 2022-04-15
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Heisenbug_hpp
#define Chaos_Heisenbug_hpp

#include "Log.hpp"
#include "String.hpp"
#include "LogRegister.hpp"

#include <type_traits>
#include <exception>
#include <string>
#include <chrono>
#include <map>

namespace chaos {
	/**
	 * @class Base class to unite all units of a case
	 * @brief Heisenberg's Uncertainty Principle states that there is inherent uncertainty in the act
	 * of measuring a variable of a particle. Commonly applied to the position and momentum of a particle,
	 * the principle states that the more precisely the position is known the more uncertain the momentum is and vice versa.
	 */
	class heisen_test
	{
	/** @name Statics */
	/** @{ */
	public:
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		heisen_test(const std::string& name)
		:
			_name(name)
		{
			/// @todo timepoint
		}

		virtual ~heisen_test()
		{

		}
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		const std::string _name;
		std::string _mask;
		std::map<log_level, std::size_t> _stat_map;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		virtual void run(const std::string& mask) final
		{
			_stat_map.clear();
			_mask = mask;
			if (_mask.empty()) {
				return ;
			}

			const std::chrono::time_point<std::chrono::system_clock> start_timepoint(std::chrono::system_clock::now());
			tearup();
			try {
				tear();
			} catch(...) {
				/// @??? catch std::terminate() from bug::inspect?
			}
			teardown();
			const std::chrono::time_point<std::chrono::system_clock> stop_timepoint(std::chrono::system_clock::now());

			_stat_map.try_emplace(log_level::dead, 0);
			_stat_map.try_emplace(log_level::fatal, 0);
			_stat_map.try_emplace(log_level::critical, 0);
			_stat_map.try_emplace(log_level::error, 0);
			_stat_map.try_emplace(log_level::alert, 0);
			_stat_map.try_emplace(log_level::warning, 0);
			_stat_map.try_emplace(log_level::notice, 0);
			_stat_map.try_emplace(log_level::checkpoint, 0);
			_stat_map.try_emplace(log_level::debug, 0);

			log_register<log>::checkpoint("chaos\\heisenbug::run::", _name, "::duration::", std::chrono::duration_cast<std::chrono::milliseconds>(stop_timepoint - start_timepoint).count());
			log_register<log>::checkpoint("chaos\\heisenbug::run::", _name, "::dead::", _stat_map.at(log_level::dead));
			log_register<log>::checkpoint("chaos\\heisenbug::run::", _name, "::fatal::", _stat_map.at(log_level::fatal));
			log_register<log>::checkpoint("chaos\\heisenbug::run::", _name, "::critical::", _stat_map.at(log_level::critical));
			log_register<log>::checkpoint("chaos\\heisenbug::run::", _name, "::error::", _stat_map.at(log_level::error));
			log_register<log>::checkpoint("chaos\\heisenbug::run::", _name, "::alert::", _stat_map.at(log_level::alert));
			log_register<log>::checkpoint("chaos\\heisenbug::run::", _name, "::warning::", _stat_map.at(log_level::warning));
			log_register<log>::checkpoint("chaos\\heisenbug::run::", _name, "::notice::", _stat_map.at(log_level::notice));
			log_register<log>::checkpoint("chaos\\heisenbug::run::", _name, "::checkpoint::", _stat_map.at(log_level::checkpoint));
			log_register<log>::checkpoint("chaos\\heisenbug::run::", _name, "::debug::", _stat_map.at(log_level::debug));
		}

		void stat(log_level level, const std::string& message)
		{
			_stat_map[level] += 1;
			log_register<log>::log(level, message);
		}

	protected:
		virtual void tearup()
		{

		}

		virtual void tear()
		{

		}

		virtual void teardown()
		{

		}
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		virtual const std::string& get_name() const
		{
			return _name;
		}

		virtual const std::string& get_mask() const
		{
			return _mask;
		}
	};

	/**
	 * @brief default one
	 * @return
	 */
	inline heisen_test& heisen_singleton()
	{
		static heisen_test retval("heisenberg");
		return retval;
	}

	/**
	 * @brief Wrapper on a case to collect errors, warnings and measure performance.
	 */
	template <typename T>
	class heisen_unit
	{
	/** @name Constructors */
	/** @{ */
	public:
		heisen_unit(const std::string& name, T* object, void(T::* method)())
		:
			_name(name),
			_object(object),
			_method(method)
		{
			/// @todo timepoint
		}

		virtual ~heisen_unit()
		{
			/// @todo timepoint
		}
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		const std::string _name;
		T* _object;
		void(T::* _method)();
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		void inspect()
		{
			inspect(*_object);
		}

		void inspect(heisen_test& t)
		{
			log_register<log>::checkpoint("chaos\\heisenbug::inspect::", _name, "::start");
			try {
				std::invoke(_method, _object);
				log_register<log>::checkpoint("chaos\\heisenbug::inspect::", _name, "::finish");
				return ;
			} catch (const std::exception& e) {
				t.stat(log_level::dead, "Uncaught exception: " + std::string(e.what()));
			} catch (...) {
				t.stat(log_level::dead, "Unknown exception");
			}
			log_register<log>::checkpoint("chaos\\heisenbug::inspect::", _name, "::abort");
		}
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		const std::string& get_name() const
		{
			return _name;
		}
	/** @} */
	};

	template <typename T>
	constexpr std::enable_if_t<std::is_convertible<T*, heisen_test*>::value, void>
	heisen_berg(heisen_unit<T>& u)
	{
		u.inspect();
	}

	template <typename T>
	constexpr std::enable_if_t<!std::is_convertible<T*, heisen_test*>::value, void>
	heisen_berg(heisen_unit<T>& u)
	{
		u.inspect(heisen_singleton());
	}

	template <typename T>
	constexpr std::enable_if_t<std::is_convertible<T*, heisen_test*>::value, bool>
	heisen_match(T* t, const char* c)
	{
		return chaos::string::wild_match(c, t->get_mask());
	}

	template <typename T>
	constexpr std::enable_if_t<!std::is_convertible<T*, heisen_test*>::value, bool>
	heisen_match(T* t, const std::string& s)
	{
		return chaos::string::wild_match(s.data(), t->get_mask());
	}

	template <typename T>
	constexpr std::enable_if_t<std::is_convertible<T*, heisen_test*>::value, void>
	heisen_bug(T& t, log_level level, const std::string& message)
	{
		t.stat(level, message);
	}

	template <typename T>
	constexpr std::enable_if_t<!std::is_convertible<T*, heisen_test*>::value, void>
	heisen_bug(T& test, log_level level, const std::string& message)
	{
		heisen_singleton().stat(level, message);
	}
}

/// @todo since 17++: if constexpr (std::is_convertible<T, chaos::test*>::type) {}
#define HEISEN(method) \
if (chaos::heisen_match(this, #method)) { \
	using T = std::remove_pointer<decltype(this)>::type; \
	chaos::heisen_unit<T> unit{#method, this, &T::test##method}; \
	chaos::heisen_berg(unit); \
}

#define LOG(message) chaos::heisen_bug(*this, chaos::log_level::debug, chaos::string::formatted_string("%s at %s:%d", message, __FILE__, __LINE__));

#define BUG(message) chaos::heisen_bug(*this, chaos::log_level::error, chaos::string::formatted_string("%s at %s:%d", message, __FILE__, __LINE__));

#define IS_TRUE(expression) if (!(expression)) chaos::heisen_bug(*this, chaos::log_level::error, chaos::string::formatted_string("Assertion failed at %s:%d", __FILE__, __LINE__));
#define IS_FALSE(expression) if (expression) chaos::heisen_bug(*this, chaos::log_level::error, chaos::string::formatted_string("Assertion failed at %s:%d", __FILE__, __LINE__));

#define ARE_EQUAL(lhs, rhs) if (lhs != rhs) chaos::heisen_bug(*this, chaos::log_level::error, chaos::string::formatted_string("Assertion failed at %s:%d", __FILE__, __LINE__));
#define ARE_NOT_EQUAL(lhs, rhs) if (lhs == rhs) chaos::heisen_bug(*this, chaos::log_level::error, chaos::string::formatted_string("Assertion failed at %s:%d", __FILE__, __LINE__));

#define IS_GREATER(lhs, rhs) if (lhs <= rhs) chaos::heisen_bug(*this, chaos::log_level::error, chaos::string::formatted_string("Assertion failed at %s:%d", __FILE__, __LINE__));
#define IS_LESS_OR_EQUAL(lhs, rhs) if (lhs > rhs) chaos::heisen_bug(*this, chaos::log_level::error, chaos::string::formatted_string("Assertion failed at %s:%d", __FILE__, __LINE__));

#define IS_GREATER_OR_EQUAL(lhs, rhs) if (lhs < rhs) chaos::heisen_bug(*this, chaos::log_level::error, chaos::string::formatted_string("Assertion failed at %s:%d", __FILE__, __LINE__));
#define IS_LESS(lhs, rhs) if (lhs >= rhs) chaos::heisen_bug(*this, chaos::log_level::error, chaos::string::formatted_string("Assertion failed at %s:%d", __FILE__, __LINE__));

#endif
