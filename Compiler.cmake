cmake_minimum_required(VERSION 3.12.4)

include_guard(GLOBAL)

include(CheckCXXCompilerFlag)

if (NOT CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
	message(FATAL_ERROR "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	message(FATAL_ERROR "MS Windows is currently unsupported")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	# Noting special yet...
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	foreach(BREW /opt/homebrew;/usr/local/Homebrew)
		execute_process(COMMAND ${BREW}/bin/brew --prefix RESULT_VARIABLE BREW_RESULT OUTPUT_VARIABLE BREW_HOME OUTPUT_STRIP_TRAILING_WHITESPACE)
		if (BREW_RESULT EQUAL 0)
			break()
		endif()
	endforeach()
	if (NOT BREW_RESULT EQUAL 0 OR NOT IS_DIRECTORY ${BREW_HOME})
		message(FATAL_ERROR "Homebrew was not detected: ${BREW_RESULT}, exiting...")
	else()
		message(NOTICE "Homebrew was detected at ${BREW_HOME}")
	endif()
	set(BREW_HOME ${BREW_HOME}/opt)
endif ()
message(STATUS "CXX Compiler: ${CMAKE_CXX_COMPILER_ID}, Version: ${CMAKE_CXX_COMPILER_VERSION}")

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ Standard (toolchain)" FORCE)
set(CMAKE_CXX_STANDARD_REQUIRED YES CACHE BOOL "C++ Standard required" FORCE)
set(CMAKE_CXX_EXTENSIONS NO CACHE BOOL "C++ Standard extensions" FORCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++" CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
set(CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS_INIT} -std=c++17" CACHE STRING "" FORCE)
set(THREADS_PREFER_PTHREAD_FLAG ON)

add_compile_definitions($<$<CONFIG:DEBUG>:DEBUG=1>)
add_compile_definitions($<$<CONFIG:DEBUG>:NDEBUG>)

add_compile_options(-Wall)
add_compile_options(-Wno-unused-parameter)
add_compile_options(-Wno-unused-private-field)
add_compile_options($<$<CONFIG:DEBUG>:-g3>)
add_compile_options($<$<CONFIG:DEBUG>:-O0>)
add_compile_options($<$<CONFIG:RELEASE>:-O3>)

list(APPEND SANITIZER_FLAGS
	null
	leak
	address
	undefined
	nullability
	float-divide-by-zero
)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
	list(REMOVE_ITEM SANITIZER_FLAGS "leak")
endif ()
foreach (SANITIZER_FLAG IN LISTS SANITIZER_FLAGS)
	check_cxx_compiler_flag("-fsanitize=${SANITIZER_FLAG}" SANITIZER_SUPPORT)
	if (NOT SANITIZER_SUPPORT)
		list(REMOVE_ITEM SANITIZER_FLAGS ${SANITIZER_FLAG})
		message(STATUS "Sanitizer flag '${SANITIZER_FLAG}' is not supported")
	endif ()
endforeach ()
list(JOIN SANITIZER_FLAGS "," SANITIZER_FLAG)
if (SANITIZER_FLAG)
	message(STATUS "Sanitizer flags '${SANITIZER_FLAG}' are supported")
	set(CHAOS_CXX_SANITIZER_FLAGS "${CHAOS_CXX_SANITIZER_FLAGS} -fsanitize=${SANITIZER_FLAG}")
endif ()
if (CHAOS_CXX_SANITIZER_FLAGS)
	string(STRIP ${CHAOS_CXX_SANITIZER_FLAGS} CHAOS_CXX_SANITIZER_FLAGS)
endif ()

check_cxx_compiler_flag("-fno-omit-frame-pointer" OPTIMIZER_FLAG_NO_OMIT_FRAME_POINTER)
if (OPTIMIZER_FLAG_NO_OMIT_FRAME_POINTER)
	set(CHAOS_CXX_OPTIMIZER_FLAGS "${CHAOS_CXX_OPTIMIZER_FLAGS} -fno-omit-frame-pointer")
	message(STATUS "Optimizer flag 'no-omit-frame-pointer' is supported")
endif ()
if (CHAOS_CXX_OPTIMIZER_FLAGS)
	string(STRIP ${CHAOS_CXX_OPTIMIZER_FLAGS} CHAOS_CXX_OPTIMIZER_FLAGS)
endif ()
