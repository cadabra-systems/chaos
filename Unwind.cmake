cmake_minimum_required(VERSION 3.12.4)

include_guard(GLOBAL)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	find_path(UNWIND_INCLUDE_DIRS NAMES libunwind.h HINTS /usr/include /usr/local/include /usr/include/unwind /usr/local/include/unwind PATH_SUFFIXES libunwind unwind)
	find_library(UNWIND_LIBRARIES NAMES unwind HINTS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu /usr/local/lib/x86_64-linux-gnu)
	if(NOT UNWIND_INCLUDE_DIRS OR NOT UNWIND_LIBRARIES)
		message(FATAL_ERROR "Failed To find libunwind.")
	endif()
	add_library(Unwind::Unwind SHARED IMPORTED)
	set_target_properties(Unwind::Unwind PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${UNWIND_INCLUDE_DIRS}" IMPORTED_LOCATION "${UNWIND_LIBRARIES}")
endif ()
