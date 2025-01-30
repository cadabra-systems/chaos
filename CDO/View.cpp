#include "View.hpp"

namespace chaos { namespace cdo {

	view::view(const std::string& name)
	:
	  row_set(name)	{	}

	std::vector<std::shared_ptr<abstract_field> > view::get_fields() const
	{
		return _fields;
	}

	void view::add_field(std::shared_ptr<abstract_field> field)
	{
		_fields.push_back(field);
	}
}}
