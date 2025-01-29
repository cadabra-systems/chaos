/**
 @file AbstractField.hpp
 @date 22.05.01
 @copyright Cadabra Systems
 @author danil@megrabyan.pro
 */

#ifndef Chaos_CDO_AbstractField_hpp
#define Chaos_CDO_AbstractField_hpp

#include <string>

namespace chaos { namespace cdo {

	class big_signed_integer;
	class signed_integer;
	class string;

	class abstract_field
	{
		/** @name Classes */
		/** @{ */
		public:
			enum class fieldType {
				big_signed_integer = 0,
				signed_integer,
				string,
			};
		/** @} */
	/** @name Constructors */
	/** @{ */
	public:
		abstract_field(const std::string& name, const std::string& alias, bool nullable = true);
		abstract_field(const std::string& name, const std::string& alias, const std::string& rawExpression, bool nullable = true);
		abstract_field(abstract_field&) = delete;
		abstract_field& operator=(abstract_field&) = delete;
		virtual ~abstract_field() = 0;
	/** @} */

	/** @name Properties */
	/** @{ */
	protected:
		std::string _alias;
		std::string _name;
		std::string _tableAlias;
		bool _nullable;
		bool _isRawExpression;
		std::string _expression;
		fieldType _type;
	/** @} */

	/** @name Procedures */
	/** @{ */
	protected:
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		std::string alias() const;
		std::string expression() const;
		std::string name() const;
		std::string tableAlias() const;
		bool is_nullable() const;
		bool is_rawExression() const;
		abstract_field::fieldType field_type() const {return _type;};

	/** @} */
	/** @name Setters */
	/** @{ */
	public:
		void set_alias(const std::string& alias);
		void set_expression(const std::string& exp);
		void set_name(const std::string& name);
		void set_tableAlias(const std::string& tbl);
		void set_nullable(bool nullable);
	/** @} */

	};
} }

#endif
