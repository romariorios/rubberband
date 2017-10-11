// Rubberband putch module: standard output
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

#include "mod_common.hpp"

#include <error.hpp>
#include <object.hpp>
#include <common_syms.hpp>

#include <cstdio>

using namespace rbb;

object putch_send_msg(object *, object &msg)
{
    if (msg << SY_DLTQM << SY_I_NUM == boolean(false))
        throw message_not_recognized_error{
            symbol("rbbputch"), msg,
            "Number or character expected"};

    putchar(msg.__value.integer());
    return {};
}

namespace rbb
{
    class base_master;
}

extern "C" object rbb_loadobj(base_master *)
{
    return mk_nativemod(putch_send_msg);
}
