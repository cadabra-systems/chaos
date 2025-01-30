/**
 @file Table.hpp
 @date 22.05.01
 @copyright Cadabra Systems
 @author danil@megrabyan.pro
 */

#ifndef Chaos_CDO_RelationalTable_hpp
#define Chaos_CDO_RelationalTable_hpp

#include "RowSet.hpp"

#include <string>
#include <vector>

namespace chaos { namespace cdo {
	/**
	 * @brief
	 * @todo Keep field order!
	 */
	class table : public row_set
	{
	/** @name Constructors */
	/** @{ */
	public:
		explicit table(const std::string& name);
		table(const table&) = default;
		table& operator=(table&) = delete;
		virtual ~table() override = default;
	/** @} */
	/** @name Properties */
	/** @{ */
	protected:
		std::vector<std::shared_ptr<abstract_field>> _fields;
	/** @} */

	/** @name Mutators */
	/** @{ */
	public:
		virtual std::vector<std::shared_ptr<abstract_field>> get_fields () const override;
	/** @} */

	/** @name Mutators */
	/** @{ */
	public:
		virtual void add_field(std::shared_ptr<abstract_field> field) override;
	/** @} */
	};
} }

#endif
