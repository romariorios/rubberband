#ifndef MOD_COMMON_HPP
#define MOD_COMMON_HPP

#include <object.hpp>
#include <common_syms.hpp>

namespace rbb
{

auto mk_nativemod(send_msg_function msg_send)
{
    struct nativemod_data : public shared_data_t
    {
        nativemod_data(send_msg_function msg_send) :
            _msg_send{msg_send} {}

        send_msg_function _msg_send;
    };

    return object{
        value_t{new nativemod_data{msg_send}},
        [](object *thisptr, auto &msg)
        {
            if (msg == SY_DLT)
                return object{
                    value_t{value_t::no_data_t},
                    [](auto, auto &msg)
                    {
                        return boolean(msg == SY_DLT || msg == SY_DLTQM);
                    }
                };

            if (msg == SY_DLTQM)
                return object{
                    value_t{value_t::no_data_t},
                    [](auto, auto &msg)
                    {
                        return boolean(msg == symbol("nativemod"));
                    }
                };

            // TODO allow the construction of a module with data
            return thisptr->__value.data_as<nativemod_data>()->_msg_send(nullptr, msg);
        }
    };
}

}

#endif
