#include <cstdio>
#include <fstream>
#include <parse.hpp>
#include <string>
#include <tclap/CmdLine.h>
#include <vector>

using namespace rbb;
using namespace std;
using namespace TCLAP;

extern void LemonCParserTrace(FILE *stream, char *zPrefix);

object __print(object *, const object &msg)
{
    puts(msg.to_string().c_str());

    return empty();
}

object program_from_file(const string &filename);

vector<string> module_paths;

class rbbs_master
{
public:
    inline object set_context(const object &obj)
    {
        _context = obj;
    }
    
    object load(const string &str)
    {
        auto file_with_path = str + ".rbb";
        
        for (auto path : module_paths) {
            if (ifstream{file_with_path}.good())
                break;
            
            file_with_path = path + "/" + str + ".rbb";
        }
        
        return program_from_file(file_with_path) << _context << object{};
    }
    
private:
    object _context;
};

class could_not_open_file : public exception
{
public:
    could_not_open_file(const string &f) : _f{f} {}
    inline const char *what() const noexcept
    {
        return string{"Could not open file \"" + _f + "\""}.c_str();
    }
    
private:
    string _f;
};

object program_from_file(const string &filename)
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
    
    return parse<rbbs_master>(program);
}

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
    
    cmd.parse(argc, argv);

    object print;
    print.__send_msg = __print;

    auto debug_mode = debug_arg.getValue();
    if (debug_mode)
        LemonCParserTrace(stdout, " -- ");

    auto result = program_from_file(file_arg.getValue());
    if (debug_mode)
        puts(result.to_string().c_str());
    
    module_paths = paths_args.getValue();

    result << table({symbol("print")}, {print}) << empty();

    return 0;
}

