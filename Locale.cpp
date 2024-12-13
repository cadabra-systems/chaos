
//
//  Locale.cpp
//
//  Created by Daniil A Megrabyan on 14.02.17.
//  Copyright © 2017 Cadabra Systems. All rights reserved.
//

#include "Locale.hpp"

#include "String.hpp"
#include <vector>

namespace chaos {
	const bimap<std::string, locale::language> locale::language_map = {
		{
			{"xx", locale::language::undefined},
			{"en", locale::language::english},
			{"ru", locale::language::russian},
			{"de", locale::language::german},
			{"es", locale::language::spanish}
		},
		locale::language::undefined
	};

	const bimap<std::string, locale::country> locale::country_map = {
		{
			{"xx", locale::country::undefined},
			{"am", locale::country::armenia},
			{"ge", locale::country::georgia},
			{"il", locale::country::israel},
			{"de", locale::country::germany},
			{"ia", locale::country::italy},
			{"tj", locale::country::tadjikistan},
			{"uz", locale::country::uzbekistan},
			{"kg", locale::country::kyrgyzstan},
			{"by", locale::country::belorussia},
			{"ua", locale::country::ukraine},
			{"kz", locale::country::kazakhstan},
			{"us", locale::country::united_states},
			{"gb", locale::country::great_britain},
			{"ru", locale::country::russia}
		},
		locale::country::undefined
	};

/*
	{
		{
			locale::language::undefined,
			{"Undefined", "Неопределен", "Spanish", "Deutch", "Greece", "Thai", "China", "Italy", "France", "Arabic"}
		},

		{
			locale::language::english,
			{"English", "Английский", "Spanish", "Deutch", "Greece", "Thai", "China", "Italy", "France", "Arabic"}
		},

		{
			locale::language::russian,
			{"Russian", "Русский", "Spanish", "Deutch", "Greece", "Thai", "China", "Italy", "France", "Arabic"}
		}
	}
*/

/*
	chaos::localization<T>::russian(T::Tag tag)
	{
		return chaos::localization<T>::fetch(tag, locale::language::russian);
	}

	chaos::localization<T>::fetch(T::Tag tag, locale::language language)
	{
		rturn T.at(tag)[language]
	}
*/

	locale::locale(std::nullptr_t)
	:
		_language(locale::language::undefined),
		_country(locale::country::undefined)
	{

	}

	locale::locale(const std::string& name)
	:
		_language(locale::language::undefined),
		_country(locale::country::undefined)
	{
		std::vector<std::string> array(string::explode<std::vector, std::string>(name, "_"));

		if (array.size() != 2) {
			return ;
		}

		_language = locale::language_map.at(string::lowercased_string(array[0]), true);
		_country = locale::country_map.at(string::lowercased_string(array[1]), true);
	}

	locale::locale(locale::language language, locale::country country)
	:
		_language(language),
		_country(country)
	{

	}

	locale::locale(const locale& origin)
	:
		_language(origin._language),
		_country(origin._country)
	{

	}

	locale::locale(locale&& origin)
	:
		_language(std::move(origin._language)),
		_country(std::move(origin._country))
	{

	}

	locale& locale::operator=(const locale& rhs)
	{
		_language = rhs.get_language();
		_country = rhs.get_country();
		return *this;
	}

	locale::operator std::string() const
	{
		return locale::language_map.at(_language, true) + "_" + string::uppercased_string(locale::country_map.at(_country, true));
	}

	bool locale::operator<(const locale& rhs) const
	{
		return (_language == rhs._language) ? _country < rhs._country : _language < rhs._language;
	}

	void locale::set_language(locale::language language)
	{
		_language = language;
	}

	void locale::set_country(locale::country country)
	{
		_country = country;
	}

	locale::language locale::get_language() const
	{
		return _language;
	}

	locale::country locale::get_country() const
	{
		return _country;
	}
}
