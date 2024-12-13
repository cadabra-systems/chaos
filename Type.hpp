//
//  Type.hpp
//
//  Created by Daniil A Megrabyan on 25.08.15.
//  Copyright (c) 2015 Cadabra Systems. All rights reserved.
//

#ifndef Chaos_Type_hpp
#define Chaos_Type_hpp

#include <memory>

namespace chaos {
	template<typename T>
	class type
	{
	/** @name Aliases */
	/** @{ */
	public:

	/** @} */
	
	/** @name Overloads */
	/** @{ */
	public:
		std::uint16_t operator()(const std::shared_ptr<const T> value) const
		{
			return value->geType();
		}
	/** @} */
	};
}

#endif
