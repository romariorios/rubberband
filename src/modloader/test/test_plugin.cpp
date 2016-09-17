#include <object.hpp>

using namespace rbb;
using namespace std;

extern "C" object rbb_loadobj()
{
    return table({symbol("a"), symbol("b")}, {number(10), number(20)});
}
