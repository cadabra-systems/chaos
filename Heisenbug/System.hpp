/**
 @file System.hpp
 @date 15.04.21
 @copyright Cadabra Systems Inc
 @author daniil@megrabyan.pro
*/

#ifndef Chaos_Heisenbug_System_hpp
#define Chaos_Heisenbug_System_hpp

#include "../Heisenbug.hpp"

#include "../Backtrace.hpp"

namespace chaos {
	class system_test : public heisen_test
	{
	/** @name Statics */
	/** @{ */
	public:
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		system_test()
		:
			heisen_test("System")
		{

		};
		virtual ~system_test() = default;
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
			HEISEN(Backtrace);
		}
	/** @} */
		
	/** @name Tests */
	/** @{ */
	private:
		/**
		 * @brief
		 */
		void testBacktrace()
		{
			const std::list<std::pair<std::string, std::uintptr_t>> callstack(backtrace::stack());
			for (const std::pair<std::string, std::uintptr_t>& frame : callstack) {
				std::cout << frame.first << "+" << frame.second << std::endl;
			}
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
