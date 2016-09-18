// Rubberband script: Run rubberband code as scripts
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

#include <cstdio>
#include <fstream>
#include <error.hpp>
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
        loader{this, cfgfile_name}
    {}

    object load(const string &str)
    {
        return loader.load_module(str);
    }

    object custom_operation(const string &name, object &obj)
    {
        if (name == "inspect_object")
            puts(obj.to_string().c_str());

        return {};
    }

    modloader::sourcefile loader;
};

int main(int argc, char **argv)
{
    CmdLine cmd{"Rubbeband script"};
    UnlabeledValueArg<string> file_arg{
        "File", "Load program from script file",
        true, "", "Script filename"};
    cmd.add(file_arg);
    
    SwitchArg debug_arg{
        "d", "debug",
        "Enable debug mode (Show parse trace and resulting program)"};
    cmd.add(debug_arg);
    
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

    const auto debug_mode = debug_arg.getValue();
    if (debug_mode)
        LemonCParserTrace(stdout, " -- ");

    rbbs_master master{cfgfile_args.getValue()};
    master.loader.add_path_list(paths_args.getValue());

    auto main_context = table();
    master.loader.autoload(main_context);

    for (const auto &module : modules_args.getValue()) {
        auto mod = master.load(module);
        mod << main_context << empty();
    }

    auto result = master.loader.program_from_file(file_arg.getValue());
    if (debug_mode)
        puts(result.to_string().c_str());

    result << main_context << empty();

    return 0;
}

