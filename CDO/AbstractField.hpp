/**
 @file AbstractField.hpp
 @date 22.05.01
 @copyright Cadabra Systems
 @author danil@megrabyan.pro
 */

#ifndef Chaos_CDO_AbstractField_hpp
#define Chaos_CDO_AbstractField_hpp

#include <string>

namespace chaos { namespace cdo {

	class big_signed_integer;
	class signed_integer;
	class string;

	class abstract_field
	{
	/** @name Constructors */
	/** @{ */
	public:
		abstract_field(const std::string& name, bool nullable = true);
		abstract_field(abstract_field&) = delete;
		abstract_field& operator=(abstract_field&) = delete;
		virtual ~abstract_field() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	protected:
		const std::string _name;
		const bool _nullable;
	/** @} */

	/** @name Procedures */
	/** @{ */
	protected:
	/** @} */

	/** @name Getters */
	/** @{ */
	public:

		virtual std::string to_SQL() = 0;

		const std::string& get_name() const;
		const bool is_nullable() const;

	/** @} */
	};
} }

#endif
