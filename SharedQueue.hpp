//
//  SharedQueue.hpp
//  Abra
//
//  Created by Daniil A Megrabyan on 02.04.15.
//  Copyright (c) 2015 Cadabra Systems. All rights reserved.
//

#ifndef Chaos_SharedQueue_hpp
#define Chaos_SharedQueue_hpp

#include <queue>
#include <mutex>
#include <exception>
#include <condition_variable>

namespace chaos {
	template<typename T>
	class shared_queue
	{
	std::queue<T> _queue;
	mutable std::mutex _mutex;
	std::condition_variable _condition;

	shared_queue& operator=(const shared_queue&) = delete;
	shared_queue(const shared_queue& other) = delete;
	
	public:
		shared_queue() = default;
		~shared_queue() = default;

		void push(T item)
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_queue.push(item);
			_condition.notify_one();
		}

		bool try_and_pop(T& popped_item)
		{
			std::lock_guard<std::mutex> lock(_mutex);
			if (_queue.empty()) {
				return false;
			}

			popped_item = _queue.front();
			_queue.pop();

			return true;
		}
		
		void wait_and_pop(T& popped_item)
		{
			std::unique_lock<std::mutex> lock(_mutex);
			while(_queue.empty())
			{
				_condition.wait(lock);
			}
			
			popped_item = _queue.front();
			_queue.pop();
		}

		bool empty() const
		{
			std::lock_guard<std::mutex> lock(_mutex);

			return _queue.empty();
		}

		unsigned size() const
		{
			std::lock_guard<std::mutex> lock(_mutex);

			return _queue.size();
		}
	};
}

#endif
