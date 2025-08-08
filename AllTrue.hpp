//
//  AllTrue.hpp
//  Cadabra
//
//  Created by Daniil A Megrabyan on 20.05.15.
//  Copyright (c) 2015 Cadabra Systems. All rights reserved.
//

#ifndef Chaos_AllTrue_hpp
#define Chaos_AllTrue_hpp

namespace chaos {
	template<typename... Args>
	constexpr bool all_true(const Args&... args)
	{
		return (args && ...);
	}
}

#endif
