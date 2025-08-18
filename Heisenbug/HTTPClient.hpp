/**
 @file Globus.hpp
 @date 15.04.21
 @copyright Cadabra Systems Inc
 @author daniil@megrabyan.pro
*/

#ifndef Chaos_Heisenbug_HTTPClient_hpp
#define Chaos_Heisenbug_HTTPClient_hpp

#include "../Heisenbug.hpp"

#include "../HTTPClient.hpp"
#include "../Flex.hpp"

namespace chaos {
	class http_client_test : public heisen_test
	{
	/** @name Statics */
	/** @{ */
	public:
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		http_client_test()
		:
			heisen_test("HTTPClient")
		{

		};
		virtual ~http_client_test() = default;
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
			HEISEN(EchoTest);
		}
	/** @} */
		
	/** @name Tests */
	/** @{ */
	private:
		/**
		 * @brief
		 */
		void testEchoTest()
		{
			http_client client("echo.free.beeceptor.com", true);
			IS_TRUE(client.send("/", flex{{"testkey", "testvalue"}}, http_client::send_mode::post))
			IS_TRUE(client.is_ok())
			IS_TRUE(client.get_content_type().kind(mime::media_type::application, "json"))
			ARE_EQUAL(flex::from_json(client.get_content_body())["parsedBody"], flex({{"testkey", "testvalue"}}))
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
