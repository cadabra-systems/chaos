/**
 @file Memento.hpp
 @date 09.03.17
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#ifndef Chaos_Money_hpp
#define Chaos_Money_hpp

#include "SafePointer.hpp"
#include "AtomicHashTable.hpp"

#include <map>
#include <set>
#include <vector>
#include <string>

namespace chaos {
	class memento
	{
	/** @name Classes */
	/** @{ */
	public:
		enum class object_type : char
		{
			null = 0,
			value = 'v',
			list = 'L',
			set = 'S',
			map = 'M',
		};

		class object
		{
		public:
			object(object_type type = object_type::null);
			~object() = default;

		private:
			object_type _type;
			union {
				void* _null;
				std::string* _value;
				std::vector<std::string>* _list;
				std::set<std::string>* _set;
				std::map<std::string, std::string>* _map;
			};

		public:
			object_type get_type() const;
			std::string* get_value() const;
			std::vector<std::string>* get_vector() const;
			std::set<std::string>* get_set() const;
			std::map<std::string, std::string>* get_map() const;

		public:
			object_type set_type(object_type type);
		};
	/** @} */

	/** @name Aliases */
	/** @{ */
	public:
		using collection = chaos::atomic_hash_table<chaos::safe_ptr<object>>;
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		memento() = default;
		~memento() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		collection _collection;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		bool reset(const std::string& key);
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		chaos::safe_ptr<object> get(const std::string& key) const;
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
		chaos::safe_ptr<object> set(const std::string& key, object_type type);
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool is_exists(const std::string& key) const;
		bool is_null(const std::string& key) const;
	/** @} */
	};
}

#endif
