/**
 @file Basic.hpp
 @date 15.04.21
 @copyright Cadabra Basics Inc
 @author daniil@megrabyan.pro
*/

#ifndef Chaos_Heisenbug_Basic_hpp
#define Chaos_Heisenbug_Basic_hpp

#include "../Heisenbug.hpp"

#include "../Murmur3.hpp"

namespace chaos {
	class basic_test : public heisen_test
	{
	/** @name Statics */
	/** @{ */
	public:
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		basic_test()
		:
			heisen_test("Basic")
		{

		};
		virtual ~basic_test() = default;
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
			HEISEN(MurMur3);
		}
	/** @} */
		
	/** @name Tests */
	/** @{ */
	private:
		/**
		 * @brief
		 */
		void testMurMur3()
		{
			ARE_EQUAL
			(
				static_cast<std::int64_t>(chaos::murmur3::hash32("systems.cadabra.cap.mentalgami.award")),
				752538711
			);
			ARE_EQUAL
			(
				static_cast<std::int64_t>(chaos::murmur3::hash32("systems.cadabra.cap.SmartCare.award")),
				2320668760
			);
			ARE_EQUAL
			(
				static_cast<std::int64_t>(chaos::murmur3::hash32("com.mentalgami.award")),
				3943767540
			);
			ARE_EQUAL
			(
				static_cast<std::int64_t>(chaos::murmur3::hash32("dataset:prefilter:channel_hub")),
				1822295993
			);
			ARE_EQUAL
			(
				static_cast<std::int64_t>(chaos::murmur3::hash32("dataset:prefilter:care_hub")),
				722147417
			);
			ARE_EQUAL
			(
				static_cast<std::int64_t>(chaos::murmur3::hash32("dataset:prefilter:license_journal")),
				3622373932
			);
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
