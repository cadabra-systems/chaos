/*
 * Serializer.cpp
 *
 *  Created on: 18.03.2011
 *	  Author: danilabagroff
 */

#ifndef Chaos_CMF_Serializer_hpp
#define Chaos_CMF_Serializer_hpp

#include "../AbstractSerializer.hpp"
#include "Proto.hpp"

#include <tuple>

namespace chaos { namespace cmf {
	class serializer final : public abstract_serializer
	{
	/** @name Constructors */
	/** @{ */
	public:
		serializer(std::ostream& stream);
	/** @} */
		
	/** @name Properties */
	/** @{ */
	/** @} */
		
	/** @name Procedures */
	/** @{ */
	private:
		void write_type(const value_type value);

		void write_string(const std::string& value);
		void write_unsigned_short_integer(const std::uint16_t value);
		void write_short_integer(const std::int16_t value);
		void write_unsigned_integer(const std::uint32_t value);
		void write_integer(const std::int32_t value);
		void write_unsigned_long_integer(const std::uint64_t value);
		void write_long_integer(const std::int64_t value);
		void write_var_integer(const std::int32_t value);
		void write_varlong_integer(const std::int64_t value);
		void write_doudle(const double value);
		void write_boolean(const bool yesno);
		void write_datetime(const std::chrono::system_clock::time_point& value);
		
	public:
		virtual void write_id(std::int64_t id) override;
		virtual void write_size(const std::uint32_t& size) override;
		virtual void write_index(const std::uint64_t& index) override;
		virtual void write_blob(const std::string& value) override;
	/** @} */
		
	/** @name Operators */
	/** @{ */
	public:
		virtual void operator<<(const std::string& value) override;
		virtual void operator<<(const std::uint8_t& value) override;
		virtual void operator<<(const std::uint16_t& value) override;
		virtual void operator<<(const std::int16_t& value) override;
		virtual void operator<<(const std::uint32_t& value) override;
		virtual void operator<<(const std::int32_t& value) override;
		virtual void operator<<(const std::uint64_t& value) override;
		virtual void operator<<(const std::int64_t& value) override;
		virtual void operator<<(const double& value) override;
		virtual void operator<<(const bool& value) override;
		virtual void operator<<(const std::chrono::system_clock::time_point& value) override;

		template <typename T>
		void operator<<(const std::vector<T>& value)
		{
			write_integer((value.size() << 1) | 0x01);

			if (value.size() <= 0) {
				return ;
			}

			value_type(get_value_type(*(value.begin())));
			
			for (const T& v : value) {
				operator<<(v);
			}
		};
		
		template <typename T>
		void operator<<(const std::map<std::string, T>& value)
		{
			if (value.size() <= 0) {
				return ;
			}
			
			value_type(get_value_type(*(value.begin().second())));
			
			for (const T& v : value) {
				string(v.first());
				operator<<(v.secod());
			}
			
			write_string("");
		};
	/** @} */
	};
} }

#endif
