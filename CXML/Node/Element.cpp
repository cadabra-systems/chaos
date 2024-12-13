/**
 @file Element.cpp
 @date 22.09.15
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#include "Element.hpp"

namespace chaos { namespace cxml {
	element::element(const std::string tag, std::initializer_list<std::shared_ptr<abstract_attribute>> list)
	:
		_tag(tag)
	{
		for (std::shared_ptr<abstract_attribute> attribute : list) {
			_map.emplace(attribute->get_name(), attribute);
		}
	}
	
	element::element(const std::string space, const std::string tag, std::initializer_list<std::shared_ptr<abstract_attribute>> list)
	:
		_tag(tag),
		_space(space)
	{
		for (std::shared_ptr<abstract_attribute> attribute : list) {
			_map.emplace(attribute->get_name(), attribute);
		}
	}
	
	std::ostream& element::feed(std::ostream& stream) const
	{
		stream << "<" << (_space.empty() ? "" : _space + ":") + _tag;
		for (const std::map<std::string, std::shared_ptr<abstract_attribute>>::value_type& attribute : _map) {
			stream << " " << attribute.first << "=\"" << attribute.second->get_value() << "\"";
		}
		
		stream << ">" << std::endl;
		
		abstract_node::feed(stream);
		
		stream << "</" << (_space.empty() ? "" : _space + ":") + _tag << ">" << std::endl;
		
		return stream;
	}

	void element::set_attribute(std::shared_ptr<abstract_attribute> attribute)
	{
		_map[attribute->get_name()] = attribute;
	}

	const std::string& element::get_tag() const
	{
		return _tag;
	}
	
	const std::shared_ptr<abstract_attribute> element::get_attribute(const std::string name) const
	{
		std::map<std::string, std::shared_ptr<abstract_attribute>>::const_iterator i(_map.find(name));
		
		return (_map.end() != i) ? i->second : nullptr;
	}
	
	bool element::has_attribute(const std::string name) const
	{
		return (_map.find(name) != _map.end());
	}
} }
