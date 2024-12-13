#ifndef Chaos_Heisenbug_Any_hpp
#define Chaos_Heisenbug_Any_hpp

#include "../Heisenbug.hpp"

#include "../Any.hpp"

#include <list>
#include <chrono>

namespace chaos {
	class any_test : public heisen_test
	{
	/** @name Constructors */
	/** @{ */
	public:
		any_test()
		:
			heisen_test("Any")
		{

		};

		virtual ~any_test() = default;
	/** @} */

	/** @name Procedures  */
	/** @{ */
	protected:
		virtual void tear() override
		{
			HEISEN(Intersection)
		}
	/** @} */

	/** @name Tests */
	/** @{ */
	private:
		/**
		 * @brief
		 */
		void testIntersection()
		{
			any lhs({std::string("com.oblozhko.mentalgami.a"), std::string("com.oblozhko.mentalgami.b")});
			any rhs1, rhs2(std::string("com.oblozhko.mentalgami.b"));

			any intersection1 = lhs & rhs1;
			IS_FALSE(rhs1 == intersection1 || lhs == intersection1);

			any intersection2 = lhs & rhs2;
			IS_TRUE(rhs2 == intersection2);
		}
	/** @} */
	};
}

#endif
