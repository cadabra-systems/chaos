//
//  Any.cpp
//  Abra
//
//  Created by Daniil A Megrabyan on 20.05.15.
//  Copyright (c) 2015 Cadabra Systems. All rights reserved.
//

#include "Any.hpp"

namespace chaos {
	const any any::null = any(nullptr);
	const any any::none = any(std::nullopt);
}
