cmake_minimum_required(VERSION 3.12.4)

include_guard(GLOBAL)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	find_package(PkgConfig REQUIRED)
	pkg_search_module(PC_UUID REQUIRED uuid)
	find_path(UUID_INCLUDE_DIRS NAMES uuid/uuid.h HINTS ${PC_UUID_INCLUDEDIR} ${PC_UUID_INCLUDE_DIRS} PATH_SUFFIXES uuid)
	find_library(UUID_LIBRARIES NAMES uuid HINTS ${PC_UUID_DIR} ${PC_UUID_LIBRARY_DIRS})
	add_library(UUid::UUid SHARED IMPORTED)
	set_target_properties(UUid::UUid PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${UUID_INCLUDE_DIRS}")
	set_target_properties(UUid::UUid PROPERTIES IMPORTED_LOCATION "${UUID_LIBRARIES}")
endif ()
