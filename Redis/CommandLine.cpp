/**
 @file CommandLine.cpp
 @date 2017-11-07
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "CommandLine.hpp"

namespace chaos { namespace redis {
	
	command_line::command_line(std::string command)
	{
		push(command);
	}
	
	command_line::command_line(command_line&& origin)
	:
		_value_vector(std::move(origin._value_vector)),
		_length_vector(std::move(origin._length_vector))
	{
		_value_vector.clear();
		_length_vector.clear();
	}
	
	command_line::~command_line()
	{
		for (const char* value : _value_vector) {
			delete[] value;
		}
	}
	
	command_line::operator std::string() const
	{
		std::string retval;
		
		for (const std::string& value : _value_vector) {
			retval += value + " ";
		}
		
		return retval;
	}
	
	void command_line::push(const std::string& arg)
	{
		char* value = new char[arg.length() + 1];
		std::copy(arg.begin(), arg.end(), value);
		value[arg.size()] = '\0';
		
		_value_vector.push_back(value);
		_length_vector.push_back(arg.length());
	}
/*
	void command_line::push(const std::int64_t& arg)
	{
		push(std::to_string(arg));
	}
*/
	
	std::size_t command_line::get_count() const
	{
		return _value_vector.size();
	}
	
	command_line::value_vector::value_type* command_line::get_value_data()
	{
		return _value_vector.data();
	}
	
	command_line::length_vector::value_type* command_line::get_length_data()
	{
		return _length_vector.data();
	}

	bool command_line::is_empty() const
	{
		return _value_vector.size() == 0;
	}
} }
