/**
 @file PhoneNumber.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_PhoneNumber_hpp
#define Chaos_PhoneNumber_hpp

#include "Locale.hpp"

#include <string>
#include <cstdint>

namespace chaos {
	class phone_number
	{
	/** @name Statics */
	/** @{ */
	public:
		static bool has_plus(const std::string& unformatted_number)
		{
			if (unformatted_number.length() < 5) {
				return false;
			}

			return (unformatted_number.at(0) == '+') ? true : false;
		}

		static std::string just_digits(const std::string& unformatted_number)
		{
			std::string retval(unformatted_number);
			retval.erase(std::remove_if(retval.begin(), retval.end(), [](char c) { return !std::isdigit(c); }), retval.end());

			return retval;
		}
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		phone_number()
		:
			_has_plus(false),
			_country(chaos::locale::country::undefined),
			_region(0),
			_e164(0)
		{

		}

		phone_number(const std::string& unformatted_number)
		:
			_has_plus(phone_number::has_plus(unformatted_number)),
			_source(phone_number::just_digits(unformatted_number)),
			_country(chaos::locale::country::undefined),
			_region(0),
			_e164(0)
		{

		}

		phone_number(const std::int64_t& e164)
		:
			_has_plus(false),
			_source(std::to_string(e164)),
			_country(chaos::locale::country::undefined),
			_region(0),
			_e164(e164)
		{

		}

		phone_number(const phone_number& origin)
		:
			_has_plus(origin._has_plus),
			_source(origin._source),
			_country(origin._country),
			_region(origin._region),
			_e164(origin._e164)
		{

		}

		phone_number(phone_number&& origin)
		:
			_has_plus(std::move(origin._has_plus)),
			_source(std::move(origin._source)),
			_country(std::move(origin._country)),
			_region(std::move(origin._region)),
			_e164(std::move(origin._e164))
		{
			/// @todo region, country
		}

		~phone_number() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		const bool _has_plus;
		const std::string _source;

		chaos::locale::country _country;
		int _region;
		std::int64_t _e164;
	/** @} */
/*
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
*/

	/** @name Operators */
	/** @{ */
	public:
		phone_number& operator=(const phone_number& rhs)
		{
			_country = rhs._country;
			_region = rhs._region;
			_e164 = rhs._e164;

			return *this;
		}

		phone_number& operator=(phone_number&& rhs)
		{
			_country = std::move(rhs._country);
			_region = std::move(rhs._region);
			_e164 = std::move(rhs._e164);

			return *this;
		}

		bool operator<(const phone_number& rhs) const
		{
			return _e164 < rhs._e164;
		}
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		bool parse(chaos::locale::country country = chaos::locale::country::undefined, bool country_detect = false)
		{
			if (_source.length() < 5) {
				return false;
			}

			if (country_detect && chaos::locale::country::undefined == country) {
				/// @todo foreach chaos::locale::contry::bimap
				if (parse(chaos::locale::country::russia)) {
					return true;
				} else if (parse(chaos::locale::country::armenia)) {
					return true;
				} else if (parse(chaos::locale::country::ukraine)) {
					return true;
				} else if (parse(chaos::locale::country::belorussia)) {
					return true;
				} else if (parse(chaos::locale::country::kazakhstan)) {
					return true;
				}
			}

			bool retval(true);
			_country = country;
			switch (country) {
				case chaos::locale::country::armenia: retval = phone_number::am(); break;
				case chaos::locale::country::russia: retval = phone_number::ru(); break;
				case chaos::locale::country::ukraine: retval = phone_number::ua(); break;
				case chaos::locale::country::belorussia: retval = phone_number::by(); break;
				case chaos::locale::country::kazakhstan: retval = phone_number::kz(); break;

				default: _e164 = std::stoll(_source);
			}

			if (!retval) {
				_country = chaos::locale::country::undefined;
				_region = 0;
				_e164 = 0;
			}

			return retval;
		}
	/** @} */

	/** @name States */
	/** @{ */
	public:
		const std::int64_t& e164() const
		{
			return _e164;
		}

	public:
		bool valid() const
		{
			return _e164 > 0;
		}

	private:
		bool am()
		{
			return false;
		}

		bool ru()
		{
			std::string source;
			if (_source.length() == 10 && _source.at(0) == '9') {
				source = "7" + _source;
			} else if (_source.length() != 11) {
				return false;
			} else {
				source = _source;
			}

			/// @note 8915 1234567
			if (source.at(0) == '8' && source.at(1) == '9') {
				source[0] = '7';
			}

			if (source.at(0) != '7') {
				return false;
			}

			_region = std::stoi(source.substr(1, 3));
			if (_region < 900 || _region > 999) {
				return false;
			}

			_e164 = std::stoll(source);
			return true;
		}

		bool ua()
		{
			return false;
		}

		bool by()
		{
			return false;
		}

		bool kz()
		{
			std::string source;
			if (_source.length() == 10 && _source.at(0) == '7') {
				source = "7" + _source;
			} else if (_source.length() != 11) {
				return false;
			} else {
				source = _source;
			}

			/// @note 8705 1234567
			if (source.at(0) == '8' && source.at(1) == '7') {
				source[0] = '7';
			}

			if (source.at(0) != '7') {
				return false;
			}

			_region = std::stoi(source.substr(1, 3));
			if (_region < 700 || _region > 799) {
				return false;
			}

			_e164 = std::stoll(source);
			return true;
		}
	/** @} */
	};
}
#endif
