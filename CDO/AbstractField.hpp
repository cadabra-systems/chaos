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
	class abstract_field
	{
	/** @name Constructors */
	/** @{ */
	public:
		abstract_field(const std::string& name, bool nullable = true);
		virtual ~abstract_field() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
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
		const std::string& get_name() const;
	/** @} */
	};
} }

#endif
