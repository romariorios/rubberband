cmake_minimum_required(VERSION 2.8)

project(rbbs)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/rbbs/cmake/Modules/")

find_package(Tclap REQUIRED)

add_executable(rbbs main.cpp)
target_link_libraries(rbbs rubberbandcore modloader)

install(
    TARGETS rbbs RUNTIME
    DESTINATION bin)

