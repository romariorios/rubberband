set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(rbbpack SHARED pack.cpp)
add_library(rbbputch SHARED putch.cpp)
add_library(rbbrandom SHARED random.cpp)
add_library(rbbtostr SHARED tostr.cpp)

install(TARGETS rbbpack
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib)
