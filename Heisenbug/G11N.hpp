/**
 @file G11N.hpp
 @date 15.04.21
 @copyright Cadabra Systems Inc
 @author daniil@megrabyan.pro
*/

#ifndef Chaos_Heisenbug_G11N_hpp
#define Chaos_Heisenbug_G11N_hpp

#include "../Heisenbug.hpp"

#include "../G11N/G11N.hpp"
#include "../G11N/String.hpp"

namespace chaos {
	class g11n_test : public heisen_test
	{
	/** @name Statics */
	/** @{ */
	public:
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		g11n_test()
		:
			heisen_test("G11N")
		{

		};
		virtual ~g11n_test() = default;
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
		virtual void tear() override
		{
			HEISEN(UTF8StringCount);
			HEISEN(UnicodeString);
		}
	/** @} */
		
	/** @name Tests */
	/** @{ */
	private:
		/**
		 * @brief
		 */
		void testUTF8StringCount()
		{
			ARE_EQUAL(g11n::string::count(std::string{"Привет, Мир!"}, g11n::string::text_item::codepoint), 12)
			ARE_EQUAL(g11n::string::count(std::string{"Привет, Мир!"}, g11n::string::text_item::grapheme), 12)
			ARE_EQUAL(g11n::string::count(std::string{"Привет, Мир!"}, g11n::string::text_item::word), 2)
			ARE_EQUAL(g11n::string::count(std::string{"Привет, Мир!"}, g11n::string::text_item::white_space), 1)

			ARE_EQUAL(g11n::string::count(std::string{"🖖🏻"}, g11n::string::text_item::codepoint), 2)
			ARE_EQUAL(g11n::string::count(std::string{"🖖🏻"}, g11n::string::text_item::grapheme), 1)
			ARE_EQUAL(g11n::string::count(std::string{"🖖🏻"}, g11n::string::text_item::word), 0)
			ARE_EQUAL(g11n::string::count(std::string{"🖖🏻"}, g11n::string::text_item::white_space), 0)
		}

		/**
		 * @brief
		 */
		void testUnicodeString()
		{
			IS_TRUE(1 == 1);
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
