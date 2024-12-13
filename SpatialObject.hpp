/**
 @file SpatialObject.hpp
 @date 09.03.17
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#ifndef Chaos_SpatialObject_hpp
#define Chaos_SpatialObject_hpp

#include <map>
#include <string>

namespace chaos {
	class spatial_object
	{
	/** @name Static */
	/** @{ */
	public:
		enum class type : std::uint8_t
		{
			point = 0, // POINT(0 0)
			linestring = 1, // LINESTRING(0 0,1 1,1 2)
			polygon = 2, // POLYGON((0 0,4 0,4 4,0 4,0 0),(1 1, 2 1, 2 2, 1 2,1 1))
			multipoint = 3, // MULTIPOINT((0 0),(1 2))
			multilinestring = 4, // MULTILINESTRING((0 0,1 1,1 2),(2 3,3 2,5 4))
			multipolygon = 5, // MULTIPOLYGON(((0 0,4 0,4 4,0 4,0 0),(1 1,2 1,2 2,1 2,1 1)), ((-1 -1,-1 -2,-2 -2,-2 -1,-1 -1)))
			geometrycollection = 6 // GEOMETRYCOLLECTION(POINT(2 3),LINESTRING(2 3,3 4))
		};
		
		enum class system : std::uint16_t
		{
			wgs84 = 4326,
			sps = 2877,
			nad83 = 4269
		};
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		spatial_object(spatial_object::type type, spatial_object::system system = system::wgs84);
		virtual ~spatial_object() = default;
	/** @} */
		
	/** @name Factories */
	/** @{ */
	public:
	/** @} */
		
	/** @name Operators */
	/** @{ */
	public:
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		const spatial_object::type _type;
		const spatial_object::system _system;
	/** @} */
		
	/** @name Procedures  */
	/** @{ */
	public:
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
		const spatial_object::type& get_type() const;
		const spatial_object::system& get_system() const;
	/** @} */
		
	/** @name Formatters */
	/** @{ */
	public:
	/** @} */
		
	/** @name States */
	/** @{ */
	public:
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
