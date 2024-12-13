/**
 @file DimensionAttribute.hpp
 @date 22.09.15
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#ifndef Chaos_CXML_Attribute_DimensionAttribute_hpp
#define Chaos_CXML_Attribute_DimensionAttribute_hpp

#include "../AbstractAttribute.hpp"

#include "../../../Chaos/Dimension.hpp"

namespace chaos { namespace cxml {
	class dimension_attribute : public abstract_attribute
	{
	/** @name Constructors */
	/** @{ */
	public:
		dimension_attribute(const std::string& name, const chaos::dimension& dimension);
		virtual ~dimension_attribute() = default;
	/** @} */
		
	/** @name Factories */
	/** @{ */
	public:
	/** @} */
		
	/** @name Operators */
	/** @{ */
	public:
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		chaos::dimension _dimension;
	/** @} */
		
	/** @name Procedures  */
	/** @{ */
	public:
	/** @} */
		
	/** @name Converters */
	/** @{ */
	public:
	/** @} */
	
	/** @name Setters */
	/** @{ */
	public:
		void set_value(const chaos::dimension::value_type value);
		void set_unit(const chaos::dimension::unit_type type);
	/** @} */
		
	/** @name Getters */
	/** @{ */
	public:
		virtual const std::string get_value() const override;
	/** @} */
		
	/** @name Formatters */
	/** @{ */
	public:
	/** @} */
		
	/** @name States */
	/** @{ */
	public:
	/** @} */
		
	/** @name Hooks */
	/** @{ */
	public:
	/** @} */
		
	/** @name Templates */
	/** @{ */
	public:
	/** @} */
	};
} }

#endif
