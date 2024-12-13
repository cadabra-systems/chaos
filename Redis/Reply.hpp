/**
 @file Reply.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Redis_Reply_hpp
#define Chaos_Redis_Reply_hpp

#include "Redis.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace chaos { namespace redis {
	class error;
	
	class reply
	{
	friend error;
		
	/** @name Classes */
	/** @{ */
	public:
		enum class type : std::uint8_t
		{
			empty = 0,
			nil = REDIS_REPLY_NIL,
			error = REDIS_REPLY_ERROR,
			string = REDIS_REPLY_STRING,
			array = REDIS_REPLY_ARRAY,
			integer = REDIS_REPLY_INTEGER,
			status = REDIS_REPLY_STATUS,
		};
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		reply(redisReply* handle, bool root = true);
		reply(reply&& origin);
		reply(reply const&) = delete;
		reply& operator=(reply const&) = delete;
		virtual ~reply();
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		redisReply* _handle;
		const bool _root;
	/** @} */
		
	/** @name Operators */
	/** @{ */
	public:
		operator std::size_t() const;
		reply operator[](std::size_t i) const;
		operator std::vector<reply>() const;
		operator std::int64_t() const;
		operator std::string() const;
	/** @} */
		
	/** @name Factories */
	/** @{ */
	public:
		std::int64_t get_integer() const;
		std::string get_string() const;
		std::string get_error_string() const;
		std::size_t get_count() const;
		reply get_element(std::size_t i) const;
		std::vector<reply> get_array() const;
	/** @} */
		
	/** @name Getters */
	/** @{ */
	public:
		reply::type get_type() const;
	/** @} */
		
	/** @name States */
	/** @{ */
	public:
		bool is_faulty() const;
		bool is_empty() const;
		bool is_nil() const;
	/** @} */
	};
	
	bool operator==(const reply& lhs, const reply::type& rhs);	
	bool operator!=(const reply& lhs, const reply::type& rhs);
} }

#endif
