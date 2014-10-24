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

int main(int argc, char **argv)
{
    if (argc < 2 && argc > 3) {
        puts("No rubberband file defined");
        return 1;
    }

    ifstream file{argv[1]};
    if (!file) {
        puts("Could not open file");
        return 1;
    }

    string program;

    while (!file.eof()) {
        string tmp;
        getline(file, tmp);
        program += tmp;
        program += "\n";
    }

    object print;
    print.__send_msg = __print;

    auto debug_mode = argc == 3 && string{argv[2]} == "-d";
    if (debug_mode)
        LemonCParserTrace(stdout, " -- ");

    auto result = parser{program}.parse();
    if (result << symbol("error") == symbol("syntax")) {
        puts(result.to_string().c_str());
        return 1;
    }

    if (debug_mode)
        puts(result.to_string().c_str());

    result << table({symbol("print")}, {print}) << empty();

    return 0;
}

