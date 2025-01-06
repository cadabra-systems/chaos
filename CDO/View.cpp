#include "View.hpp"
#include <stdexcept>

namespace chaos { namespace cdo {

view::view(const std::string& name)
	: row_set(name)
{
}

const std::vector<std::shared_ptr<abstract_field>>& view::get_fields() const
{
	return _fields;
}

void view::add_field(std::shared_ptr<abstract_field> field)
{
	if (!field) {
		throw std::invalid_argument("add_field(...) - null field pointer");
	}
	_fields.push_back(field);
}

}}
