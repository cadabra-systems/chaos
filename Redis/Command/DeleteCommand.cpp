/**
 @file DeleteCommand.cpp
 @date 2026-04-04
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "DeleteCommand.hpp"

namespace chaos { namespace redis {
	delete_command::delete_command(std::string key)
	:
		function<std::int32_t>(),

		_key_vector({std::move(key)})
	{

	}

	delete_command::delete_command(std::vector<std::string> key_vector)
	:
		function<std::int32_t>(),

		_key_vector(std::move(key_vector))
	{

	}

	procedure::status delete_command::execute(redisContext* context)
	{
		std::vector<const char*> value_vector(_key_vector.size() + 1);
		std::vector<std::size_t> length_vector(_key_vector.size() + 1);
		value_vector[0] = "DEL"; length_vector[0] = 3;
		for (std::size_t i = 0; i < _key_vector.size(); ++i) {
			value_vector[i + 1] = _key_vector[i].data();
			length_vector[i + 1] = _key_vector[i].size();
		}
		return procedure::execute(context, static_cast<int>(_key_vector.size() + 1), value_vector.data(), length_vector.data());
	}
} }
