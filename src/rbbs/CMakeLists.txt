cmake_minimum_required(VERSION 2.8)

project(rbbs)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/rbbs/cmake/Modules/")

find_package(Tclap REQUIRED)

add_executable(rbbs main.cpp)
target_link_libraries(rbbs rubberbandcore)

install(
    TARGETS rbbs RUNTIME
    DESTINATION bin)

