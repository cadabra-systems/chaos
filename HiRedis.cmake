cmake_minimum_required(VERSION 3.12.4)

include_guard(GLOBAL)

find_package(PkgConfig REQUIRED)

pkg_search_module(PC_HIREDIS REQUIRED hiredis)
find_path(HIREDIS_INCLUDE_DIRS NAMES hiredis/hiredis.h HINTS ${PC_HIREDIS_INCLUDEDIR} ${PC_HIREDIS_INCLUDE_DIRS} PATH_SUFFIXES hiredis)
find_library(HIREDIS_LIBRARIES NAMES hiredis HINTS ${PC_HIREDIS_DIR} ${PC_HIREDIS_LIBRARY_DIRS})
add_library(HiRedis::HiRedis SHARED IMPORTED)
set_target_properties(HiRedis::HiRedis PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${HIREDIS_INCLUDE_DIRS}")
set_target_properties(HiRedis::HiRedis PROPERTIES IMPORTED_LOCATION "${HIREDIS_LIBRARIES}")
