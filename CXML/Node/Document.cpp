/**
 @file Document.cpp
 @date 26.02.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
*/

#include "Document.hpp"

namespace chaos { namespace cxml {
	document::document(const std::string name)
	:
		abstract_node({}),
		_name(name)
	{
		
	}
	
	std::ostream& document::feed(std::ostream& stream) const
	{
		stream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
		stream << "<" << _name << " xmlns:cx=\"http://www.cadabra.systems/cxml/2017\">" << std::endl;
		abstract_node::feed(stream);
		stream << "</" << _name << ">" << std::endl;
		
		return stream;
	}
		
	const std::shared_ptr<abstract_node> document::get_parent() const
	{
		return nullptr;
	}
	
	bool document::is_orphan() const
	{
		return true;
	}
} }
