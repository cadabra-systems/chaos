/**
 @file Backtrace.cpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "Backtrace.hpp"

#include "String.hpp"
#include "Demangler.hpp"

#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <libunwind.h> //#include <execinfo.h>

namespace chaos {
	std::list<std::pair<std::string, std::uintptr_t>> backtrace::stack()
	{
		char proc_name[1024];

		unw_context_t context;
		unw_cursor_t cursor;
		unw_word_t ip, offset;

		unw_getcontext(&context);
		unw_init_local(&cursor, &context);

		std::list<std::pair<std::string, std::uintptr_t>> retval;
		while (unw_step(&cursor) > 0) {
			unw_get_reg(&cursor, UNW_REG_IP, &ip);
			if (0 == ip) {
				break;
			} else if (unw_get_proc_name(&cursor, proc_name, sizeof(proc_name), &offset) == 0) {
				retval.emplace_back(demangler::name(proc_name), offset);
			} else {
				retval.emplace_back("???", 0);
			}
		}
/*
		void* buffer[100];
		const int stack_size(::backtrace(buffer, 100));
		const std::unique_ptr<char*> stack_trace(::backtrace_symbols(buffer, stack_size));
		if (nullptr == stack_trace) {
			return {};
		}
		std::list<std::string> retval;
		for (int s = 0; s < stack_size; s++) {
			retval.emplace_back(stack_trace.get()[s], 0);
		}
*/
		return retval;
	}

	std::list<std::string> backtrace::list()
	{
		std::list<std::string> string_list;
		const std::list<std::pair<std::string, std::uintptr_t>> frame_list(stack());
		std::for_each(
						frame_list.cbegin(), frame_list.cend(),
						[&string_list](const std::pair<std::string, std::uintptr_t>& frame)
						{
							string_list.emplace_back(string::formatted_string("%s+0x%lx\n", frame.first.data(), frame.second));
						}
		);
		return string_list;
	}
}
