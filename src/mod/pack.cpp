#include <object.hpp>
#include <shared_data_t.hpp>
#include <error.hpp>

using namespace rbb;

class pack_data : public shared_data_t
{
public:
    explicit pack_data(size_t size) :
        _pack{new uint8_t[size]},
        _size{size}
    {}

    // Forbid implicit copies
    pack_data(const pack_data &) = delete;
    pack_data &operator=(const pack_data &) = delete;

    pack_data(pack_data &&other) :
        _pack{other._pack},
        _size{other._size}
    {
        other._pack = nullptr;
        other._size = 0;
    }

    pack_data &operator=(pack_data &&) = default;

    ~pack_data()
    {
        delete _pack;
    }

    // Allow explicit copies if necessary
    pack_data copy() const
    {
        pack_data ret{_size};
        for (auto i = 0; i < _size; ++i)
            ret.pack_array[i] = pack_array[i];

        return ret;
    }

    uint8_t *pack_array;
    const size_t _size;
};

object pack_data_send_msg(object *thisptr, object &msg)
{
    // TODO implement
    return {};
}

object pack(object *, object &msg)
{
    auto throw_msg_error = [&msg](const char *details)
    {
        throw message_not_recognized_error{symbol("pack"), msg, details};
    };

    if (msg << symbol("<<?") << symbol("[|]") == boolean(false))  // If msg isn't an array
        throw_msg_error("array expected");

    const auto size = number_to_double(msg << symbol("*"));
    auto pack = new pack_data{size};
    for (auto i = 0; i < size; ++i) {
        auto el = msg << number(i);
        if (el << symbol("<<?") << symbol("[0]") == boolean(false))  // If element isn't a number
            throw_msg_error("all elements should be numbers");
        if (
            el << symbol(">=") << number(0) == boolean(false) ||
            el << symbol("<") << number(256) == boolean(false))
            throw_msg_error("all numbers should be within the [0; 255] range");

        const auto num = static_cast<uint8_t>(number_to_double(el));
        pack->pack_array[i] = num;
    }

    return object::create_data_object(pack, pack_data_send_msg);
}
