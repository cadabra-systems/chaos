/**
 @file String.hpp
 @date 17.12.2024
 @copyright Cadabra Systems
 @author avlukyanov01@gmail.com
 */

#ifndef Chaos_CDO_Field_String_hpp
#define Chaos_CDO_Field_String_hpp

#include "../AbstractField.hpp"

namespace chaos { namespace cdo {
	class string: public abstract_field
	{
	/** @name Constructors */
	/** @{ */
	public:
		string(const std::string& name, const std::string& alias = "", bool nullable = true, const std::string& value = "", const uint8_t &length = 255);
		string(const std::string& name, const std::string& alias = "", const std::string& rawExpression = "", bool nullable = true);
		string(string&) = delete;
		string& operator=(string&) = delete;
		virtual ~string() override = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		std::string _value;
		uint8_t _length;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		virtual std::string to_SQL() override;

		std::string get_value() const;
		void set_value(const std::string& val);

	/** @} */
	};
} }

#endif

