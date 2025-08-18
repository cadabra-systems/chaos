/**
 @file BreakingBad.hpp
 @date 15.04.21
 @copyright Cadabra Systems Inc
 @author daniil@megrabyan.pro
*/

#ifndef Chaos_Heisenbug_BreakingBad_hpp
#define Chaos_Heisenbug_BreakingBad_hpp

#include "../Heisenbug.hpp"

namespace chaos {
	class breaking_bad : protected heisen_test
	{
	/** @name Statics */
	/** @{ */
	public:
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		breaking_bad();
		virtual ~breaking_bad() = default;
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
		std::string _test_mask;
		std::string _case_mask;
	/** @} */
		
	/** @name Procedures  */
	/** @{ */
	public:
		void masquerade(const std::string& test_mask, const std::string& case_mask);

	protected:
		virtual void tear() override;
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		virtual const std::string& get_mask() const override;
	/** @} */

	/** @name Tests */
	/** @{ */
	private:
		/**
		* @brief Basic stuff
		*/
		void testBasic();

		/**
		 * @brief System stuff
		 */
		void testSystem();

		/**
		 * @brief Atomic Hash Table
		 */
		void testAtomicHashTable();

		/**
		 * @brief Atomic Forward List
		 */
		void testAtomicForwardList();

		/**
		 * @brief System stuff
		 */
		void testHTTPClient();

		/**
		 * @brief Cadabra Data Objects
		 */
		void testCDO();

		/**
		 * @brief JSON(b)
		 */
		void testFlex();

		/**
		 * @brief Cadabra Web Services (Cadabra Cloud: CNS, Fiscus, CTS, ...)
		 */
		void testCWS();

		/**
		 * @brief String Utility
		 */
		void testString();

		/**
		 * @brief Date
		 */
		void testDate();

		/**
		 * @brief Any container
		 */
		void testAny();

		/**
		 * @brief Globus
		 */
		void testGlobus();

		/**
		 * @brief ODBC
		 */
		void testODBC();
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
