/**
 @file AbstractField.cpp
 @date 22.05.01
 @copyright Cadabra Systems
 @author daniil@megrabyan.pro
*/

#include "AbstractField.hpp"

namespace chaos { namespace cdo {
	abstract_field::abstract_field(const std::string& name, const std::string& alias, bool nullable)
	:
		_alias(alias),
		_name(name),
		_nullable(nullable)
	{
		_expression = "";
		_isRawExpression = false;
	}

	abstract_field::abstract_field(const std::string& name, const std::string& alias, const std::string& rawExpression, bool nullable)
	:
		_alias(alias),
		_name(name),
		_nullable(nullable),
		_isRawExpression(!rawExpression.empty()),
		_expression(rawExpression)
	{}

	abstract_field::~abstract_field() {}

	std::string abstract_field::name() const
	{
		return _name;
	}

	bool abstract_field::is_nullable() const
	{
		return _nullable;
	}

	std::string abstract_field::alias() const
	{
		return _alias;
	}

	std::string abstract_field::expression() const
	{
		return _expression;
	}

	bool abstract_field::is_rawExression() const
	{
		return _isRawExpression;
	}

	std::string abstract_field::tableAlias() const
	{
		return _tableAlias;
	}


	void abstract_field::set_alias(const std::string& alias)
	{
		_alias = alias;
	}

	void abstract_field::set_expression(const std::string& exp)
	{
		_expression = exp;
		_isRawExpression = !exp.empty();
	}

	void abstract_field::set_name(const std::string& name)
	{
		_name = name;
	}

	void abstract_field::set_tableAlias(const std::string& tbl)
	{
		_tableAlias = tbl;
	}

	void abstract_field::set_nullable(bool nullable)
	{
		_nullable = nullable;
	}


} }
