/**
 @file Command.cpp
 @date 2017-11-07
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "Command.hpp"

namespace chaos { namespace redis {
	command::command()
	:
		_reply(new reply(nullptr))
	{
		
	}
	
	command::~command()
	{
	
	}
	
	const reply* command::operator->() const
	{
		return _reply.get();
	}
	
	void command::reset_reply(std::unique_ptr<reply> reply)
	{
		_reply = std::move(reply);
	}
	
	const reply& command::get_reply() const
	{
		return *_reply.get();
	}
	
	bool command::has_reply() const
	{
		return nullptr != _reply;
	}
} }
