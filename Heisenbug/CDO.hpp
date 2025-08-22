/**
 @file CDO.hpp
 @date 15.04.21
 @copyright Cadabra Systems Inc
 @author daniil@megrabyan.pro
*/

#ifndef Chaos_Heisenbug_CDO_hpp
#define Chaos_Heisenbug_CDO_hpp

#include "../Heisenbug.hpp"

namespace chaos {
	class cdo_test : public heisen_test
	{
	/** @name Statics */
	/** @{ */
	public:
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		cdo_test()
		:
			heisen_test("CDOs")
		{

		};
		virtual ~cdo_test() = default;
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
	protected:
	/** @} */
		
	/** @name Procedures  */
	/** @{ */
	protected:
		virtual void structuralize() override
		{
			HEISEN(SelectData);
			HEISEN(Two);
		}
	/** @} */
		
	/** @name Tests */
	/** @{ */
	private:
		/**
		 * @brief
		 */
		void testSelectData()
		{
			/// select() - means everything
/*
			struct point {
				float x;
				float y;
			};
			QUINCE_MAP_CLASS(point, (x)(y))

			struct movie {
				serial id;                  // primary key with automatically assigned values
				std::string title;
			};
			QUINCE_MAP_CLASS(movie, (id)(title))

			struct cinema {
				serial id;                  // primary key with automatically assigned values
				point location;
			};
			QUINCE_MAP_CLASS(cinema, (id)(location))

			struct screen {
				serial id;                  // primary key with automatically assigned values
				serial cinema_id;           // foreign key to cinemas table
				serial current_movie_id;    // foreign key to movies table
			};
			QUINCE_MAP_CLASS(screen, (id)(cinema_id)(current_movie_id))

			const serial fried_green_quinces_id         = movies.insert({serial(), "Fried Green Quinces"});
			const serial a_clockwork_quince_id          = movies.insert({serial(), "A Clockwork Quince"});
			const serial quince_of_tides_id             = movies.insert({serial(), "Quince of Tides"});
			const serial bridge_on_the_river_quince_id  = movies.insert({serial(), "Bridge on the River Quince"});
			const serial les_inquincibles_id            = movies.insert({serial(), "Les Inquincibles"});
			const serial the_quince_and_the_dead_id     = movies.insert({serial(), "The Quince and the Dead"});
			const serial conquincetador_id              = movies.insert({serial(), "Conquincetador"});
			const serial queen_quince_id                = movies.insert({serial(), "Queen Quince"});
			const serial frankfurt_quincidences_id      = movies.insert({serial(), "Frankfurt Quincidences"});

			const point my_place{10.0f, 20.0f};

			const serial around_the_corner  = cinemas.insert({serial(), {11.0f, 19.0f}});
			const serial down_the_street    = cinemas.insert({serial(), {2.0f, 19.0f}});
			const serial out_of_town        = cinemas.insert({serial(), {2.0f, 1000.0f }});

			screens.insert({serial(), around_the_corner, les_inquincibles_id});
			screens.insert({serial(), around_the_corner, quince_of_tides_id});
			screens.insert({serial(), around_the_corner, quince_of_tides_id});
			screens.insert({serial(), around_the_corner, queen_quince_id});
			screens.insert({serial(), around_the_corner, a_clockwork_quince_id});
			screens.insert({serial(), around_the_corner, frankfurt_quincidences_id});
			screens.insert({serial(), around_the_corner, frankfurt_quincidences_id});
			screens.insert({serial(), down_the_street, les_inquincibles_id});
			screens.insert({serial(), down_the_street, les_inquincibles_id});
			screens.insert({serial(), down_the_street, conquincetador_id});
			screens.insert({serial(), down_the_street, quince_of_tides_id});
			screens.insert({serial(), down_the_street, bridge_on_the_river_quince_id});
			screens.insert({serial(), out_of_town, les_inquincibles_id});
			screens.insert({serial(), out_of_town, bridge_on_the_river_quince_id});
			screens.insert({serial(), out_of_town, fried_green_quinces_id});
			screens.insert({serial(), out_of_town, fried_green_quinces_id});
			screens.insert({serial(), out_of_town, the_quince_and_the_dead_id});

*/

/*
			serial_table<movie> movies(db, "movies", &movie::id);
			serial_table<cinema> cinemas(db, "cinemas", &cinema::id);
			serial_table<screen> screens(db, "screens", &screen::id);

			screens.specify_foreign(screens->cinema_id, cinemas);
			screens.specify_foreign(screens->current_movie_id, movies);

			movies.open();
			cinemas.open();
			screens.open();

			extern table<point> points;
			const query<point> some_points = points.where(points->y > 1.9f);
			extern const query<float> interesting_numbers;

			points.where(points->x > 4.0f && points->y < 3.0f)


			const float max_radius = 100.0f;
			const exprn_mapper<float> x_distance = cinemas->location.x - my_place.x;
			const exprn_mapper<float> y_distance = cinemas->location.y - my_place.y;
			const predicate within_radius = x_distance*x_distance + y_distance*y_distance <= max_radius*max_radius;



			// Most typical cases. Each of these queries produces one float per point in points:
			//
			const query<float> xs = points.select(points->x);
			const query<float> products = points.select(points->x * points->y);

			// The exprn uses an aggregate function, so the query produces one output only:
			//
			const query<double> total = points.select(sum(points->x));

			// Okay, because points's value mapper is identical to some_points's value mapper.
			// (Stylistically questionable though.)
			//
			const query<float> some_xs = some_points.select(points->x);

			// Also okay, because equivalent to the preceding example:
			//
			const query<float> some_xs_again = points.where(points->y > 1.9f).select(points->x);

			// Invalid: the exprn is not an abstract_mapper:
			//
			const query<float> threes = points.select(3.0f);  // wrong

			// Okay, although the exprn doesn't mention points's value mapper:
			//
			const query<float> threes = points.select(exprn_mapper<float>(3));

			// Invalid: exprn refers to a value mapper that is not visible to it:
			//
			const query<bool> nonsense =
				points
				.select(points->x == *interesting_numbers); // wrong
			const query<int64_t> more_nonsense =
				points
				.select(count(points->x == *interesting_numbers)); // wrong

			// Okay: scalar() makes *interesting_numbers (aka the value mapper for interesting_numbers)
			// visible to any expression inside its subquery.
			//
			const query<int64_t> point_counts =
				interesting_numbers
				.select(scalar(
					points
					.select(count(points->x == *interesting_numbers))
				));

			weather_reports
			.order(- weather_reports->time, + weather_reports->field2)
			.distinct_on(weather_reports->location)
			.select(weather_reports->location, weather_reports->time, weather_reports->report)

			const query<std::tuple<screen, movie>> possible_screenings = screens.join(movies);
			const query<std::tuple<cinema, screen, movie>> combinations = join(cinemas, screens, movies);

			l.union_(r)
			l.intersect(r)
			l.except(r)
			l.union_all(r)
			l.intersect_all(r) (PostgreSQL only)
			l.except_all(r) (PostgreSQL only)
			provided that l and r are queries, and they have the same value type as each other.

			The result is a query with the following characteristics:

			Its value type is the same as l's value type (which is the same as r's).
			Its value mapper is identical to l's value mapper (which may or may not be identical to r's).
			Its outputs consist of an appropriate combination of the outputs of l and r:
			l.union_(r) produces exactly one copy of any output that is produced at least once by l or at least once by r.
			l.intersect(r) produces exactly one copy of any output that is produced at least once by l and at least once by r.
			l.except(r) produces exactly one copy of any output that is produced at least once by l but not at all by r.
			l.union_all(r) produces n+m copies of any output that is produced n times by l and m times by r.
			l.intersect_all(r) produces min(n, m) copies of any output that is produced n times by l and m times by r.
			l.except_all(r) produces max(n - m, 0) copies of any output that is produced n times by l and m times by r.
			q.where(c1).union_(q.where(c2))
			q.where(c1 || c2)
			q.where(c1).intersect(q.where(c2))
			q.where(c1 && c2)
			q.where(c1).except(q.where(c2))
			q.where(c1 && !c2)


*/

		}

		/**
		 * @brief
		 */
		void testTwo()
		{
			IS_TRUE(1 == 2);
		}
	/** @} */
	
	/** @name Setters */
	/** @{ */
	public:
	/** @} */
		
	/** @name Getters */
	/** @{ */
	public:
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
