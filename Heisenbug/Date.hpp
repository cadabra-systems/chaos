#ifndef Chaos_Heisenbug_Date_hpp
#define Chaos_Heisenbug_Date_hpp

#include "../Date.hpp"
#include "../Heisenbug.hpp"

namespace chaos {
	class date_test : public heisen_test
	{
	/** @name Constructors */
	/** @{ */
	public:
		date_test()
		:
			heisen_test("Date")
		{

		};

		virtual ~date_test() = default;
	/** @} */

	/** @name Procedures  */
	/** @{ */
	protected:
		virtual void tear() override
		{
			HEISEN(EqualOperator)
			HEISEN(NotEqualOperator)
			HEISEN(GreaterOperator)
			HEISEN(GreaterEqualOperator)
			HEISEN(LessOperator)
			HEISEN(LessEqualOperator)
		}
	/** @} */

	/** @name Tests */
	/** @{ */
	private:
		/**
		 * @brief
		 */
		void testEqualOperator()
		{
			// Сравнение двух дат, содержащих хотя бы одно поле any или undefined, всегда возвращает false:
			IS_FALSE(chaos::date("**-01-2020") == chaos::date("##-01-2020"));
			IS_FALSE(chaos::date("**-01-2020") == chaos::date("01-##-2020"));
			IS_FALSE(chaos::date("**-01-2020") == chaos::date("01-01-####"));

			IS_FALSE(chaos::date("01-**-2020") == chaos::date("##-01-2020"));
			IS_FALSE(chaos::date("01-**-2020") == chaos::date("01-##-2020"));
			IS_FALSE(chaos::date("01-**-2020") == chaos::date("01-01-####"));

			IS_FALSE(chaos::date("01-01-****") == chaos::date("##-01-2020"));
			IS_FALSE(chaos::date("01-01-****") == chaos::date("01-##-2020"));
			IS_FALSE(chaos::date("01-01-****") == chaos::date("01-01-####"));

			// Для valid-полей все стандартно:
			ARE_EQUAL(chaos::date("01-01-2020"), chaos::date("01-01-2020"));

			IS_FALSE(chaos::date("01-01-2020") == chaos::date("02-01-2020"));
			IS_FALSE(chaos::date("01-01-2020") == chaos::date("01-02-2020"));
			IS_FALSE(chaos::date("01-01-2020") == chaos::date("01-01-2022"));

			// Any-поле всегда равно valid-полю:
			IS_TRUE(chaos::date("01-01-2020") == chaos::date("**-01-2020"));
			IS_TRUE(chaos::date("01-01-2020") == chaos::date("01-**-2020"));
			IS_TRUE(chaos::date("01-01-2020") == chaos::date("01-01-****"));

			// Undefined-поле всегда не равно valid-полю:
			IS_FALSE(chaos::date("01-01-2020") == chaos::date("##-01-2020"));
			IS_FALSE(chaos::date("01-01-2020") == chaos::date("01-##-2020"));
			IS_FALSE(chaos::date("01-01-2020") == chaos::date("01-01-####"));

			// Any-поле всегда равно any-полю:
			IS_TRUE(chaos::date("**-01-2020") == chaos::date("**-01-2020"));
			IS_TRUE(chaos::date("01-**-2020") == chaos::date("01-**-2020"));
			IS_TRUE(chaos::date("01-01-****") == chaos::date("01-01-****"));

			// Undefined-поле всегда равно undefined-полю:
			IS_TRUE(chaos::date("##-01-2020") == chaos::date("##-01-2020"));
			IS_TRUE(chaos::date("01-##-2020") == chaos::date("01-##-2020"));
			IS_TRUE(chaos::date("01-01-####") == chaos::date("01-01-####"));
		}

		/**
		 * @brief
		 */
		void testNotEqualOperator()
		{
			// Сравнение двух дат, содержащих хотя бы одно поле any или undefined, всегда возвращает false:
			IS_FALSE(chaos::date("**-01-2020") != chaos::date("##-01-2020"));
			IS_FALSE(chaos::date("**-01-2020") != chaos::date("01-##-2020"));
			IS_FALSE(chaos::date("**-01-2020") != chaos::date("01-01-####"));

			IS_FALSE(chaos::date("01-**-2020") != chaos::date("##-01-2020"));
			IS_FALSE(chaos::date("01-**-2020") != chaos::date("01-##-2020"));
			IS_FALSE(chaos::date("01-**-2020") != chaos::date("01-01-####"));

			IS_FALSE(chaos::date("01-01-****") != chaos::date("##-01-2020"));
			IS_FALSE(chaos::date("01-01-****") != chaos::date("01-##-2020"));
			IS_FALSE(chaos::date("01-01-****") != chaos::date("01-01-####"));

			// Для valid-полей все стандартно:
			IS_FALSE(chaos::date("01-01-2020") != chaos::date("01-01-2020"));

			IS_TRUE(chaos::date("01-01-2020") != chaos::date("02-01-2020"));
			IS_TRUE(chaos::date("01-01-2020") != chaos::date("01-02-2020"));
			IS_TRUE(chaos::date("01-01-2020") != chaos::date("01-01-2022"));

			// Any-поле всегда равно valid-полю:
			IS_FALSE(chaos::date("01-01-2020") != chaos::date("**-01-2020"));
			IS_FALSE(chaos::date("01-01-2020") != chaos::date("01-**-2020"));
			IS_FALSE(chaos::date("01-01-2020") != chaos::date("01-01-****"));

			// Undefined-поле всегда не равно valid-полю:
			IS_TRUE(chaos::date("01-01-2020") != chaos::date("##-01-2020"));
			IS_TRUE(chaos::date("01-01-2020") != chaos::date("01-##-2020"));
			IS_TRUE(chaos::date("01-01-2020") != chaos::date("01-01-####"));

			// Any-поле всегда равно any-полю:
			IS_FALSE(chaos::date("**-01-2020") != chaos::date("**-01-2020"));
			IS_FALSE(chaos::date("01-**-2020") != chaos::date("01-**-2020"));
			IS_FALSE(chaos::date("01-01-****") != chaos::date("01-01-****"));

			// Undefined-поле всегда равно undefined-полю:
			IS_FALSE(chaos::date("##-01-2020") != chaos::date("##-01-2020"));
			IS_FALSE(chaos::date("01-##-2020") != chaos::date("01-##-2020"));
			IS_FALSE(chaos::date("01-01-####") != chaos::date("01-01-####"));
		}

		void testGreaterOperator()
		{
			// Сравнение двух дат, содержащих хотя бы одно поле any или undefined, всегда возвращает false:
			IS_FALSE(chaos::date("**-01-2020") > chaos::date("##-01-2020"));
			IS_FALSE(chaos::date("**-01-2020") > chaos::date("01-##-2020"));
			IS_FALSE(chaos::date("**-01-2020") > chaos::date("01-01-####"));

			IS_FALSE(chaos::date("01-**-2020") > chaos::date("##-01-2020"));
			IS_FALSE(chaos::date("01-**-2020") > chaos::date("01-##-2020"));
			IS_FALSE(chaos::date("01-**-2020") > chaos::date("01-01-####"));

			IS_FALSE(chaos::date("01-01-****") > chaos::date("##-01-2020"));
			IS_FALSE(chaos::date("01-01-****") > chaos::date("01-##-2020"));
			IS_FALSE(chaos::date("01-01-****") > chaos::date("01-01-####"));

			// Для valid-полей все стандартно:
			IS_FALSE(chaos::date("01-01-2020") > chaos::date("01-01-2020"));

			IS_TRUE(chaos::date("02-01-2020") > chaos::date("01-01-2020"));
			IS_TRUE(chaos::date("01-02-2020") > chaos::date("01-01-2020"));
			IS_TRUE(chaos::date("01-01-2022") > chaos::date("01-01-2020"));

			IS_FALSE(chaos::date("01-01-2020") > chaos::date("02-01-2020"));
			IS_FALSE(chaos::date("01-01-2020") > chaos::date("01-02-2020"));
			IS_FALSE(chaos::date("01-01-2020") > chaos::date("01-01-2022"));

			// Any-поле всегда равно valid-полю:
			IS_FALSE(chaos::date("01-01-2020") > chaos::date("**-01-2020"));
			IS_FALSE(chaos::date("01-01-2020") > chaos::date("01-**-2020"));
			IS_FALSE(chaos::date("01-01-2020") > chaos::date("01-01-****"));

			// Undefined-поле всегда не равно valid-полю:
			IS_FALSE(chaos::date("01-01-2020") > chaos::date("##-01-2020"));
			IS_FALSE(chaos::date("01-01-2020") > chaos::date("01-##-2020"));
			IS_FALSE(chaos::date("01-01-2020") > chaos::date("01-01-####"));

			// Any-поле всегда равно any-полю:
			IS_FALSE(chaos::date("**-01-2020") > chaos::date("**-01-2020"));
			IS_FALSE(chaos::date("01-**-2020") > chaos::date("01-**-2020"));
			IS_FALSE(chaos::date("01-01-****") > chaos::date("01-01-****"));

			// Undefined-поле всегда равно undefined-полю:
			IS_FALSE(chaos::date("##-01-2020") > chaos::date("##-01-2020"));
			IS_FALSE(chaos::date("01-##-2020") > chaos::date("01-##-2020"));
			IS_FALSE(chaos::date("01-01-####") > chaos::date("01-01-####"));
		}

		void testGreaterEqualOperator()
		{
			// Сравнение двух дат, содержащих хотя бы одно поле any или undefined, всегда возвращает false:
			IS_FALSE(chaos::date("**-01-2020") >= chaos::date("##-01-2020"));
			IS_FALSE(chaos::date("**-01-2020") >= chaos::date("01-##-2020"));
			IS_FALSE(chaos::date("**-01-2020") >= chaos::date("01-01-####"));

			IS_FALSE(chaos::date("01-**-2020") >= chaos::date("##-01-2020"));
			IS_FALSE(chaos::date("01-**-2020") >= chaos::date("01-##-2020"));
			IS_FALSE(chaos::date("01-**-2020") >= chaos::date("01-01-####"));

			IS_FALSE(chaos::date("01-01-****") >= chaos::date("##-01-2020"));
			IS_FALSE(chaos::date("01-01-****") >= chaos::date("01-##-2020"));
			IS_FALSE(chaos::date("01-01-****") >= chaos::date("01-01-####"));

			// Для valid-полей все стандартно:
			IS_TRUE(chaos::date("01-01-2020") >= chaos::date("01-01-2020"));

			IS_TRUE(chaos::date("02-01-2020") >= chaos::date("01-01-2020"));
			IS_TRUE(chaos::date("01-02-2020") >= chaos::date("01-01-2020"));
			IS_TRUE(chaos::date("01-01-2022") >= chaos::date("01-01-2020"));

			IS_FALSE(chaos::date("01-01-2020") >= chaos::date("02-01-2020"));
			IS_FALSE(chaos::date("01-01-2020") >= chaos::date("01-02-2020"));
			IS_FALSE(chaos::date("01-01-2020") >= chaos::date("01-01-2022"));

			// Any-поле всегда равно valid-полю:
			IS_TRUE(chaos::date("01-01-2020") >= chaos::date("**-01-2020"));
			IS_TRUE(chaos::date("01-01-2020") >= chaos::date("01-**-2020"));
			IS_TRUE(chaos::date("01-01-2020") >= chaos::date("01-01-****"));

			// Undefined-поле всегда не равно valid-полю:
			IS_FALSE(chaos::date("01-01-2020") >= chaos::date("##-01-2020"));
			IS_FALSE(chaos::date("01-01-2020") >= chaos::date("01-##-2020"));
			IS_FALSE(chaos::date("01-01-2020") >= chaos::date("01-01-####"));

			// Any-поле всегда равно any-полю:
			IS_TRUE(chaos::date("**-01-2020") >= chaos::date("**-01-2020"));
			IS_TRUE(chaos::date("01-**-2020") >= chaos::date("01-**-2020"));
			IS_TRUE(chaos::date("01-01-****") >= chaos::date("01-01-****"));

			// Undefined-поле всегда равно undefined-полю:
			IS_TRUE(chaos::date("##-01-2020") >= chaos::date("##-01-2020"));
			IS_TRUE(chaos::date("01-##-2020") >= chaos::date("01-##-2020"));
			IS_TRUE(chaos::date("01-01-####") >= chaos::date("01-01-####"));
		}

		void testLessOperator()
		{
			// Сравнение двух дат, содержащих хотя бы одно поле any или undefined, всегда возвращает false:
			IS_FALSE(chaos::date("**-01-2020") < chaos::date("##-01-2020"));
			IS_FALSE(chaos::date("**-01-2020") < chaos::date("01-##-2020"));
			IS_FALSE(chaos::date("**-01-2020") < chaos::date("01-01-####"));

			IS_FALSE(chaos::date("01-**-2020") < chaos::date("##-01-2020"));
			IS_FALSE(chaos::date("01-**-2020") < chaos::date("01-##-2020"));
			IS_FALSE(chaos::date("01-**-2020") < chaos::date("01-01-####"));

			IS_FALSE(chaos::date("01-01-****") < chaos::date("##-01-2020"));
			IS_FALSE(chaos::date("01-01-****") < chaos::date("01-##-2020"));
			IS_FALSE(chaos::date("01-01-****") < chaos::date("01-01-####"));

			// Для valid-полей все стандартно:
			IS_FALSE(chaos::date("01-01-2020") < chaos::date("01-01-2020"));

			IS_FALSE(chaos::date("02-01-2020") < chaos::date("01-01-2020"));
			IS_FALSE(chaos::date("01-02-2020") < chaos::date("01-01-2020"));
			IS_FALSE(chaos::date("01-01-2022") < chaos::date("01-01-2020"));

			IS_TRUE(chaos::date("01-01-2020") < chaos::date("02-01-2020"));
			IS_TRUE(chaos::date("01-01-2020") < chaos::date("01-02-2020"));
			IS_TRUE(chaos::date("01-01-2020") < chaos::date("01-01-2022"));

			// Any-поле всегда равно valid-полю:
			IS_FALSE(chaos::date("01-01-2020") < chaos::date("**-01-2020"));
			IS_FALSE(chaos::date("01-01-2020") < chaos::date("01-**-2020"));
			IS_FALSE(chaos::date("01-01-2020") < chaos::date("01-01-****"));

			// Undefined-поле всегда не равно valid-полю:
			IS_FALSE(chaos::date("01-01-2020") < chaos::date("##-01-2020"));
			IS_FALSE(chaos::date("01-01-2020") < chaos::date("01-##-2020"));
			IS_FALSE(chaos::date("01-01-2020") < chaos::date("01-01-####"));

			// Any-поле всегда равно any-полю:
			IS_FALSE(chaos::date("**-01-2020") < chaos::date("**-01-2020"));
			IS_FALSE(chaos::date("01-**-2020") < chaos::date("01-**-2020"));
			IS_FALSE(chaos::date("01-01-****") < chaos::date("01-01-****"));

			// Undefined-поле всегда равно undefined-полю:
			IS_FALSE(chaos::date("##-01-2020") < chaos::date("##-01-2020"));
			IS_FALSE(chaos::date("01-##-2020") < chaos::date("01-##-2020"));
			IS_FALSE(chaos::date("01-01-####") < chaos::date("01-01-####"));
		}

		void testLessEqualOperator()
		{
			// Сравнение двух дат, содержащих хотя бы одно поле any или undefined, всегда возвращает false:
			IS_FALSE(chaos::date("**-01-2020") <= chaos::date("##-01-2020"));
			IS_FALSE(chaos::date("**-01-2020") <= chaos::date("01-##-2020"));
			IS_FALSE(chaos::date("**-01-2020") <= chaos::date("01-01-####"));

			IS_FALSE(chaos::date("01-**-2020") <= chaos::date("##-01-2020"));
			IS_FALSE(chaos::date("01-**-2020") <= chaos::date("01-##-2020"));
			IS_FALSE(chaos::date("01-**-2020") <= chaos::date("01-01-####"));

			IS_FALSE(chaos::date("01-01-****") <= chaos::date("##-01-2020"));
			IS_FALSE(chaos::date("01-01-****") <= chaos::date("01-##-2020"));
			IS_FALSE(chaos::date("01-01-****") <= chaos::date("01-01-####"));

			// Для valid-полей все стандартно:
			IS_TRUE(chaos::date("01-01-2020") <= chaos::date("01-01-2020"));

			IS_FALSE(chaos::date("02-01-2020") <= chaos::date("01-01-2020"));
			IS_FALSE(chaos::date("01-02-2020") <= chaos::date("01-01-2020"));
			IS_FALSE(chaos::date("01-01-2022") <= chaos::date("01-01-2020"));

			IS_TRUE(chaos::date("01-01-2020") <= chaos::date("02-01-2020"));
			IS_TRUE(chaos::date("01-01-2020") <= chaos::date("01-02-2020"));
			IS_TRUE(chaos::date("01-01-2020") <= chaos::date("01-01-2022"));

			// Any-поле всегда равно valid-полю:
			IS_TRUE(chaos::date("01-01-2020") <= chaos::date("**-01-2020"));
			IS_TRUE(chaos::date("01-01-2020") <= chaos::date("01-**-2020"));
			IS_TRUE(chaos::date("01-01-2020") <= chaos::date("01-01-****"));

			// Undefined-поле всегда не равно valid-полю:
			IS_FALSE(chaos::date("01-01-2020") <= chaos::date("##-01-2020"));
			IS_FALSE(chaos::date("01-01-2020") <= chaos::date("01-##-2020"));
			IS_FALSE(chaos::date("01-01-2020") <= chaos::date("01-01-####"));

			// Any-поле всегда равно any-полю:
			IS_TRUE(chaos::date("**-01-2020") <= chaos::date("**-01-2020"));
			IS_TRUE(chaos::date("01-**-2020") <= chaos::date("01-**-2020"));
			IS_TRUE(chaos::date("01-01-****") <= chaos::date("01-01-****"));

			// Undefined-поле всегда равно undefined-полю:
			IS_TRUE(chaos::date("##-01-2020") <= chaos::date("##-01-2020"));
			IS_TRUE(chaos::date("01-##-2020") <= chaos::date("01-##-2020"));
			IS_TRUE(chaos::date("01-01-####") <= chaos::date("01-01-####"));
		}
	/** @} */
	};
}

#endif
