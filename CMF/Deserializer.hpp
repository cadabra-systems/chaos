/*
 * Deserializer.cpp
 *
 *  Created on: 18.03.2011
 *	  Author: danilabagroff
 */

#ifndef Chaos_CMF_Deserializer_hpp
#define Chaos_CMF_Deserializer_hpp

#include "../AbstractDeserializer.hpp"
#include "Proto.hpp"

#include <tuple>

namespace chaos { namespace cmf {
	class deserializer final : public abstract_deserializer
	{
	/** @name Constructors */
	/** @{ */
	public:
		deserializer(std::istream& stream);
	/** @} */
		
	/** @name Procedures */
	/** @{ */
	private:
		value_type read_type();
		
		std::string read_string();
		std::uint16_t read_unsigned_short_integer();
		std::int16_t read_short_integer();
		std::uint32_t read_unsigned_integer();
		std::int32_t read_integer();
		std::int32_t read_var_integer();
		std::int64_t read_var_long_integer();
		std::uint64_t read_unsigned_long_integer();
		std::int64_t read_long_integer();
		double read_doudle();
		bool read_boolean();
		std::chrono::system_clock::time_point read_datetime();

	public:
		virtual std::int64_t read_id() override;
		virtual std::uint32_t read_size() override;
		virtual std::uint64_t read_index() override;
		virtual std::string read_blob() override;
	/** @} */
	
	/** @name Operators */
	/** @{ */
	public:
		virtual void operator>>(std::string& value) override;
		virtual void operator>>(std::uint16_t& value) override;
		virtual void operator>>(std::int16_t& value) override;
		virtual void operator>>(std::uint32_t& value) override;
		virtual void operator>>(std::int32_t& value) override;
		virtual void operator>>(std::uint64_t& value) override;
		virtual void operator>>(std::int64_t& value) override;
		virtual void operator>>(double& value) override;
		virtual void operator>>(bool& value) override;
		virtual void operator>>(std::chrono::system_clock::time_point& value) override;
/*
		template <typename TValue>
		void operator>>(std::vector<TValue>& value)
		{
			const std::int32_t length(Integer() >> 1);

			if (length <= 0) {
				return ;
			}
			
			ValueType type(ValueType());

			if (CMF::getValueType(value.begin()) != type) {
				throw (ExceptionProvoker<Exception>() << CMF::Exception::Code::AmbiguousData << "Value expected to be: " << type << " type of");
			}

			for (std::int32_t i=0; i<length; i++) {
				TValue v;
				this >> v;
				value.push_back(value);
			}
		};
		
		template <typename TValue>
		void operator>>(std::map<std::string, TValue>& value)
		{
			if (value.size() <= 0) {
				return ;
			}
			
			ValueType type(ValueType());
			
			if (CMF::getValueType(value.begin().second) != type) {
				throw (ExceptionProvoker<Exception>() << CMF::Exception::Code::AmbiguousData << "Value expected to be: " << type << " type of");
			}
			
			std::string key;
			while (!(key = String()).empty()) {
				TValue v;
				this >> v;
				value[key] = v;
			}
		};
*/ 
	/** @} */
	};
} }

#endif
