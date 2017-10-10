#include <interfaces.hpp>
#include <object.hpp>
#include <shared_data_t.hpp>
#include <string>


namespace ____native_str_obj_hpp_internal
{
    struct not_implemented : public std::exception
    {
        const char *what() const noexcept override
        {
            return "Not implemented";
        }
    };
}

class native_str_data : public rbb::shared_data_t
{
public:
    native_str_data(const std::string &str) :
        str{str} {}

    std::string str;
};

IFACES(native_str)
(
    rbb::iface::listable{
        [](rbb::object *, rbb::object &) -> rbb::object { throw ____native_str_obj_hpp_internal::not_implemented{}; },
        [](rbb::object *, rbb::object &) -> rbb::object { throw ____native_str_obj_hpp_internal::not_implemented{}; },
        [](rbb::object *thisptr) -> int {
            return thisptr->__value.data_as<native_str_data>()->str.size();
        },
        [](rbb::object *thisptr, int index) -> rbb::object {
            return rbb::number(thisptr->__value.data_as<native_str_data>()->str[index]);
        },
        [](rbb::object *, int, rbb::object) { throw ____native_str_obj_hpp_internal::not_implemented{}; }
    }
);

SELECT_RESPONSE_FOR(native_str)
