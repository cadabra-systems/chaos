/**
 @file DummyThread.cpp
 @date 02.04.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
*/

#include "DummyThread.hpp"

namespace chaos {
	std::shared_ptr<dummy_thread> dummy_thread::instance()
	{
		std::shared_ptr<dummy_thread> retval(new dummy_thread());
		return retval;
	}
	
	dummy_thread::dummy_thread()
	:
		worker_thread()
	{

	}
	
	dummy_thread::~dummy_thread()
	{
	
	}
	
	void dummy_thread::send(std::function<void()>)
	{
	
	}
}
