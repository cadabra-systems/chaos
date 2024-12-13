/**
 @file AbstractNode.cpp
 @date 22.09.15
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#include "AbstractNode.hpp"

namespace chaos { namespace cxml {
	abstract_node::abstract_node(std::initializer_list<std::shared_ptr<abstract_node>> list)
	{
		for (std::shared_ptr<abstract_node> node : list) {
			append_child(node);
		}
	}
	
	std::ostream& abstract_node::feed(std::ostream& stream) const
	{
		for (const std::list<std::shared_ptr<abstract_node>>::value_type& node : _children) {
			node->feed(stream);
		}
		
		return stream;
	}
	
	std::shared_ptr<abstract_node> abstract_node::append_child(std::shared_ptr<abstract_node> child)
	{
		if (nullptr == child) {
			return nullptr;
		}

		_children.push_back(child);
		child->set_parent(shared_from_this());
		
		return child;
	}
	
	std::shared_ptr<abstract_node> abstract_node::insert_child(std::shared_ptr<abstract_node> child, std::shared_ptr<abstract_node> before)
	{
		if (nullptr == child) {
			return nullptr;
		}
		
		if (nullptr == before) {
			return append_child(child);
		}
		
		/// @todo
		// node->setParent(shared_from_this());
		// _children.insert(, child);
		
		return child;
	}
	
	void abstract_node::remove_child(std::shared_ptr<abstract_node> child)
	{
		_children.remove(child);
	}
	
	void abstract_node::set_parent(std::shared_ptr<abstract_node> child)
	{
		_parent = child;
	}
	
	const std::shared_ptr<abstract_node> abstract_node::get_parent() const
	{
		return _parent.lock();
	}
	
	bool abstract_node::is_orphan() const
	{
		return _parent.expired();
	}
	
	bool abstract_node::has_children() const
	{
		return !_children.empty();
	}

} }
