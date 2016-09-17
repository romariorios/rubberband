#include "../native_linux.hpp"

#include <iostream>

using namespace rbb;
using namespace std;

int main(int argc, char **argv)
{
    modloader::native_linux loader;
    auto obj = loader.load_module("test_plugin");

    cout << obj.to_string() << "\n";

    return 0;
}
