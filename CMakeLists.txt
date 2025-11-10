cmake_minimum_required(VERSION 3.10)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

project(metamod-fwgs LANGUAGES C CXX)

# Project-wide options
option(DEBUG "Build with debug information." OFF)
option(USE_STATIC_LIBSTDC "Enables static linking libstdc++." OFF)
option(BUILD_EXAMPLE "Build example plugin (if available)" OFF)

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

# Expose options to subdirectories
set(DEBUG ${DEBUG} CACHE BOOL "Build with debug information.")
set(USE_STATIC_LIBSTDC ${USE_STATIC_LIBSTDC} CACHE BOOL "Enables static linking libstdc++.")
set(BUILD_EXAMPLE ${BUILD_EXAMPLE} CACHE BOOL "Build example plugin (if available)")

# Appversion target: run platform-specific script if present
set(APPVERSION_SCRIPT "${CMAKE_SOURCE_DIR}/metamod/version/appversion.sh")
set(APPVERSION_HEADER "${CMAKE_SOURCE_DIR}/version/appversion.h")

# Create custom target appversion
if(WIN32)
  set(MSVC_PREBUILD "${CMAKE_SOURCE_DIR}/msvc/PreBuild.bat")
  if(EXISTS "${MSVC_PREBUILD}")
    # Convert to native Windows paths
    file(TO_NATIVE_PATH "${MSVC_PREBUILD}" MSVC_PREBUILD_NATIVE)
    file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/version" MSVC_VERSION_NATIVE)
    file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}" MSVC_ROOT_NATIVE)

    add_custom_command(OUTPUT ${APPVERSION_HEADER}
        COMMAND ${CMAKE_COMMAND} -E echo "Running PreBuild.bat"
        COMMAND cmd /C call "${MSVC_PREBUILD_NATIVE}" "${MSVC_VERSION_NATIVE}" "${MSVC_ROOT_NATIVE}"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/msvc"
        COMMENT "Generating application version files (msvc PreBuild)"
        VERBATIM
    )

    add_custom_target(appversion DEPENDS ${APPVERSION_HEADER})
  endif()
elseif(EXISTS "${APPVERSION_SCRIPT}" AND NOT WIN32)
  add_custom_command(OUTPUT ${APPVERSION_HEADER}
    COMMAND ${CMAKE_COMMAND} -E env "SHELL=/bin/sh" bash "${APPVERSION_SCRIPT}" "${CMAKE_SOURCE_DIR}"
    COMMENT "Generating application version files (sh)"
    VERBATIM
  )
  add_custom_target(appversion DEPENDS ${APPVERSION_HEADER})
else()
  message(STATUS "No appversion script found; skipping appversion target")
endif()

# Add main metamod directory
if(EXISTS "${CMAKE_SOURCE_DIR}/metamod/CMakeLists.txt")
  add_subdirectory(metamod)
else()
  message(FATAL_ERROR "Required subdirectory 'metamod' not found")
endif()

# Optionally build example plugin (use CMakeLists in example if present, otherwise try existing Makefile)
if(BUILD_EXAMPLE)
  if(EXISTS "${CMAKE_SOURCE_DIR}/metamod/extra/example/CMakeLists.txt")
    add_subdirectory(metamod/extra/example)
  elseif(EXISTS "${CMAKE_SOURCE_DIR}/metamod/extra/example/Makefile")
    add_custom_target(example
      COMMAND ${CMAKE_COMMAND} -E chdir "${CMAKE_SOURCE_DIR}/metamod/extra/example" ${CMAKE_MAKE_PROGRAM}
      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/metamod/extra/example"
      COMMENT "Building example plugin using the provided Makefile"
    )
  else()
    message(STATUS "No example build files found under metamod/extra/example")
  endif()
endif()

# Install rules for metamod target if available
if(TARGET metamod)
  install(TARGETS metamod
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
  )
endif()

message(STATUS "Configuration summary:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  DEBUG option: ${DEBUG}")
message(STATUS "  USE_STATIC_LIBSTDC option: ${USE_STATIC_LIBSTDC}")
message(STATUS "  BUILD_EXAMPLE option: ${BUILD_EXAMPLE}")
