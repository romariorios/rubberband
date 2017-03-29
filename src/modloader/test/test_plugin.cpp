#include <object.hpp>

using namespace rbb;
using namespace std;

object thing(object *, object &)
{
    return symbol("sup");
}

extern "C" object rbb_loadobj()
{
    return object{value_t{value_t::no_data_t}, thing};
}
