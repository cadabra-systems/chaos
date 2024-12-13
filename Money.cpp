/**
 @file Money.cpp
 @date 09.03.17
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#include "Money.hpp"

#include "String.hpp"

namespace chaos {
	const bimap<std::string, money::currency> money::currency_map = {
		{
			{"XXX", money::currency::undefined},
			{"AST", money::currency::appstore_tier},
			{"PMT", money::currency::playmarket_tier},
			{"AGT", money::currency::appgallery_tier},
			{"RUB", money::currency::russian_ruble},
			{"USD", money::currency::us_dollar},
			{"EUR", money::currency::euro},
			{"AMD", money::currency::armenian_dram},
			{"CCC", money::currency::cadabra_crypto_coin},
			{"CLP", money::currency::cadabra_level_price},
			{"CVE", money::currency::cadabra_virtual_expense}
		},
		money::currency::undefined
	};

	std::string money::currency_code(money::currency currency, bool to_upper)
	{
		return to_upper ? string::uppercased_string(money::currency_map.at(currency, true)) : money::currency_map.at(currency, true);
	}

	bool money::currency_tier(money::currency currency)
	{
		static const std::set<money::currency> tier_set
		{
			money::currency::appstore_tier,
			money::currency::appgallery_tier,
			money::currency::playmarket_tier,
			money::currency::cadabra_crypto_coin,
			money::currency::cadabra_level_price
		};
		return tier_set.find(currency) != tier_set.cend();
	}

	money::money()
	:
		_currency(money::currency::undefined),
		_centum(0),
		_negative(false)
	{

	}

	money::money(money::currency currency, unsigned long long centum, bool negative)
	:
		_currency(currency),
		_centum(centum),
		_negative(negative)
	{

	}

	money::money(const money& origon)
	:
		_currency(origon._currency),
		_centum(origon._centum),
		_negative(origon._negative)
	{

	}

	money::money(money&& origon)
	:
		_currency(std::move(origon._currency)),
		_centum(std::move(origon._centum)),
		_negative(std::move(origon._negative))
	{

	}

	money& money::operator=(const money& origon)
	{
		_currency = origon._currency;
		_centum = origon._centum;
		_negative = std::move(origon._negative);

		return *this;
	}

	money& money::operator=(money&& origon)
	{
		_currency = std::move(origon._currency);
		_centum = std::move(origon._centum);
		_negative = std::move(origon._negative);

		return *this;
	}

	money::operator double() const
	{
		return value();
	}

	money::operator long long() const
	{
		return _negative ? -(_centum) : _centum;
	}

	money money::operator*(double rhs) const
	{
		const std::uint64_t value(std::round(_centum * std::abs(rhs)));
		return money(_currency, value, rhs < 0 || _negative);
	}

	money& money::operator+=(const money& rhs)
	{
		if (rhs._currency != _currency) {
			return *this;
		} else if (rhs._negative && _negative) {
			_centum += rhs._centum;
		} else if (!rhs._negative && !_negative) {
			_centum += rhs._centum;
		} else if (rhs._negative && !_negative) {
			if (_centum > rhs._centum) {
				_centum -= rhs._centum;
			} else if (_centum < rhs._centum) {
				_centum = rhs._centum - _centum;
				_negative = true;
			} else {
				_centum = 0;
			}
		} else {
			if (_centum > rhs._centum) {
				_centum -= rhs._centum;
			} else if (_centum < rhs._centum) {
				_centum = rhs._centum - _centum;
				_negative = true;
			} else {
				_centum = 0;
			}
		}
		return *this;
	}

	money money::operator+(const money& rhs) const
	{
		if (rhs._currency != _currency) {
			return *this;
		} else if (rhs._negative && _negative) {
			return {_currency, rhs._centum + _centum, true};
		} else if (!rhs._negative && !_negative) {
			return {_currency, rhs._centum + _centum, false};
		} else if (rhs._negative && !_negative) {
			if (_centum > rhs._centum) {
				return {_currency, _centum - rhs._centum, false};
			} else if (_centum < rhs._centum) {
				return {_currency, rhs._centum - _centum, true};
			}
		} else {
			if (_centum > rhs._centum) {
				return {_currency, _centum + rhs._centum, false};
			} else if (_centum < rhs._centum) {
				return {_currency, rhs._centum - _centum, true};
			}
		}
		return {_currency, 0, false};
	}

	std::string money::print() const
	{
		return (currency_tier(_currency) ? std::to_string((_negative ? -_centum : _centum)) : string::formatted_string("%.2f", value())) + " " + currency_code(true);
	}

	double money::value() const
	{
		return currency_tier(_currency) ? (_negative ? -_centum : _centum) : (_centum / (_negative ? -100.0 : 100.0));
	}

	money::centum_type money::centum() const
	{
		return _centum;
	}

	money::currency money::currency_type() const
	{
		return _currency;
	}

	std::string money::currency_code(bool upper) const
	{
		return !upper ? money::currency_map.at(_currency, true) : string::uppercased_string(money::currency_map.at(_currency, true));
	}

	bool money::negative() const
	{
		return _negative;
	}

	bool money::zero() const
	{
		return 0 == _centum;
	}
}
