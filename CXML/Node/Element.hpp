/**
 @file Element.hpp
 @date 22.09.15
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#ifndef Chaos_CXML_Node_Element_hpp
#define Chaos_CXML_Node_Element_hpp

#include "../AbstractNode.hpp"
#include "../AbstractAttribute.hpp"

#include <string>
#include <map>
#include <memory>

namespace chaos { namespace cxml {
	class element : public abstract_node
	{
	/** @name Constants */
	/** @{ */
	public:
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		element(const std::string tag, std::initializer_list<std::shared_ptr<abstract_attribute>> list = {});
		element(const std::string space, const std::string tag, std::initializer_list<std::shared_ptr<abstract_attribute>> list);
		virtual ~element() = default;
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
		const std::string _tag;
		const std::string _space;
		std::map<std::string, std::shared_ptr<abstract_attribute>> _map;
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
		void set_attribute(std::shared_ptr<abstract_attribute> attribute);
	/** @} */
		
	/** @name Getters */
	/** @{ */
	public:
		const std::string& get_tag() const;
		const std::shared_ptr<abstract_attribute> get_attribute(const std::string name) const;
	/** @} */
		
	/** @name Formatters */
	/** @{ */
	public:
	/** @} */
		
	/** @name States */
	/** @{ */
	public:
		bool has_attribute(const std::string name) const;
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
