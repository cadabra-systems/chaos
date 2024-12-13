/**
 @file Reply.cpp
 @date 2017-11-07
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "Reply.hpp"

#include <utility>

namespace chaos { namespace redis {
	reply::reply(redisReply* handle, bool root)
	:
		_handle(handle),
		_root(root)
	{
	}
	
	reply::reply(reply&& origin)
	:
		_handle(std::move(origin._handle)),
		_root(origin._root)
	{
		origin._handle = nullptr;
	}
	
	reply::~reply()
	{
		if (_root) {
			freeReplyObject(_handle);
		}
	}
		
	reply::type reply::get_type() const
	{
		return nullptr == _handle ? reply::type::empty : static_cast<reply::type>(_handle->type);
	}
	
	reply::operator std::size_t() const
	{
		return get_count();
	}
	
	reply reply::operator[](std::size_t i) const
	{
		return get_element(i);
	}
	
	reply::operator std::vector<reply>() const
	{
		return get_array();
	}
	
	reply::operator std::int64_t() const
	{
		return get_integer();
	}
	
	reply::operator std::string() const
	{
		return get_string();
	}
	
	std::int64_t reply::get_integer() const
	{
		return (get_type() == reply::type::integer) ? _handle->integer : 0;
	}
	
	std::string reply::get_string() const
	{
		return (get_type() == reply::type::string) ? std::string(_handle->str, _handle->len) : "";
	}
	
	std::string reply::get_error_string() const
	{
		return (get_type() == reply::type::error) ? std::string(_handle->str, _handle->len) : "";
	}
	
	std::size_t reply::get_count() const
	{
		return (get_type() == reply::type::array) ? static_cast<std::size_t>(_handle->elements) : 0;
	}
	
	reply reply::get_element(std::size_t i) const
	{
		if (get_type() != reply::type::array) {
			return reply(nullptr);
		}
		
		if (get_count() <= i) {
			return reply(nullptr);
		}
		
		return reply(_handle->element[i], false);
	}
	
	std::vector<reply> reply::get_array() const
	{
		std::vector<reply> retval;
		if (get_type() != reply::type::array) {
			return retval;
		}
		
		for (std::size_t i=0; i<_handle->elements; i++) {
			retval.emplace_back(_handle->element[i], false);
		}
		
		return retval;
	}
	
	bool reply::is_empty() const
	{
		return (nullptr == _handle);
	}
	
	bool reply::is_faulty() const
	{
		if (is_empty()) {
			return false;
		}
		
		return (_handle->type == REDIS_REPLY_ERROR);
	}
	
	bool reply::is_nil() const
	{
		if (is_empty()) {
			return false;
		}
		
		return (_handle->type == REDIS_REPLY_NIL);
	}
	
	bool operator==(const reply& lhs, const reply::type& rhs)
	{
		return lhs.get_type() == rhs;
	};
	
	bool operator!=(const reply& lhs, const reply::type& rhs)
	{
		return lhs.get_type() != rhs;
	};
} }

