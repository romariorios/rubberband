cmake_minimum_required(VERSION 2.8)

project(rubberbandcore)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(lemon lemon.c)
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
