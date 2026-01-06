cmake_minimum_required(VERSION 3.19)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

project(metamod-fwgs LANGUAGES C CXX)

# Project-wide options
option(USE_STATIC_LIBSTDC "Enables static linking libstdc++" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Xash3D FWGS Library Naming Scheme compliance
# see documentation: https://github.com/FWGS/xash3d-fwgs/blob/master/Documentation/extensions/library-naming.md
include(LibraryNaming)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	message(STATUS "Building for 64-bit")
else()
	message(STATUS "Building for 32-bit")
endif()

# Default to Release if not specified for single-config generators
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "No build type specified. Defaulting to Release.")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

# Detect platform and compiler
if(WIN32)
  set(TARGET_PLATFORM "Windows")
else()
  set(TARGET_PLATFORM "Unix")
endif()

if(MSVC)
  set(TARGET_COMPILER "MSVC")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(TARGET_COMPILER "Clang")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(TARGET_COMPILER "GCC")
else()
  set(TARGET_COMPILER "Unknown")
endif()

message(STATUS "Configuring for ${TARGET_PLATFORM} using ${TARGET_COMPILER}")

# Basic compiler flags per toolchain (kept conservative; subprojects can extend)
if(MSVC)
  add_compile_options(/W3 /EHsc)
else()
  # Common sane defaults for GCC/Clang
  add_compile_options(-Wall -Wextra -Wno-unused-parameter)
  # Ensure position-independent code when building shared libs on Unix
  if(NOT WIN32)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
  endif()
endif()

# Appversion target: run platform-specific script
if(WIN32)
    set(MSVC_PREBUILD "${CMAKE_SOURCE_DIR}/metamod/version/appversion.bat")
    file(TO_NATIVE_PATH "${MSVC_PREBUILD}" SCRIPT_PATH_NATIVE)
    file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/metamod/version" VERSION_DIR_NATIVE)
    file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}" PROJECT_ROOT_NATIVE)

    add_custom_command(OUTPUT appversion_command
        COMMAND cmd /C call "${SCRIPT_PATH_NATIVE}" "${VERSION_DIR_NATIVE}" "${PROJECT_ROOT_NATIVE}"
        COMMENT "Generating application version header"
        VERBATIM
    )
	add_custom_target(appversion DEPENDS appversion_command)
else()
    set(APPVERSION_SCRIPT "${CMAKE_SOURCE_DIR}/metamod/version/appversion.sh")
    add_custom_command(OUTPUT appversion_command
        COMMAND ${CMAKE_COMMAND} -E env "SHELL=/bin/sh" bash "${APPVERSION_SCRIPT}" "${CMAKE_SOURCE_DIR}"
        COMMENT "Generating application version files (sh)"
        VERBATIM
    )
    add_custom_target(appversion DEPENDS appversion_command)
endif()

add_subdirectory(library-suffix)
add_subdirectory(metamod)

message(STATUS "Configuration summary:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  USE_STATIC_LIBSTDC option: ${USE_STATIC_LIBSTDC}")
