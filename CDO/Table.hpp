/**
 @file Table.hpp
 @date 22.05.01
 @copyright Cadabra Systems
 @author danil@megrabyan.pro
 */

#ifndef Chaos_CDO_RelationalTable_hpp
#define Chaos_CDO_RelationalTable_hpp

#include "FieldSet.hpp"

#include <map>
#include <set>
#include <string>
#include <cstdint>

namespace chaos { namespace cdo {
	/**
	 * @brief
	 * @todo Keep field order!
	 */
	class table
	{
	/** @name Constructors */
	/** @{ */
	public:
		table();
		virtual ~table();
	/** @} */
	/** @name Properties */
	/** @{ */
	private:
		field_set _field_set;
	/** @} */

	/** @name Mutators */
	/** @{ */
	protected:
		const integer_field& field_integer(const std::string& name, bool nullable = true, std::int32_t value = 0);
/*
		 const biginteger_field& field_biginteger(const std::string& name, bool nullable = true, std::int64_t value = 0);
		 const string_field& field_string(const std::string& name, std::size_t length = 0, bool nullable = true, const std::string& value = "");
*/
	/** @} */
	};
} }

#endif
