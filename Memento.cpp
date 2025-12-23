/**
 @file Memento.cpp
 @date 09.03.17
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#include "Memento.hpp"

namespace chaos {
	memento::object::object(object_type type)
	:
		_type(object_type::null),
		_null(nullptr)
	{
		set_type(type);
	}

	memento::object_type memento::object::get_type() const
	{
		return _type;
	}

	std::string* memento::object::get_value() const
	{
		return object_type::value == _type ? _value : nullptr;
	}

	std::vector<std::string>* memento::object::get_vector() const
	{
		return object_type::list == _type ? _list : nullptr;
	}

	std::set<std::string>* memento::object::get_set() const
	{
		return object_type::set == _type ? _set : nullptr;
	}

	std::map<std::string, std::string>* memento::object::get_map() const
	{
		return object_type::map == _type ? _map : nullptr;
	}

	memento::object_type memento::object::set_type(object_type type)
	{
		object_type retval(object_type::null);
		switch (_type) {
			case object_type::value:
				retval = object_type::value;
				delete _value;
				break;
			case object_type::list:
				retval = object_type::list;
				delete _list;
				break;
			case object_type::set:
				retval = object_type::set;
				delete _set;
				break;
			case object_type::map:
				retval = object_type::map;
				delete _map;
				break;
			default:
				retval = object_type::null;
				break;
		}
		switch (type) {
			case object_type::value:
				_value = new std::string();
				break;
			case object_type::list:
				_list = new std::vector<std::string>();
				break;
			case object_type::set:
				_set = new std::set<std::string>();
				break;
			case object_type::map:
				_map = new std::map<std::string, std::string>();
				break;
			default:
				_null = nullptr;
				break;
		}
		_type = type;
		return retval;
	}

	bool memento::reset(const std::string& key)
	{
		return _collection.erase(std::hash<std::string>()(key)) == 1;
	}

	chaos::safe_ptr<memento::object> memento::get(const std::string& key) const
	{
		memento::collection::const_iterator o(_collection.find(std::hash<std::string>()(key)));
		return _collection.cend() == o ? nullptr :  o->second;
	}

	chaos::safe_ptr<memento::object> memento::set(const std::string& key, object_type type)
	{
		return _collection.insert(std::hash<std::string>()(key), chaos::make_safe<memento::object>(type), true).first->second;
	}

	bool memento::is_exists(const std::string& key) const
	{
		return _collection.cend() != _collection.find(std::hash<std::string>()(key));
	}

	bool memento::is_null(const std::string& key) const
	{
		memento::collection::const_iterator o(_collection.find(std::hash<std::string>()(key)));
		return _collection.cend() == o || o->second->get_type() == object_type::null;
	}
}
