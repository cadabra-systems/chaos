/**
 @file CTime.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_Flex_CTime_hpp
#define Chaos_Flex_CTime_hpp

#include "../Flex.hpp"

#include <ctime>

namespace chaos {
	template <>
	struct adl_serializer<std::tm> {
		static void to_flex(flex& output, const std::tm& input)
		{
			output = std::asctime(&input);
		}

		static void from_flex(const flex& input, std::tm& output)
		{
			/// @todo not implemented
/*
			locale C:
				asctime                             Wed Nov  4 00:45:01 2020
				strftime %c                         Wed Nov  4 00:45:01 2020
				strftime %a %b %e %H:%M:%S %Y       Wed Nov  4 00:45:01 2020

			locale en_US.utf8:
				asctime                             Wed Nov  4 00:45:01 2020
				strftime %c                         Wed 04 Nov 2020 12:45:01 AM UTC
				strftime %a %b %e %H:%M:%S %Y       Wed Nov  4 00:45:01 2020

			locale de_DE.utf8:
				asctime                             Wed Nov  4 00:45:01 2020
				strftime %c                         Mi 04 Nov 2020 00:45:01 UTC
				strftime %a %b %e %H:%M:%S %Y       Mi Nov  4 00:45:01 2020

			locale ja_JP.utf8:
				asctime                             Wed Nov  4 00:45:01 2020
				strftime %c                         2020年11月04日 00時45分01秒
				strftime %a %b %e %H:%M:%S %Y       水 11月  4 00:45:01 2020
*/

		}
	};
}

#endif
