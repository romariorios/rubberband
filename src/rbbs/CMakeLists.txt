cmake_minimum_required(VERSION 2.8)

project(rbbs)

add_executable(rbbs main.cpp)
target_link_libraries(rbbs rubberbandcore)

install(
    TARGETS rbbs RUNTIME
    DESTINATION bin)

