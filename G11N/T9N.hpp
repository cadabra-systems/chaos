/**
 @file T9N.hpp
 @date 2022-05-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_G11N_T9N_hpp
#define Chaos_G11N_T9N_hpp

#include "L10N.hpp"

#include "../Date.hpp"
#include "../Gender.hpp"

#include <unordered_map>

#include <unicode/fmtable.h>

namespace chaos { namespace g11n {
	class t9n
	{
	/** @name Constructors */
	/** @{ */
	public:
		t9n(const std::string& resource_path, const std::string& class_name, const std::string& locale);
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		std::shared_ptr<l10n> _resource_table;
		std::unordered_map<std::string, icu::Formattable> _named_arguments;

		std::string _input;
	/** @} */

	/** @name Procedures */
	/** @{ */
	private:
		std::string manual_process() const;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		operator std::string() const;
		t9n& operator()(const std::string& phrase);

		t9n& operator()(const std::string& key, int number);
		t9n& operator()(const std::string& key, double number);
		t9n& operator()(const std::string& key, const std::string& string);
		t9n& operator()(const std::string& key, const chaos::date& date);
		t9n& operator()(const std::string& key, gender gender);
	/** @} */
	};
} }
#endif
