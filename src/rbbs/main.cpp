// Rubberband script: Run rubberband code as scripts
// Copyright (c) 2016--2017  Luiz Romário Santana Rios <luizromario at gmail dot com>
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

#include <cstdio>
#include <fstream>
#include <error.hpp>
#include <modloader/multi.hpp>
#include <modloader/native_linux.hpp>
#include <modloader/sourcefile.hpp>
#include <tclap/CmdLine.h>

using namespace rbb;
using namespace std;
using namespace TCLAP;

extern void LemonCParserTrace(FILE *stream, char *zPrefix);

class rbbs_master : public base_master
{
public:
    rbbs_master(const std::string &cfgfile_name) :
        _loader{this, cfgfile_name},
        file_loader{_loader.add_loader<modloader::sourcefile>()}
    {
        _loader.add_loader<modloader::native_linux>();
    }

    object load(const string &str)
    {
        return _loader.load_module(str);
    }

    object custom_operation(const string &name, object &obj)
    {
        if (name == "inspect_object")
            puts(obj.to_string().c_str());

        return {};
    }

    auto &loader()
    {
        return _loader;
    }

private:
    modloader::multi _loader;

public:
    // Needs to be after _loader
    modloader::sourcefile &file_loader;
};

int main(int argc, char **argv)
{
    CmdLine cmd{"Rubbeband script"};
    UnlabeledValueArg<string> file_arg{
        "File", "Load program from script file",
        true, "", "Script filename"};
    cmd.add(file_arg);

#ifndef NDEBUG
    SwitchArg debug_arg{
        "d", "debug",
        "Enable debug mode (Show parse trace and resulting program)"};
    cmd.add(debug_arg);

    const auto debug_mode = debug_arg.getValue();
    if (debug_mode)
        LemonCParserTrace(stdout, " -- ");
#else
    // LemonCParserTrace is not available in Release mode, so debugging is unavailable in Release
    const auto debug_mode = false;
#endif
    
    MultiArg<string> paths_args{
        "p", "path", "Module lookup path",
        false, "Absolute or relative path"};
    cmd.add(paths_args);
    
    MultiArg<string> modules_args{
        "m", "module", "Load module",
        false, "Module name"
    };
    cmd.add(modules_args);

    ValueArg<string> cfgfile_args{
        "c", "cfgfile", "Set config file path (default: .cfg.json)",
        false, ".cfg.json", "File path"
    };
    cmd.add(cfgfile_args);
    
    cmd.parse(argc, argv);

    rbbs_master master{cfgfile_args.getValue()};
    master.loader().add_path_list(paths_args.getValue());

    auto main_context = table();
    master.loader().autoload(main_context);

    for (const auto &module : modules_args.getValue()) {
        auto mod = master.load(module);
        mod << main_context << empty();
    }

    auto result = master.file_loader.program_from_file(file_arg.getValue());
    if (debug_mode)
        puts(result.to_string().c_str());

    result << main_context << empty();

    return 0;
}

