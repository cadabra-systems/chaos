/**
 @file Globus.hpp
 @date 15.04.21
 @copyright Cadabra Systems Inc
 @author daniil@megrabyan.pro
*/

#ifndef Chaos_Heisenbug_Globus_hpp
#define Chaos_Heisenbug_Globus_hpp

#include "../Heisenbug.hpp"

#include "../Globus/Globus.hpp"
#include "../Globus/String.hpp"

namespace chaos {
	class globus_test : public heisen_test
	{
	/** @name Statics */
	/** @{ */
	public:
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		globus_test()
		:
			heisen_test("Globus")
		{

		};
		virtual ~globus_test() = default;
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
			ARE_EQUAL(globus::string::count(std::string{"Привет, Мир!"}, globus::string::text_item::codepoint), 12)
			ARE_EQUAL(globus::string::count(std::string{"Привет, Мир!"}, globus::string::text_item::grapheme), 12)
			ARE_EQUAL(globus::string::count(std::string{"Привет, Мир!"}, globus::string::text_item::word), 2)
			ARE_EQUAL(globus::string::count(std::string{"Привет, Мир!"}, globus::string::text_item::white_space), 1)

			ARE_EQUAL(globus::string::count(std::string{"🖖🏻"}, globus::string::text_item::codepoint), 2)
			ARE_EQUAL(globus::string::count(std::string{"🖖🏻"}, globus::string::text_item::grapheme), 1)
			ARE_EQUAL(globus::string::count(std::string{"🖖🏻"}, globus::string::text_item::word), 0)
			ARE_EQUAL(globus::string::count(std::string{"🖖🏻"}, globus::string::text_item::white_space), 0)
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
