#include <cstdio>
#include <fstream>
#include <parse.hpp>
#include <string>

using namespace rbb;
using namespace std;

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
    if (argc < 2 || argc > 3) {
        puts("No rubberband file defined");
        return 1;
    }

    object print;
    print.__send_msg = __print;

    auto debug_mode = argc == 3 && string{argv[2]} == "-d";
    if (debug_mode)
        LemonCParserTrace(stdout, " -- ");

    auto result = program_from_file(argv[1]);
    if (debug_mode)
        puts(result.to_string().c_str());

    result << table({symbol("print")}, {print}) << empty();

    return 0;
}

