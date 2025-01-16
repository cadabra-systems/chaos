/**
 @file Heisenbug.cpp
 @date 2022-04-15
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "Heisenbug/BreakingBad.hpp"

#include "String.hpp"

#include <vector>
#include <string>

int main(int argc, char* argv[])
{
	std::vector<std::string> mask_vector;
	mask_vector.reserve(argc - 1);
	for (int a = 1; a < argc; ++a) {
		mask_vector.push_back(argv[a]);
	}

	chaos::breaking_bad chaos_test;
	for (const std::string& mask : mask_vector) {
		const std::vector mask_piece_vector(chaos::string::explode<std::vector, std::string>(mask, "."));
		if (mask_piece_vector.size() > 1) {
			chaos_test.masquerade(mask_piece_vector.at(0), mask_piece_vector.at(1));
		}
	}

	return EXIT_SUCCESS;
}
