/**
 @file Heisenbug.cpp
 @date 2022-04-15
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "Heisenbug/BreakingBad.hpp"

#include "String.hpp"
#include "LogThread.hpp"
#include "Log.hpp"
#include "ODBC/Log.hpp"
#include "Redis/Log.hpp"
#include "Kafka/Log.hpp"
#include "RabbitMQ/Log.hpp"

#include <vector>
#include <string>
#include <iostream>

int main(int argc, char* argv[])
{
	chaos::log_level verbose_level(chaos::log_level::checkpoint);
	std::vector<std::string> mask_vector; mask_vector.reserve(argc - 1);
	for (int a = 1; a < argc; ++a) {
		const std::string arg(argv[a]);
		if (arg.substr(0, 10) == "--verbose=") {
			const std::string level(arg.substr(10));
			if (level == "debug") {
				verbose_level = chaos::log_level::debug;
			} else if (level == "checkpoint") {
				verbose_level = chaos::log_level::checkpoint;
			} else if (level == "notice") {
				verbose_level = chaos::log_level::notice;
			} else if (level == "warning") {
				verbose_level = chaos::log_level::warning;
			} else if (level == "alert") {
				verbose_level = chaos::log_level::alert;
			} else if (level == "error") {
				verbose_level = chaos::log_level::error;
			} else if (level == "critical") {
				verbose_level = chaos::log_level::critical;
			} else if (level == "fatal") {
				verbose_level = chaos::log_level::fatal;
			} else if (level == "dead") {
				verbose_level = chaos::log_level::dead;
			}
		} else {
			mask_vector.push_back(arg);
		}
	}

	chaos::log_register<chaos::log>::thread(chaos::log_thread::shared_instance());
	chaos::log_register<chaos::log>::stream(std::make_unique<chaos::text_log_stream>(std::clog), verbose_level);
	chaos::log_register<chaos::odbc::log>::stream(std::make_unique<chaos::text_log_stream>(std::clog), verbose_level);
	chaos::log_register<chaos::redis::log>::stream(std::make_unique<chaos::text_log_stream>(std::clog), verbose_level);
	chaos::log_register<chaos::kafka::log>::stream(std::make_unique<chaos::text_log_stream>(std::clog), verbose_level);
	chaos::log_register<chaos::rabbitmq::log>::stream(std::make_unique<chaos::text_log_stream>(std::clog), verbose_level);

	chaos::breaking_bad chaos_test;
	for (const std::string& mask : mask_vector) {
		const std::vector mask_piece_vector(chaos::string::explode<std::vector, std::string>(mask, "."));
		if (mask_piece_vector.size() > 1) {
			chaos_test.masquerade(mask_piece_vector.at(0), mask_piece_vector.at(1));
		}
	}

	return EXIT_SUCCESS;
}
