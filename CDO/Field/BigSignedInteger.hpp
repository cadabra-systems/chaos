/**
 @file BigSignedInteger.hpp
 @date 17.12.2024
 @copyright Cadabra Systems
 @author avlukyanov01@gmail.com
 */

#ifndef Chaos_CDO_Field_BigIntegerField_hpp
#define Chaos_CDO_Field_BigIntegerField_hpp

#include "../AbstractField.hpp"

namespace chaos { namespace cdo {
	class big_signed_integer: public abstract_field
	{
	/** @name Constructors */
	/** @{ */
	public:
		big_signed_integer(const std::string& name, const std::string& alias = "", bool nullable = true, std::int64_t value = 0);
		big_signed_integer(const std::string& name, const std::string& alias = "", const std::string& rawExpression = "", bool nullable = true);
		big_signed_integer(big_signed_integer&) = delete;
		big_signed_integer& operator=(big_signed_integer&) = delete;
		virtual ~big_signed_integer() override = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		std::int64_t _value;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:

		std::int64_t get_value() const;
		void set_value(const std::int64_t& val) {_value = val;};

	/** @} */
	};
} }

#endif
