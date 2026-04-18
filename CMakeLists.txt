cmake_minimum_required(VERSION 3.19)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

include(AppVersion)

project(metamod-fwgs LANGUAGES C CXX)

# Project-wide options
option(USE_STATIC_RUNTIME "Enables static linking runtime library" ON)

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

add_subdirectory(library-suffix)
add_subdirectory(metamod)

message(STATUS "Configuration summary:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  USE_STATIC_RUNTIME option: ${USE_STATIC_RUNTIME}")
