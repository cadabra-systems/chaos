cmake_minimum_required(VERSION 3.12.4)

include_guard(GLOBAL)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(OPENSSL_ROOT_DIR "${BREW_HOME}/openssl/lib")
	set(OPENSSL_INCLUDE_DIR "${BREW_HOME}/openssl/include")
	message(STATUS "OpenSSL was detected at ${OPENSSL_ROOT_DIR} and ${OPENSSL_INCLUDE_DIR}")
endif ()

find_package(OpenSSL REQUIRED)
