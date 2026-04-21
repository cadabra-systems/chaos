cmake_minimum_required(VERSION 3.12.4)

include_guard(GLOBAL)

find_package(PkgConfig REQUIRED)

pkg_search_module(PC_RDKAFKA REQUIRED rdkafka)
find_path(RDKAFKA_INCLUDE_DIRS NAMES librdkafka/rdkafka.h HINTS ${PC_RDKAFKA_INCLUDEDIR} ${PC_RDKAFKA_INCLUDE_DIRS})
find_library(RDKAFKA_LIBRARIES NAMES rdkafka HINTS ${PC_RDKAFKA_DIR} ${PC_RDKAFKA_LIBRARY_DIRS})
add_library(RdKafka::RdKafka SHARED IMPORTED)
set_target_properties(RdKafka::RdKafka PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${RDKAFKA_INCLUDE_DIRS}")
set_target_properties(RdKafka::RdKafka PROPERTIES IMPORTED_LOCATION "${RDKAFKA_LIBRARIES}")
