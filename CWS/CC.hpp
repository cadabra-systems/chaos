/**
 @file CC.hpp
 @date 20.02.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
 */

#ifndef Chaos_CWS_CC_hpp
#define Chaos_CWS_CC_hpp

#include "../Log.hpp"
namespace chaos { namespace cws {
	enum class service
	{
		cvs = 'v', /// < Cadabra Video Service
		cns = 'n', ///   Cadabra Notification Service
		cdn = 'd', /// < Cadabra Delivery Network
		cts = 't', /// < Cadabra Tube Service
		sos = 's',
		fiscus = 'f'  /// < Fiscal Service
	};
} }

#endif



