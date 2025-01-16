cmake_minimum_required(VERSION 3.12.4)

include_guard(GLOBAL)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(ICU_ROOT "${BREW_HOME}/icu4c")
	set(ICU_DEBUG TRUE)
	find_package(ICU COMPONENTS uc i18n REQUIRED)
	set_target_properties(ICU::uc PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${BREW_HOME}/icu4c/include")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	find_package(ICU COMPONENTS uc i18n REQUIRED)
else ()
	message(FATAL_ERROR "Unknown build system, exiting..." )
endif ()
