cmake_minimum_required(VERSION 3.19)
project(metamod CXX)

set(PROJECT_HEADERS_DIR
	"${PROJECT_SOURCE_DIR}"
	"${PROJECT_SOURCE_DIR}/src"
	"${PROJECT_SOURCE_DIR}/include/engine"
	"${PROJECT_SOURCE_DIR}/include/common"
	"${PROJECT_SOURCE_DIR}/include/dlls"
	"${PROJECT_SOURCE_DIR}/include/game_shared"
	"${PROJECT_SOURCE_DIR}/include/pm_shared"
	"${PROJECT_SOURCE_DIR}/include/public"
	"${CMAKE_SOURCE_DIR}/public"
)

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
)

# required for ABI compatibility
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	list(APPEND METAMOD_SOURCES "msvc/metamod.rc")
	list(APPEND METAMOD_SOURCES "msvc/metamod.def")
elseif(XASH_LINUX AND XASH_X86)
    list(APPEND METAMOD_SOURCES "src/glibc_compat.cpp")
endif()

add_library(metamod SHARED ${METAMOD_SOURCES})
add_dependencies(metamod appversion)

target_include_directories(metamod PRIVATE
	${PROJECT_HEADERS_DIR}
)

set(METAMOD_COMPILE_OPTIONS "")
set(METAMOD_LINK_OPTIONS "")

if (MSVC)
    list(APPEND METAMOD_COMPILE_OPTIONS /W3 /EHsc)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    list(APPEND METAMOD_COMPILE_OPTIONS -Wall -Wextra -Wno-unused-parameter -fno-exceptions -fno-builtin -Wno-unknown-pragmas)

    # debug mode flags
    list(APPEND METAMOD_COMPILE_OPTIONS
		$<$<CONFIG:Debug>:-g3>
		$<$<CONFIG:Debug>:-O0> 
		$<$<CONFIG:Debug>:-ggdb>)

	# release mode flags
    list(APPEND METAMOD_COMPILE_OPTIONS
		$<$<NOT:$<CONFIG:Debug>>:-g> 
		$<$<NOT:$<CONFIG:Debug>>:-O3>  
		$<$<NOT:$<CONFIG:Debug>>:-fno-stack-protector>)

    list(APPEND METAMOD_LINK_OPTIONS $<$<CONFIG:Release>:-s>) # strip debug symbols, but only in strict release mode
    list(APPEND METAMOD_COMPILE_OPTIONS -mtune=generic -msse3)

    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 8.0)
            list(APPEND METAMOD_COMPILE_OPTIONS -Wno-stringop-truncation -Wno-format-truncation -Wno-class-memaccess -fcf-protection=none)
        endif()
        list(APPEND METAMOD_COMPILE_OPTIONS -Wno-maybe-uninitialized -Wno-unused-but-set-variable)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        list(APPEND METAMOD_COMPILE_OPTIONS -Wno-maybe-uninitialized)
    endif()    
endif()

# MSVC: add configuration-specific flags using generator expressions to avoid conflicts such as /RTC1 vs /O2
if (MSVC)
    # Use /RTC1 only for Debug configuration; avoid it for Release where /O2 is used
    target_compile_options(metamod PRIVATE
        $<$<CONFIG:Debug>:/Zi;/Od;/RTC1>
        $<$<NOT:$<CONFIG:Debug>>:/O2>
    )
endif()

target_compile_options(metamod PRIVATE ${METAMOD_COMPILE_OPTIONS})
target_link_options(metamod PRIVATE ${METAMOD_LINK_OPTIONS})
set_target_properties(metamod PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(metamod PRIVATE library_suffix)

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
target_compile_definitions(metamod PRIVATE $<$<NOT:$<CONFIG:Debug>>:NDEBUG>)

# required for compliance with Xash3D FWGS library naming scheme
set_target_postfix(metamod)

# copy compiled metamod binaries & extra files to install directory
install(DIRECTORY "${PROJECT_SOURCE_DIR}/extra/" DESTINATION ".")
install(TARGETS metamod
        RUNTIME DESTINATION "."
		LIBRARY DESTINATION "."
		ARCHIVE DESTINATION "."
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
        GROUP_READ GROUP_EXECUTE
        WORLD_READ WORLD_EXECUTE)
