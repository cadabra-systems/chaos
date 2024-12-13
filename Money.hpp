/**
 @file Money.hpp
 @date 09.03.17
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#ifndef Chaos_Money_hpp
#define Chaos_Money_hpp

#include <string>
#include <cstdint>
#include <cmath>

#include "BiMap.hpp"

namespace chaos {
	class money
	{
	/** @name Static */
	/** @{ */
	public:
		enum class currency
		{
			undefined,
			appstore_tier, /// @note (Tier1, Tier2... )
			playmarket_tier, /// @note (Tier1, Tier2... )
			appgallery_tier, /// @note (Tier1, Tier2... )
			armenian_dram, /// @note (1 Dram = 100 Luma)
			russian_ruble, /// @note (1 Ruble = 100 Kopeks)
			us_dollar, /// @note (1 Dollar = 10 Dimes = 100 Cents)
			euro, /// @note (1 Euro = 100 Cent)
			cadabra_crypto_coin, /// @note (cadabra crypto coin)
			cadabra_virtual_expense,
			cadabra_level_price
		};
	/** @} */

	/** @name Aliases */
	/** @{ */
	public:
		using centum_type = std::uint64_t;
	/** @} */

	/** @name Static */
	/** @{ */
	public:
		static const bimap<std::string, money::currency> currency_map;
		static std::string currency_code(money::currency currency, bool to_upper = false);
		static bool currency_tier(money::currency currency);
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		money();
		money(money::currency currency, unsigned long long centum = 0, bool negative = false);
		money(const money& origon);
		money(money&& origon);
		~money() = default;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		money& operator=(const money& origon);
		money& operator=(money&& origon);
		operator double() const;
		operator long long() const;
		money operator*(double rhs) const;
		money& operator+=(const money& rhs);
		money operator+(const money& rhs) const;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		money::currency _currency;
		centum_type _centum;
		bool _negative;
	/** @} */

	/** @name Procedures  */
	/** @{ */
	public:
		std::string print() const;
	/** @} */

	/** @name Converters */
	/** @{ */
	public:
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		double value() const;
		centum_type centum() const;
		money::currency currency_type() const;
		std::string currency_code(bool upper = false) const;
	/** @} */

	/** @name Formatters */
	/** @{ */
	public:
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool negative() const;
		bool zero() const;
	/** @} */

	/** @name Hooks */
	/** @{ */
	public:
	/** @} */

	/** @name Templates */
	/** @{ */
	public:
	/** @} */
	};
}

#endif
