cmake_minimum_required(VERSION 2.8)

project(rubberbandcore)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(rubberbandcore_sources
    block.cpp
    object.cpp
    parse.cpp
    symbol.cpp
    to_string.cpp
    tokenizer.cpp

    lemon_parser.cpp)

set(rubberbandcore_headers
    block.hpp
    object.hpp
    parse.hpp
    symbol.hpp
    tokenizer.hpp)

add_library(rubberbandcore SHARED ${rubberbandcore_sources})

install(TARGETS rubberbandcore
    LIBRARY DESTINATION lib)
install(FILES ${rubberbandcore_headers} DESTINATION include)
