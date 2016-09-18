// Rubberband modloader: Load external Rubberband modules
// Copyright (c) 2016  Luiz Romário Santana Rios <luizromario at gmail dot com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#include "native_linux.hpp"

#include <dlfcn.h>
#include <fstream>

using namespace rbb;
using namespace rbb::modloader;
using namespace std;

using loadobj_fun = object(*)();

native_linux::native_linux() :
    base{{}}
{}

class dl_handle
{
public:
    dl_handle(void *handle) :
        _handle{handle}
    {
        if (!_handle)
            throw dlopen_error{};

        dlerror();  // not sure if this is necessary
    }

    ~dl_handle()
    {
        dlclose(_handle);
    }

    operator void *() const
    {
        return _handle;
    }

private:
    void *_handle;
};

object native_linux::load_module(const string &modname) const
{
    auto file_with_path = "./lib" + modname + ".so";

    for (const auto &path : module_paths) {
        if (ifstream{file_with_path}.good())
            break;

        file_with_path = path + "/lib" + modname + ".so";
    }

    dl_handle module_handle{dlopen(file_with_path.c_str(), RTLD_LAZY)};

    auto rbb_loadobj = reinterpret_cast<loadobj_fun>(dlsym(module_handle, "rbb_loadobj"));
    const auto dl_error_str = dlerror();
    if (dl_error_str)
        throw dlsym_error{dl_error_str};

    return rbb_loadobj();
}

dlopen_error::dlopen_error() :
    load_error{dlerror()}
{}

dlsym_error::dlsym_error(char *error) :
    load_error{error}
{}
