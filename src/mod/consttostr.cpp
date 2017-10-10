// Rubberband consttostr module: turn primitive constants into strings
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
#include <mod/native_str_obj.hpp>
#include <object.hpp>
#include <parse.hpp>
#include <shared_data_t.hpp>

using namespace rbb;
using namespace std;

static string consttostr(object &consttostr, const object &obj)
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
    default:
        break;
    }

    throw message_not_recognized_error{consttostr, obj, "Not a primitive constant"};
}

object consttostr_send_msg_function(object *thisptr, object &msg)
{
    return native_str_to_obj(consttostr(*thisptr, msg));
}

extern "C" object rbb_loadobj(base_master *)
{
    return object{
        value_t{value_t::no_data_t},
        consttostr_send_msg_function};
}
