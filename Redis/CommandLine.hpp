/**
 @file CommandLine.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Redis_CommandLine_hpp
#define Chaos_Redis_CommandLine_hpp

#include "Redis.hpp"

#include <vector>
#include <string>
#include <list>
#include <unordered_map>

namespace chaos { namespace redis {
	class command_line
	{
	/** @name Aliases */
	/** @{ */
	public:
		using value_vector = std::vector<const char*>;
		using length_vector = std::vector<std::size_t>;
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		command_line() = default;
		
		template<typename... TArgs>
		command_line(std::string command, TArgs ...args)
		{
			push(command, std::forward<TArgs>(args)...);
		}
		
		command_line(std::string command);
		command_line(command_line&& origin);
		virtual ~command_line();

	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		value_vector _value_vector;
		length_vector _length_vector;
	/** @} */
		
	/** @name Operators */
	/** @{ */
	public:
		operator std::string() const;
	/** @} */
		
	/** @name Procedures */
	/** @{ */
	public:
		void push(const std::string& arg);
//		void push(const std::int64_t& arg);

		template <typename T>
		typename std::enable_if<std::is_integral<T>::value, void>::type
		push(const T& arg)
		{
			push(std::to_string(arg));
		}

		template<typename T>
		void push(const typename std::unordered_map<std::string, const T>& arg)
		{
			for (const typename std::unordered_map<std::string, T>::value_type& value : arg) {
				push(value.first);
				push(value.second);
			}
		}
		
		template<typename T>
		void push(const typename std::list<T>& arg)
		{
			for (const typename std::list<T>::value_type& value : arg) {
				push(value);
			}
		}
		
		template<typename T, typename... TArgs>
		void push(const T& arg, TArgs... args)
		{
			push(arg);
			push(args...);
		}
	/** @} */
		
	/** @name Getters */
	/** @{ */
	public:
		std::size_t get_count() const;
		value_vector::value_type* get_value_data();
		length_vector::value_type* get_length_data();
	/** @} */
		
	/** @name States */
	/** @{ */
	public:
		bool is_empty() const;
	/** @} */
	};
} }

#endif
