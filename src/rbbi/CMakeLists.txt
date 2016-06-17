cmake_minimum_required(VERSION 2.8)
project(rbbi)
find_package(Boost REQUIRED)
find_path(readline_DIR readline)
find_library(readline_LIB readline)

if (readline_DIR-NOTFOUND)
    message(FATAL_ERROR "Could not find readline includes")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

add_executable(rbbi rbbi.cpp)
target_link_libraries(rbbi rubberbandcore ${readline_LIB})

install(
    TARGETS rbbi RUNTIME
    DESTINATION bin)
