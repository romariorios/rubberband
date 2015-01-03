#include <cstdio>
#include <fstream>
#include <parse.hpp>
#include <string>
#include <tclap/CmdLine.h>

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

class rbbs_master
{
public:
    inline object set_context(const object &obj)
    {
        _context = obj;
    }
    
    inline object load(const string &str)
    {
        return program_from_file(str + ".rbb") << _context << object{};
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
    
    cmd.parse(argc, argv);

    object print;
    print.__send_msg = __print;

    auto debug_mode = debug_arg.getValue();
    if (debug_mode)
        LemonCParserTrace(stdout, " -- ");

    auto result = program_from_file(file_arg.getValue());
    if (debug_mode)
        puts(result.to_string().c_str());

    result << table({symbol("print")}, {print}) << empty();

    return 0;
}

