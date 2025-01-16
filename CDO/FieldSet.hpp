// /**
//  @file FieldSet.hpp
//  @date 22.05.01
//  @copyright Cadabra Systems
//  @author danil@megrabyan.pro
//  */

// #ifndef Chaos_CDO_FieldSet_hpp
// #define Chaos_CDO_FieldSet_hpp

// #include "Field/IntegerField.hpp"

// #include <map>
// #include <set>
// #include <memory>
// #include <string>
// #include <cstdint>

// namespace chaos { namespace cdo {
// 	/**
// 	 * @brief The field_set class
// 	 * @todo Keep field order!
// 	 */
// 	class field_set
// 	{
// 	/** @name Constructors */
// 	/** @{ */
// 	public:
// 		struct comparator
// 		{
// 			using is_transparent = void;
// 			inline bool operator()(const std::unique_ptr<abstract_field>& lhs, const std::unique_ptr<abstract_field>& rhs) const;
// 		};
// 	/** @} */

// 	/** @name Constructors */
// 	/** @{ */
// 	public:
// 		field_set();
// 		virtual ~field_set();
// 	/** @} */

// 	/** @name Properties */
// 	/** @{ */
// 	private:
// 		std::set<std::unique_ptr<abstract_field>, comparator> _set;
// 	/** @} */

// 	/** @name Mutators */
// 	/** @{ */
// 	protected:
// 		const signed_integer& operator()(const std::string& name, bool nullable = true, std::int32_t value = 0);
// /*
// 		 const biginteger_field& operator()(const std::string& name, bool nullable = true, std::int64_t value = 0);
// 		 const string_field& operator()(const std::string& name, std::size_t length = 0, bool nullable = true, const std::string& value = "");
// */
// 	/** @} */
// 	};
// } }

// #endif
