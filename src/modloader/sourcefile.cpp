#include "sourcefile.hpp"

#include <fstream>

using namespace rbb;
using namespace rbb::modloader;
using namespace std;

sourcefile::sourcefile(base_master *master) :
    _master{*master}
{}

object sourcefile::load_module(const string &modname) const
{
    auto file_with_path = modname + ".rbb";

    for (auto path : _module_paths) {
        if (ifstream{file_with_path}.good())
            break;

        file_with_path = path + "/" + modname + ".rbb";
    }

    return program_from_file(file_with_path);
}

object sourcefile::program_from_file(const string &filename) const
{
    ifstream file{filename};
    if (!file) {
        throw could_not_open_file{filename};
    }

    string program;

    while (!file.eof()) {
        string tmp;
        getline(file, tmp);
        program += tmp;
        program += "\n";
    }

    try {
        return _master.parse(program);
    } catch (const syntax_error &e) {
        throw sourcefile_syntax_error{e, filename};
    }
}

void sourcefile::add_path_list(const vector<string> &paths)
{
    _module_paths.insert(_module_paths.cbegin(), paths.begin(), paths.end());
}
