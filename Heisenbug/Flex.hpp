/**
 @file Flex.hpp
 @date 15.04.21
 @copyright Cadabra Systems
 @author daniil@megrabyan.pro
*/

#ifndef Chaos_Heisenbug_Flex_hpp
#define Chaos_Heisenbug_Flex_hpp

#include "../Heisenbug.hpp"

#include "../Flex.hpp"
#include "../URI.hpp"

#include <string>
#include <chrono>
#include <variant>

namespace chaos {
	class flex_test : public heisen_test
	{
	/** @name Aliases */
	/** @{ */
	public:
		using Variant = std::variant
		<
			char,
			std::int64_t,
			double,
			bool,
			std::string,
			std::chrono::system_clock::time_point,
			std::pair<std::chrono::system_clock::time_point, std::chrono::system_clock::time_point>,
			chaos::flex,
			chaos::uri
		>;
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		flex_test()
		:
			heisen_test("Flex")
		{

		};
		virtual ~flex_test() = default;
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
			HEISEN(ComplexVariant);
		}
	/** @} */
		
	/** @name Tests */
	/** @{ */
	private:
		/**
		 * @brief
		 */
		void testComplexVariant()
		{
			Variant v('X');
			chaos::flex retval{
								{"key", "k"},
								{"value", v}
			};
			std::string dump(retval.dump());
			std::cout << dump;
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
