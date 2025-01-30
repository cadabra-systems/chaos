/**
 @file UIDAttribute.hpp
 @date 22.09.15
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#ifndef Chaos_CXML_Attribute_UIDAttribute_hpp
#define Chaos_CXML_Attribute_UIDAttribute_hpp

#include "../AbstractAttribute.hpp"

#include "../../UID.hpp"

#include <string>

namespace chaos { namespace cxml {
	class uid_attribute : public abstract_attribute
	{
	/** @name Constants */
	/** @{ */
	public:
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		uid_attribute(const std::string name, const chaos::uid value);
		virtual ~uid_attribute() = default;
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
		chaos::uid _value;
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
		void set_value(const chaos::uid value);
		void set_value(const chaos::uid& value);
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
