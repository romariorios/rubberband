// Rubberband pack module: create bytearrays from lists
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
#include <shared_data_t.hpp>

using namespace rbb;
using namespace std;

class pack_data : public shared_data_t
{
public:
    explicit pack_data(size_t size) :
        pack_array{new uint8_t[size]},
        size {size}
    {}

    // Forbid implicit copies
    pack_data(const pack_data &) = delete;
    pack_data &operator=(const pack_data &) = delete;

    pack_data(pack_data &&other) :
        pack_array{other.pack_array},
        size {other.size}
    {
        other.pack_array = nullptr;
    }

    pack_data &operator=(pack_data &&) = default;

    ~pack_data()
    {
        delete[] pack_array;
    }

    // Allow explicit copies if necessary
    pack_data copy() const
    {
        pack_data ret{size};
        for (auto i = 0; i < size; ++i)
            ret.pack_array[i] = pack_array[i];

        return ret;
    }

    bool compare(const pack_data &other) const
    {
        if (size != other.size)
            return false;

        for (auto i = 0; i < size; ++i)
            if (pack_array[i] != other.pack_array[i])
                return false;

        return true;
    }

    pack_data concat(const pack_data &other) const
    {
        const auto new_size = size + other.size;
        pack_data new_pack{new_size};

        for (auto i = 0; i < size; ++i)
            new_pack.pack_array[i] = pack_array[i];

        for (auto i = size, j = 0ul; j < other.size; ++i, ++j)
            new_pack.pack_array[i] = other.pack_array[j];

        return new_pack;
    }

    pack_data slice(size_t pos, size_t size) const
    {
        pack_data new_pack{size};

        for (auto i = 0ul, j = pos; i < size; ++i, ++j)
            new_pack.pack_array[i] = pack_array[j];

        return new_pack;
    }

    uint8_t *pack_array;
    const size_t size;
};

static shared_ptr<pack_data> to_data(object &obj)
{
    if (obj.__value.type != value_t::data_t)
        return nullptr;

    return obj.__value.try_data_as<pack_data>();
}

static object inner_obj(object &obj)
{
    if (obj.__value.type != value_t::data_t)
        return {};

    auto d = obj.__value.try_data_as<object_data>();
    if (!d)
        return {};

    return d->obj;
}

static object pack_eq_send_msg(object *thisptr, object &msg)
{
    auto obj = inner_obj(*thisptr);
    auto d = to_data(obj);
    auto msg_d = to_data(msg);

    if (!msg_d)
        return boolean(false);

    return boolean(d->compare(*msg_d));
}

static object pack_ne_send_msg(object *thisptr, object &msg)
{
    return pack_eq_send_msg(thisptr, msg) << "><";  // eq negated
}

static object pack_data_send_msg(object *, object &);

static object pack_concat_send_msg(object *thisptr, object &msg)
{
    auto obj = inner_obj(*thisptr);
    auto d = to_data(obj);
    auto msg_d = to_data(msg);

    if (!msg_d)
        throw message_not_recognized_error{*thisptr, msg, "Expected pack"};

    return object{value_t{
        new pack_data{d->concat(*msg_d)}}, pack_data_send_msg};
}

static object pack_slice_send_msg(object *thisptr, object &msg)
{
    if (msg << "<<?" << "listable" != boolean(true))
        throw message_not_recognized_error{*thisptr, msg, "Expected array"};

    if (msg << "len" << ">=" << 2 != boolean(true))
        throw message_not_recognized_error{*thisptr, msg, "Too few arguments"};

    auto obj = inner_obj(*thisptr);
    auto d = to_data(obj);
    return object{value_t{
        new pack_data{
            d->slice(
                number_to_double(msg << 0),
                number_to_double(msg << 1))}},
        pack_data_send_msg};
}

static int pack_get_size(object *thisptr)
{
    return to_data(*thisptr)->size;
}

static object pack_get_element(object *thisptr, int index)
{
    return number(to_data(*thisptr)->pack_array[index]);
}

static void pack_set_element(object *thisptr, int index, object el)
{
    if (el << "<<?" << "numeric" != boolean(true))
        throw message_not_recognized_error{*thisptr, el, "Number expected"};

    auto d = to_data(*thisptr);

    if (index < 0 || index >= d->size)
        throw message_not_recognized_error{*thisptr, el, "Index out of range"};

    auto num = number_to_double(el);
    if (num < 0 || num >= 256)
        throw message_not_recognized_error{*thisptr, el, "Number out of [0; 256) range"};

    d->pack_array[index] = static_cast<uint8_t>(num);
}

IFACES(pack_data)
(
    iface::comparable{
        pack_eq_send_msg,
        pack_ne_send_msg
    },
    iface::listable{
        pack_concat_send_msg,
        pack_slice_send_msg,
        pack_get_size,
        pack_get_element,
        pack_set_element
    }
);

SELECT_RESPONSE_FOR(pack_data)

object pack(object *, object &msg)
{
    auto throw_msg_error = [&msg](const char *details)
    {
        throw message_not_recognized_error{symbol("pack"), msg, details};
    };

    if (msg << symbol("<<?") << SY_I_ARR == boolean(false))  // If msg isn't an array
        throw_msg_error("array expected");

    const auto size = number_to_double(msg << symbol("len"));
    auto pack = new pack_data{static_cast<size_t>(size)};
    for (auto i = 0; i < size; ++i) {
        auto el = msg << number(i);
        if (el << symbol("<<?") << symbol("numeric") == boolean(false))  // If element isn't a number
            throw_msg_error("all elements should be numbers");
        if (
            el << symbol(">=") << number(0) == boolean(false) ||
            el << symbol("<") << number(256) == boolean(false))
            throw_msg_error("all numbers should be within the [0; 256) range");

        const auto num = static_cast<uint8_t>(number_to_double(el));
        pack->pack_array[i] = num;
    }

    return object{value_t{pack}, pack_data_send_msg};
}

extern "C" object rbb_loadobj()
{
    return object{value_t{value_t::no_data_t}, pack};
}
