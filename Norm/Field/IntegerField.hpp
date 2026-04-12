/**
 @file IntegerField.hpp
 @date 22.05.01
 @copyright Cadabra Systems
 @author danil@megrabyan.pro
 */

#ifndef Chaos_CDO_Field_IntegerField_hpp
#define Chaos_CDO_Field_IntegerField_hpp

#include "../AbstractField.hpp"

namespace chaos { namespace cdo {
	class integer_field : public abstract_field
	{
	/** @name Constructors */
	/** @{ */
	public:
		integer_field(const std::string& name, bool nullable = true, std::int32_t value = 0);
		virtual ~integer_field() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		std::int32_t _value;
	/** @} */

	/** @name Procedures */
	/** @{ */
	protected:
	/** @} */
	};
} }

#endif
