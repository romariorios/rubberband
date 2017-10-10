set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(modules pack putch random consttostr)

foreach (m IN ITEMS ${modules})
    add_library(rbb${m} SHARED ${m}.cpp)
    list(APPEND mod_targets rbb${m})
endforeach()

install(TARGETS ${mod_targets}
    ARCHIVE DESTINATION lib/rbb
    LIBRARY DESTINATION lib/rbb)

set(mod_headers native_str_obj.hpp)

install(FILES ${mod_headers}
    DESTINATION include/rbb/mod)
