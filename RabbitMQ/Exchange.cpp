/**
 @file Exchange.cpp
 @date 2026-04-22
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "Exchange.hpp"

namespace chaos { namespace rabbitmq {
	const chaos::bimap<exchange::type, std::string> exchange::type_map =
	{
		{
			{exchange::type::fanout, "fanout"},
			{exchange::type::direct, "direct"},
			{exchange::type::topic, "topic"},
			{exchange::type::meta, "headers"}
		},
		exchange::type::direct
	};
} }
