/**
 @file AbstractNode.hpp
 @date 22.09.15
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
 */

#ifndef Chaos_CXML_AbstractNode_hpp
#define Chaos_CXML_AbstractNode_hpp

#include <initializer_list>
#include <list>
#include <memory>
#include <ostream>

namespace chaos { namespace cxml {
	class abstract_node : public std::enable_shared_from_this<abstract_node>
	{
	/** @name Constants */
	/** @{ */
	public:
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		abstract_node(std::initializer_list<std::shared_ptr<abstract_node>> list = {});
		virtual ~abstract_node() = default;
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
		std::list<std::shared_ptr<abstract_node>> _children;
		std::weak_ptr<abstract_node> _parent;
		
	protected:
	/** @} */
		
	/** @name Procedures  */
	/** @{ */
	public:
		virtual std::ostream& feed(std::ostream& stream) const;
		
		virtual std::shared_ptr<abstract_node> append_child(std::shared_ptr<abstract_node> child);
		virtual std::shared_ptr<abstract_node> insert_child(std::shared_ptr<abstract_node> child, std::shared_ptr<abstract_node> before = nullptr);
		virtual void remove_child(std::shared_ptr<abstract_node> child);
	/** @} */
	
	/** @name Converters */
	/** @{ */
	public:
	/** @} */
	
	/** @name Setters */
	/** @{ */
	private:
		void set_parent(std::shared_ptr<abstract_node> child);
	/** @} */
		
	/** @name Getters */
	/** @{ */
	public:
		virtual const std::shared_ptr<abstract_node> get_parent() const;
	/** @} */
		
	/** @name Formatters */
	/** @{ */
	public:
	/** @} */
		
	/** @name States */
	/** @{ */
	public:
		virtual bool is_orphan() const;
		virtual bool has_children() const final;
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
