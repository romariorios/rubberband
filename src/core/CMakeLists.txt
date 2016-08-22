cmake_minimum_required(VERSION 2.8)

project(rubberbandcore)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(deps_dir "${CMAKE_CURRENT_SOURCE_DIR}/deps")
set(lemon_src "${deps_dir}/lemon.c")
set(lemon_patched "${deps_dir}/LEMON_PATCHED")

if (EXISTS ${lemon_src})
    message(STATUS "${lemon_src} is present")
else()
    message(FATAL_ERROR "You have to download lemon.c from https://www.sqlite.org/src/raw/tool/lemon.c?name=09a96bed19955697a5e20c49ad863ec2005815a2 and copy it to ${deps_dir}")
endif()

if (EXISTS ${lemon_patched})
    message(STATUS "${lemon_src} was already patched")
else()
    if (UNIX)
        set(PATCH_COMMAND patch)
    endif()

    if (NOT PATCH_COMMAND)
        message(FATAL_ERROR "The patch command has not been defined")
    endif()

    foreach(patch_file IN ITEMS cppoutput noconflicterror)
        execute_process(
            COMMAND
            ${PATCH_COMMAND}
            ${lemon_src}
            "${CMAKE_CURRENT_SOURCE_DIR}/patches/lemon-${patch_file}.diff"
            ERROR_VARIABLE patch_status)

        if (NOT ${patch_status} EQUAL "0")
            message(FATAL_ERROR "Could not apply patch to lemon")
        endif()
    endforeach()

    execute_process(COMMAND cmake -E touch ${lemon_patched})
endif()

add_executable(lemon ${lemon_src})
file(
    COPY lemon_parser.y lempar.c
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
add_custom_command(
    OUTPUT lemon_parser.cpp
    COMMAND lemon -e -P lemon_parser.y
    COMMENT "Generating parser" VERBATIM)

set(rubberbandcore_sources
    block.cpp
    interfaces.cpp
    object.cpp
    parse.cpp
    symbol.cpp
    to_string.cpp
    tokenizer.cpp

    lemon_parser.cpp)

set(rubberbandcore_headers
    block.hpp
    error.hpp
    object.hpp
    parse.hpp
    shared_data_t.hpp
    symbol.hpp
    tokenizer.hpp)

add_library(rubberbandcore SHARED ${rubberbandcore_sources})

install(TARGETS rubberbandcore
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib)
install(FILES ${rubberbandcore_headers} DESTINATION include/rbb)
