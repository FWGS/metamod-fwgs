cmake_minimum_required(VERSION 3.10)
project(metamod CXX)

# Source/include directories
set(PROJECT_SRC_DIR
	"${PROJECT_SOURCE_DIR}"
	"${PROJECT_SOURCE_DIR}/src"
	"${PROJECT_SOURCE_DIR}/include/engine"
	"${PROJECT_SOURCE_DIR}/include/common"
	"${PROJECT_SOURCE_DIR}/include/dlls"
	"${PROJECT_SOURCE_DIR}/include/game_shared"
	"${PROJECT_SOURCE_DIR}/include/pm_shared"
	"${PROJECT_SOURCE_DIR}/include/public"
)

set(PROJECT_PUBLIC_DIR
	"${PROJECT_SOURCE_DIR}/include/public"
	"${PROJECT_SOURCE_DIR}/include/public/metamod"
)

# Collect sources
set(METAMOD_SOURCES
	"src/api_info.cpp"
	"src/commands_meta.cpp"
	"src/conf_meta.cpp"
	"src/dllapi.cpp"
	"src/engine_api.cpp"
	"src/game_support.cpp"
	"src/h_export.cpp"
	"src/log_meta.cpp"
	"src/mdebug.cpp"
	"src/mem_utils.cpp"
	"src/meta_rehlds_api.cpp"
	"src/metamod.cpp"
	"src/mextdll.cpp"
	"src/mlist.cpp"
	"src/mplayer.cpp"
	"src/mplugin.cpp"
	"src/mreg.cpp"
	"src/mutil.cpp"
	"src/precompiled.cpp"
	"src/public_amalgamation.cpp"
	"src/reg_support.cpp"
	"src/sdk_util.cpp"
	"src/studioapi.cpp"
	"src/sys_module.cpp"
	"src/utils.cpp"
	"src/glibc_compat.cpp"
)

if (WIN32)
    list(APPEND METAMOD_SOURCES "src/osdep_linkent_win32.cpp")
else()
    list(APPEND METAMOD_SOURCES "src/osdep_linkent_linux.cpp")
endif()

# required for ABI compatibility
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	list(APPEND METAMOD_SOURCES "msvc/metamod.rc")
	list(APPEND METAMOD_SOURCES "msvc/metamod.def")
endif()

add_library(metamod SHARED ${METAMOD_SOURCES})
set(APPVERSION_HEADER "${PROJECT_SOURCE_DIR}/version/appversion.h")

if (WIN32)
    set(MSVC_PREBUILD "${PROJECT_SOURCE_DIR}/msvc/PreBuild.bat")
    set(MSVC_POSTBUILD "${PROJECT_SOURCE_DIR}/msvc/PostBuild.bat")

    if (EXISTS "${MSVC_PREBUILD}")
        # Convert to native Windows paths
        file(TO_NATIVE_PATH "${MSVC_PREBUILD}" MSVC_PREBUILD_NATIVE)
        file(TO_NATIVE_PATH "${PROJECT_SOURCE_DIR}/version" MSVC_VERSION_NATIVE)
        file(TO_NATIVE_PATH "${PROJECT_SOURCE_DIR}" MSVC_ROOT_NATIVE)

        add_custom_command(OUTPUT ${APPVERSION_HEADER}
            COMMAND ${CMAKE_COMMAND} -E echo "Running PreBuild.bat"
            COMMAND cmd /C call "${MSVC_PREBUILD_NATIVE}" "${MSVC_VERSION_NATIVE}" "${MSVC_ROOT_NATIVE}"
            WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/msvc"
            COMMENT "Generating application version files (msvc PreBuild)"
            VERBATIM
        )

        add_custom_target(appversion_prebuild DEPENDS ${APPVERSION_HEADER})
        add_dependencies(metamod appversion_prebuild)
    endif()

    if (EXISTS "${MSVC_POSTBUILD}")
        file(TO_NATIVE_PATH "${MSVC_POSTBUILD}" MSVC_POSTBUILD_NATIVE)

        add_custom_command(TARGET metamod POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Running PostBuild.bat"
            COMMAND cmd /C call "${MSVC_POSTBUILD_NATIVE}" "$<TARGET_FILE_DIR:metamod>" "$<TARGET_FILE_NAME:metamod>" "$<TARGET_FILE_SUFFIX:metamod>" "${PROJECT_SOURCE_DIR}"
            WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/msvc"
            COMMENT "Running msvc PostBuild script"
            VERBATIM
        )
    endif()
else()
    if (EXISTS "${PROJECT_SOURCE_DIR}/version/appversion.sh")
        add_custom_command(OUTPUT ${APPVERSION_HEADER}
            COMMAND ${CMAKE_COMMAND} -E env "SHELL=/bin/sh" sh "${PROJECT_SOURCE_DIR}/version/appversion.sh" "${PROJECT_SOURCE_DIR}"
            COMMENT "Generating application version files (sh)"
            VERBATIM
        )
        add_custom_target(appversion DEPENDS ${APPVERSION_HEADER})
        add_dependencies(metamod appversion)
    endif()
endif()


target_include_directories(metamod PRIVATE
	${PROJECT_SRC_DIR}
	${PROJECT_PUBLIC_DIR}
)

# Compiler-specific and platform-specific flags
set(METAMOD_COMPILE_OPTIONS "")
set(METAMOD_LINK_OPTIONS "")

if (MSVC)
    # MSVC sensible defaults
    list(APPEND METAMOD_COMPILE_OPTIONS /W3 /EHsc)
    # Avoid C runtime warnings
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    # GCC / Clang common options
    list(APPEND METAMOD_COMPILE_OPTIONS -Wall -Wextra -Wno-unused-parameter -fno-exceptions -fno-builtin -Wno-unknown-pragmas)

    # Try to enable some optimizations and sane defaults
    if (DEBUG)
        list(APPEND METAMOD_COMPILE_OPTIONS -g3 -O0 -ggdb)
    else()
        list(APPEND METAMOD_COMPILE_OPTIONS -g0 -O3 -fno-stack-protector)
        list(APPEND METAMOD_LINK_OPTIONS -s)
    endif()

    # Architecture/processor tuning - leave conservative default
    list(APPEND METAMOD_COMPILE_OPTIONS -mtune=generic -msse3)

    # Compiler specific tweaks
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 8.0)
            list(APPEND METAMOD_COMPILE_OPTIONS -Wno-stringop-truncation -Wno-format-truncation -Wno-class-memaccess -fcf-protection=none)
        endif()
        list(APPEND METAMOD_COMPILE_OPTIONS -Wno-maybe-uninitialized -Wno-unused-but-set-variable)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        # Clang specific adjustments can go here
        list(APPEND METAMOD_COMPILE_OPTIONS -Wno-maybe-uninitialized)
    endif()

    # Position independent code for shared libraries on non-Windows
    set_target_properties(metamod PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Apply compile and link options
target_compile_options(metamod PRIVATE ${METAMOD_COMPILE_OPTIONS})
# target_link_options requires CMake >= 3.13, but most modern envs have it; guard just in case
if (POLICY CMP0079)
    cmake_policy(SET CMP0079 NEW)
endif()
if (COMMAND target_link_options)
    target_link_options(metamod PRIVATE ${METAMOD_LINK_OPTIONS})
else()
    # Fallback to setting property
    set_target_properties(metamod PROPERTIES LINK_FLAGS "${METAMOD_LINK_OPTIONS}")
endif()

# MSVC: add configuration-specific flags using generator expressions to avoid conflicts such as /RTC1 vs /O2
if (MSVC)
    # Use /RTC1 only for Debug configuration; avoid it for Release where /O2 is used
    target_compile_options(metamod PRIVATE
        $<$<CONFIG:Debug>:/Zi;/Od;/RTC1>
        $<$<NOT:$<CONFIG:Debug>>:/O2>
    )
endif()

if (UNIX)
    target_link_libraries(metamod PRIVATE dl pthread)
elseif (WIN32)
    target_link_libraries(metamod PRIVATE psapi)
endif()

if (USE_STATIC_LIBSTDC)
    target_compile_definitions(metamod PRIVATE BUILD_STATIC_LIBSTDC)
    if (NOT MSVC)
        list(APPEND METAMOD_LINK_OPTIONS -static-libgcc -static-libstdc++)
        if (COMMAND target_link_options)
            target_link_options(metamod PRIVATE -static-libgcc -static-libstdc++)
        else()
            set_target_properties(metamod PROPERTIES LINK_FLAGS "${METAMOD_LINK_OPTIONS}")
        endif()
    endif()
endif()

target_compile_definitions(metamod PRIVATE
    METAMOD_CORE
)

if (WIN32)
    target_compile_definitions(metamod PRIVATE _WINDOWS)
else()
    # map Windows string functions to POSIX equivalents
    target_compile_definitions(metamod PRIVATE
        _LINUX
        LINUX
        _GLIBCXX_USE_CXX11_ABI=0
        _stricmp=strcasecmp
        _strnicmp=strncasecmp
        _strdup=strdup
        _unlink=unlink
        _write=write
        _close=close
        _getcwd=getcwd
        _vsnprintf=vsnprintf
        _vsnwprintf=vswprintf
        _snprintf=snprintf
    )
endif()

# NDEBUG in release mode
if (NOT DEBUG)
    target_compile_definitions(metamod PRIVATE NDEBUG)
endif()

set_target_properties(metamod PROPERTIES
    OUTPUT_NAME metamod
    PREFIX ""
)
