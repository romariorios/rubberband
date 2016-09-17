add_library(test_plugin SHARED test_plugin.cpp)

add_executable(native_linux_plugin_test native_linux_plugin_test.cpp)
target_link_libraries(
    native_linux_plugin_test
    modloader)
