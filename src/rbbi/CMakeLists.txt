cmake_minimum_required(VERSION 2.8)
project(rbbi)
find_package(Boost REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

add_executable(rbbi rbbi.cpp)
target_link_libraries(rbbi rubberbandcore)

install(
    TARGETS rbbi RUNTIME
    DESTINATION bin)

