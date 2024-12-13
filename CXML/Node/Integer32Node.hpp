/**
 @file Integer32Node.hpp
 @date 22.09.15
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#ifndef Chaos_CXML_Node_Integer32Node_hpp
#define Chaos_CXML_Node_Integer32Node_hpp

#include "../AbstractNode.hpp"

#include <string>

namespace chaos { namespace cxml {
	class integer32_node : public abstract_node
	{
	/** @name Constants */
	/** @{ */
	public:
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		integer32_node(const std::int32_t value);
		virtual ~integer32_node() = default;
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
		std::int32_t _value;
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
