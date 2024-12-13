//
//  Locale.hpp
//
//  Created by Daniil A Megrabyan on 14.02.17.
//  Copyright © 2017 Cadabra Systems. All rights reserved.
//

#ifndef Chaos_Locale_hpp
#define Chaos_Locale_hpp

#include "BiMap.hpp"

#include <string>
#include <cstdint>

namespace chaos {
	class locale
	{
	/** @name Classes */
	/** @{ */
	public:
		enum class language : std::int16_t
		{
			undefined,
			english = 2057,
			russian = 1049,
			german = 1031,
			spanish = 1034
		};

		enum class country
		{
			undefined,
			armenia,
			georgia,
			israel,
			germany,
			italy,
			united_states,
			great_britain,
			belorussia,
			ukraine,
			kazakhstan,
			tadjikistan,
			uzbekistan,
			kyrgyzstan,
			russia
		};
	/** @} */

	/** @name Aliases */
	/** @{ */
	public:
	/** @} */

	/** @name Constants */
	/** @{ */
	public:
		static const bimap<std::string, locale::language> language_map;
		static const bimap<std::string, locale::country> country_map;
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		locale(std::nullptr_t);
		locale(const std::string& name);
		locale(locale::language language, locale::country country);
		locale(const locale& origin);
		locale(locale&& origin);
		~locale() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		locale::language _language;
		locale::country _country;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		locale& operator=(const locale& rhs);
		operator std::string() const;
		bool operator<(const locale& rhs) const;
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
		void set_language(locale::language language);
		void set_country(locale::country country);
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		locale::language get_language() const;
		locale::country get_country() const;
	/** @} */
	};
}
#endif
