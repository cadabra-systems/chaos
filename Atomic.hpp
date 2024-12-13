//
//  Atomic.hpp
//  Ace
//
//  Created by Daniil A Megrabyan on 03.04.17.
//  Copyright © 2017 Cadabra Systems. All rights reserved.
//

#ifndef Chaos_Atomic_hpp
#define Chaos_Atomic_hpp

#include <mutex>

namespace chaos{
	template<typename T>
	class atomic
	{
	public:
		atomic() = default;
		atomic(T value)
		:
			_value(value)
		{
			
		};
		atomic(const atomic&) = delete;
		
		void store(T value)
		{
			std::lock_guard<std::mutex> lock(_mutex);
			
			_value = value;
		};
		
		T exchange(T value)
		{
			std::lock_guard<std::mutex> lock(_mutex);
			
			T retval(_value);
			_value = value;
			
			return retval;
		};
		
		T load() const
		{
			std::lock_guard<std::mutex> lock(_mutex);
			
			return _value;
		};
		
		operator T() const
		{
			std::lock_guard<std::mutex> lock(_mutex);
			
			return _value;
		};
	
	private:
		T _value;
		mutable std::mutex _mutex;
	};
}
#endif
