include_guard(GLOBAL)

# The vars themselves. Keep these CACHE so they can be overridden with -D
set(APP_COMMIT_COUNT "" CACHE STRING "Commit count (auto-detected from git if empty)")
set(APP_COMMIT_SHA   "" CACHE STRING "Commit SHA (auto-detected from git if empty)")
set(APP_COMMIT_DATE  "" CACHE STRING "Commit date (auto-detected from git if empty)")
set(APP_COMMIT_TIME  "" CACHE STRING "Commit time (auto-detected from git if empty)")
set(APP_COMMIT_DIRTY "" CACHE STRING "Commit dirty flag (auto-detected from git if empty)")

find_package(Git)

if(GIT_FOUND)
	function(git VAR)
		execute_process(
				COMMAND ${GIT_EXECUTABLE} ${ARGN}
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
