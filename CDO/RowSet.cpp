#include "RowSet.hpp"
#include <stdexcept>

namespace chaos { namespace cdo {

	row_set::row_set(const std::string& name)
	:
	  _name(name)
	{
		if(_name.empty()) {
			throw std::invalid_argument("Name cannot be empty!");
		}
	}

} }
