#ifndef Chaos_Flex_UniquePointer_hpp
#define Chaos_Flex_UniquePointer_hpp

#include "../Flex.hpp"

namespace chaos {
	template <typename T>
	struct adl_serializer<std::unique_ptr<T>> {
		static void to_flex(chaos::flex& output, const std::unique_ptr<T>& input) {
			if (!input) {
				output = nullptr;
			} else {
				output = *input;
			}
		}
	};
}

#endif
