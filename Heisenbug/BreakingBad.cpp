/**
 @file BreakingBad.cpp
 @date 15.04.21
 @copyright Cadabra Systems Inc
 @author daniil@megrabyan.pro
*/

#include "BreakingBad.hpp"

#include "Basic.hpp"
#include "HTTPClient.hpp"
#include "System.hpp"
#include "AtomicHashTable.hpp"
#include "AtomicForwardList.hpp"
#include "CDO.hpp"
#include "Date.hpp"
#include "Flex.hpp"
#include "Any.hpp"
#include "Globus.hpp"
#include "ODBC.hpp"

namespace chaos {
	breaking_bad::breaking_bad()
	:
		heisen_test("chaos")
	{
		
	}

	void breaking_bad::masquerade(const std::string& test_mask, const std::string& case_mask)
	{
		_test_mask = test_mask;
		_case_mask = case_mask;
		run(_test_mask);
	}

	void breaking_bad::structuralize()
	{
		HEISEN(Basic)
		HEISEN(HTTPClient)
		HEISEN(Any)
		HEISEN(Date)
		HEISEN(Flex)
		HEISEN(System)
		HEISEN(AtomicHashTable)
		HEISEN(AtomicForwardList)
		HEISEN(CDO)
		HEISEN(Globus)
		HEISEN(ODBC)
	}

	const std::string& breaking_bad::get_mask() const
	{
		return _test_mask;
	}

	void breaking_bad::testBasic()
	{
		chaos::basic_test().run(_case_mask);
	}

	void breaking_bad::testSystem()
	{
		chaos::system_test().run(_case_mask);
	}

	void breaking_bad::testAtomicHashTable()
	{
		chaos::atomic_hash_table_test().run(_case_mask);
	}

	void breaking_bad::testAtomicForwardList()
	{
		chaos::atomic_forward_list_test().run(_case_mask);
	}

	void breaking_bad::testHTTPClient()
	{
		chaos::http_client_test().run(_case_mask);
	}

	void breaking_bad::testCDO()
	{
		chaos::cdo_test().run(_case_mask);
	}

	void breaking_bad::testFlex()
	{
		chaos::flex_test().run(_case_mask);
	}

	void breaking_bad::testCWS()
	{
	}

	void breaking_bad::testString()
	{
	}

	void breaking_bad::testDate()
	{
		chaos::date_test().run(_case_mask);
	}

	void breaking_bad::testAny()
	{
		chaos::any_test().run(_case_mask);
	}

	void breaking_bad::testGlobus()
	{
		chaos::globus_test().run(_case_mask);
	}

	void breaking_bad::testODBC()
	{
		chaos::odbc_test().run(_case_mask);
	}
}
