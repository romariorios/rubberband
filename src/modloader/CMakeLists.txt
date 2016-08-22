cmake_minimum_required(VERSION 2.8)
project(modloader)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(modloader_srcs
    sourcefile.cpp)
set(modloader_header
    sourcefile.hpp)

add_library(modloader STATIC ${modloader_srcs})

install(
    TARGETS modloader
    ARCHIVE DESTINATION lib)
install(
    FILES ${modloader_headers}
    DESTINATION include/rbb/modloader)
