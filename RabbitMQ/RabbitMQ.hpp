/**
 @file RabbitMQ.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_RabbitMQ_RabbitMQ_hpp
#define Chaos_RabbitMQ_RabbitMQ_hpp

#include <rabbitmq-c/amqp.h>

#include <atomic>

namespace chaos { namespace rabbitmq {
	using state = std::atomic<amqp_connection_state_t>;
} }

#endif
