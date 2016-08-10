cmake_minimum_required(VERSION 2.8)

project(rubberbandcore)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if (NOT LEMON_URL)
    set(LEMON_URL
        "https://www.sqlite.org/src/raw/tool/lemon.c?name=09a96bed19955697a5e20c49ad863ec2005815a2")
endif()

set(lemon_src
    "${CMAKE_CURRENT_BINARY_DIR}/lemon.c")
file(
    DOWNLOAD 
    ${LEMON_URL}
    ${lemon_src}
    SHOW_PROGRESS
    STATUS download_result
    EXPECTED_HASH SHA256=0c0d12587d548ad348b87fb5520d7e4e36f7c22fd8b6695c4b2e6710de96e90f)

list(GET download_result "0" download_status)
if (NOT download_status EQUAL "0")
    message(FATAL_ERROR "Could not download lemon")
endif()

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
