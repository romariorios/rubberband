#include <object.hpp>

using namespace rbb;
using namespace std;

object thing(object *, object &)
{
    return symbol("sup");
}

extern "C" object rbb_loadobj()
{
    return object::create_object(value_t::no_data_t, thing);
}
