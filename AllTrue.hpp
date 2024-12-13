//
//  AllTrue.hpp
//  Cadabra
//
//  Created by Daniil A Megrabyan on 20.05.15.
//  Copyright (c) 2015 Cadabra Systems. All rights reserved.
//

#ifndef Chaos_AllTrue_hpp
#define Chaos_AllTrue_hpp

#include <type_traits>

namespace chaos {
	template<bool...>
	struct bool_pack;

	template<bool... v>
	using all_true = std::is_same<bool_pack<true, v...>, bool_pack<v..., true>>;
}

#endif
