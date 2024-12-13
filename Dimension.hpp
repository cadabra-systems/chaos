/**
 @file Dimension.hpp
 @date 09.11.15
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#ifndef Chaos_Dimension_hpp
#define Chaos_Dimension_hpp

#include <cstdint>

namespace chaos {
	class dimension
	{
	/** @name Classes */
	/** @{ */
	public:
		enum class unit_type : std::uint8_t
		{
			automatic = 0,
			ratio = 1,
			length = 2
		};
		
		enum class equality_type : std::uint8_t
		{
			less, // or equal
			equal,
			greater // or equal
		};
	/** @} */
		
	/** @name Aliases */
	/** @{ */
	public:
		using value_type = std::int32_t;
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		dimension(dimension::unit_type unit = dimension::unit_type::automatic, dimension::value_type value = 0)
		:
			_value(value),
			_unit(unit),
			_equality(equality_type::equal)
		{

		}
		
		virtual ~dimension() = default;
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		dimension::value_type _value;
		dimension::unit_type _unit;
		dimension::equality_type _equality;
	/** @} */
		
	/** @name Setters */
	/** @{ */
	public:
		void set(const dimension::unit_type& unit)
		{
			_unit = unit;
		}
		
		void set(const dimension::value_type& value)
		{
			_value = value;
		}
		
		void set(const dimension::equality_type& equality)
		{
			_equality = equality;
		}
	/** @} */
		
	/** @name Getters */
	/** @{ */
	public:
		const dimension::unit_type& unit() const
		{
			return _unit;
		}
		
		const dimension::value_type& value() const
		{
			return _value;
		}
		
		const dimension::equality_type& equality() const
		{
			return _equality;
		}
	/** @} */
	};
}
#endif
