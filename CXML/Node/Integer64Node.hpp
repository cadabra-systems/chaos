/**
 @file Integer64Node.hpp
 @date 22.09.15
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#ifndef Chaos_CXML_Node_Integer64Node_hpp
#define Chaos_CXML_Node_Integer64Node_hpp

#include "../AbstractNode.hpp"

#include <string>

namespace chaos { namespace cxml {
	class integer64_node : public abstract_node
	{
	/** @name Constants */
	/** @{ */
	public:
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		integer64_node(const std::int64_t value);
		virtual ~integer64_node() = default;
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
		std::int64_t _value;
	/** @} */
		
	/** @name Procedures  */
	/** @{ */
	public:
		virtual std::ostream& feed(std::ostream& stream) const override final;
	/** @} */
		
	/** @name Converters */
	/** @{ */
	public:
	/** @} */
	
	/** @name Setters */
	/** @{ */
	public:
		void set_value(int value);
	/** @} */
		
	/** @name Getters */
	/** @{ */
	public:
		int get_value() const;
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
