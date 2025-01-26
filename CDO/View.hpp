/**
 @file View.hpp
 @date 17.12.2024
 @copyright Cadabra Systems
 @author avlukyanov01@gmail.com
 */

#ifndef Chaos_CDO_View_hpp
#define Chaos_CDO_View_hpp

#include "RowSet.hpp"

#include "AbstractField.hpp"
#include <vector>

namespace chaos { namespace cdo {

	class view : public row_set
	{
	/** @name Constructors */
	/** @{ */
	public:
		view(const std::string& name);
		view(const view&) = default;
		view& operator=(view&) = delete;
		virtual ~view() override = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		std::vector<std::shared_ptr<abstract_field>> _fields;
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		 virtual std::vector<std::shared_ptr<abstract_field>> get_fields() const override;
	/** @} */

	/** @name Mutators */
	/** @{ */
	public:
		virtual void add_field(std::shared_ptr<abstract_field> field) override;
	/** @} */
	};

}}

#endif // VIEW_HPP
