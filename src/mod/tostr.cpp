// Rubberband tostr module: turn objects into strings
// Copyright (c) 2017  Luiz Romário Santana Rios <luizromario at gmail dot com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#include <error.hpp>
#include <interfaces.hpp>
#include <object.hpp>
#include <parse.hpp>
#include <shared_data_t.hpp>

using namespace rbb;
using namespace std;

struct not_implemented : public std::exception
{
    const char *what() const noexcept override
    {
        return "Not implemented";
    }
};

class native_str_data : public shared_data_t
{
public:
    native_str_data(const string &str) :
        str{str} {}

    string str;
};

IFACES(native_str)
(
    iface::listable{
        [](object *, object &) -> object { throw not_implemented{}; },
        [](object *, object &) -> object { throw not_implemented{}; },
        [](object *thisptr) -> int {
            return thisptr->__value.data_as<native_str_data>()->str.size();
        },
        [](object *thisptr, int index) -> object {
            return number(thisptr->__value.data_as<native_str_data>()->str[index]);
        },
        [](object *, int, object) { throw not_implemented{}; }
    }
);

SELECT_RESPONSE_FOR(native_str)

class tostr_data : public shared_data_t
{
public:
    tostr_data(const object &mk_string) :
        mk_string{mk_string} {}

    object mk_string;
};

static string data_tostr(value_t::data_ptr &&data)
{
    return data->to_string();
}

static string obj_tostr(const object &obj)
{
    switch (obj.__value.type) {
    case value_t::empty_t:
        return "()";
    case value_t::integer_t:
        return std::to_string(obj.__value.integer());
    case value_t::floating_t:
        return std::to_string(obj.__value.floating());
    case value_t::symbol_t:
        return *obj.__value.symbol();
    case value_t::boolean_t:
        return obj.__value.boolean()? "?t" : "?f";
    case value_t::data_t:
        return data_tostr(move(obj.__value.data()));
    default:
        break;
    }

    return obj.to_string();
}

object tostr_send_msg_function(object *thisptr, object &msg)
{
    auto d = thisptr->__value.data_as<tostr_data>();

    return d->mk_string << object{
        value_t{new native_str_data{obj_tostr(msg)}},
        native_str_send_msg};
}

extern "C" object rbb_loadobj(base_master *master)
{
    auto str_imports = rbb::table();
    master->load("base") << str_imports << empty();
    master->load("string") << str_imports << empty();

    auto mk_string = str_imports << "mk_string";

    return object{
        value_t{new tostr_data{mk_string}},
        tostr_send_msg_function};
}
