cmake_minimum_required(VERSION 2.8)
project(modloader)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/deps/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(modloader_srcs
    multi.cpp
    native_linux.cpp
    sourcefile.cpp
    base.cpp)
set(modloader_headers
    multi.hpp
    native_linux.hpp
    sourcefile.hpp
    base.hpp)

add_library(modloader STATIC ${modloader_srcs})
target_link_libraries(modloader dl rubberbandcore)

install(
    TARGETS modloader
    ARCHIVE DESTINATION lib)
install(
    FILES ${modloader_headers}
    DESTINATION include/rbb/modloader)

if (MODLOADER_PLUGIN_TESTS)
    add_subdirectory(test)
endif()
