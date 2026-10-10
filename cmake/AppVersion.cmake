# AppVersion.cmake - collects VCS state and generates version/appversion.h
#
# This file is designed to be run as a CMake script on every build:
#
#	cmake
#		-DAPP_SOURCE_DIR=<top-level project source directory>
#		-DAPP_BINARY_DIR=<metamod target binary directory>
#		[-DAPP_COMMIT_COUNT=<override, auto-detected from git if empty>]
#		[-DAPP_COMMIT_SHA=<override, auto-detected from git if empty>]
#		[-DAPP_COMMIT_DATE=<override, auto-detected from git if empty>]
#		[-DAPP_COMMIT_TIME=<override, auto-detected from git if empty>]
#		[-DAPP_COMMIT_DIRTY=<override, auto-detected from git if empty>]
#		-P cmake/AppVersion.cmake
#
# configure_file() rewrites the header only when its content actually
# changes, so the projects depending on it get rebuilt only when the
# version information is really different.

if(NOT APP_SOURCE_DIR)
	message(FATAL_ERROR "APP_SOURCE_DIR must be specified, e.g. -DAPP_SOURCE_DIR=${CMAKE_SOURCE_DIR}")
endif()

if(NOT APP_BINARY_DIR)
	message(FATAL_ERROR "APP_BINARY_DIR must be specified, e.g. -DAPP_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}")
endif()

find_package(Git)

if(GIT_FOUND)
	# note: git commands must be executed in the repository, the caller is
	# supposed to pass a directory inside the working tree as APP_SOURCE_DIR
	function(git VAR)
		execute_process(
				COMMAND ${GIT_EXECUTABLE} ${ARGN}
				WORKING_DIRECTORY ${APP_SOURCE_DIR}
				OUTPUT_STRIP_TRAILING_WHITESPACE
				OUTPUT_VARIABLE output
				COMMAND_ERROR_IS_FATAL ANY
		)

		set(${VAR} ${output} PARENT_SCOPE)
	endfunction()

	if(NOT APP_COMMIT_COUNT)
		git(APP_COMMIT_COUNT rev-list --count HEAD)
	endif()

	if(NOT APP_COMMIT_SHA)
		git(APP_COMMIT_SHA log HEAD -1 --format=%h)
	endif()

	if(NOT APP_COMMIT_DATE)
		git(APP_COMMIT_DATE log HEAD -1 --format=%ad "--date=format:%b %d %Y")
	endif()

	if(NOT APP_COMMIT_TIME)
		git(APP_COMMIT_TIME log HEAD -1 --format=%ad "--date=format:%H:%M:%S")
	endif()

	if(APP_COMMIT_DIRTY STREQUAL "")
		execute_process(
				COMMAND ${GIT_EXECUTABLE} status --porcelain
				WORKING_DIRECTORY ${APP_SOURCE_DIR}
				RESULT_VARIABLE exit_code
				OUTPUT_VARIABLE output
				OUTPUT_STRIP_TRAILING_WHITESPACE
				COMMAND_ERROR_IS_FATAL ANY
		)

		if(NOT "${output}" STREQUAL "")
			set(APP_COMMIT_DIRTY TRUE)
		else()
			set(APP_COMMIT_DIRTY FALSE)
		endif()
	endif()
else()
	message(STATUS "Git not found, please specify -DAPP_COMMIT_*")

	if(NOT APP_COMMIT_COUNT)
		set(APP_COMMIT_COUNT "0")
	endif()

	if(NOT APP_COMMIT_SHA)
		set(APP_COMMIT_SHA "unknown")
	endif()

	if(NOT APP_COMMIT_DATE)
		set(APP_COMMIT_DATE "unknown")
	endif()

	if(NOT APP_COMMIT_TIME)
		set(APP_COMMIT_TIME "unknown")
	endif()

	if(APP_COMMIT_DIRTY STREQUAL "")
		set(APP_COMMIT_DIRTY FALSE)
	endif()
endif()

set(APP_VERSION_MAJOR 1)
set(APP_VERSION_MINOR 0)
set(APP_VERSION_MAINTENANCE 0)

# TODO: The old one checks for GH/BitBucket - is this still necessary? It's a pain to do.
set(APP_COMMIT_URL "https://github.com/FWGS/metamod-fwgs/commit/")

set(APP_VERSION      "${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}.${APP_VERSION_MAINTENANCE}.${APP_COMMIT_COUNT}")
set(APP_VERSION_C    "${APP_VERSION_MAJOR},${APP_VERSION_MINOR},${APP_VERSION_MAINTENANCE},${APP_COMMIT_COUNT}")
set(APP_VERSION_STRD "${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}.${APP_VERSION_MAINTENANCE}.${APP_COMMIT_COUNT}")

if(APP_COMMIT_DIRTY)
	set(APP_VERSION "${APP_VERSION}+m")
endif()

configure_file(
	"${APP_SOURCE_DIR}/metamod/version/appversion.h.in"
	"${APP_BINARY_DIR}/version/appversion.h"
)

message(STATUS "Metamod version: ${APP_VERSION}, commit: ${APP_COMMIT_SHA}")
