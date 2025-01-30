/**
 @file RealNumericAttribute.hpp
 @date 22.09.15
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#ifndef Chaos_CXML_Attribute_IndexAttribute_hpp
#define Chaos_CXML_Attribute_IndexAttribute_hpp

#include "../AbstractAttribute.hpp"

#include "../../Hash.hpp"

namespace chaos { namespace cxml {
	class index_attribute : public abstract_attribute
	{
	/** @name Constants */
	/** @{ */
	public:
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		index_attribute(const std::string name, const std::uint64_t value);
		virtual ~index_attribute() = default;
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
		std::uint64_t _value;
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
		void set_value(const std::uint64_t value);
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
