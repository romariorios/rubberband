#include "block.hpp"
#include "block_private.hpp"
#include "object.hpp"
#include "object_private.hpp"
#include "symbol.hpp"

#include <typeinfo>

#include <cstdio>

using namespace rbb;

std::string rbb::symbol_node::to_string() const
{
    if (!up)
        return std::string{};
    
    return up->to_string() + std::string{ch};
}

std::string rbb::array_data::to_string() const
{
    std::string result{"|["};
    
    for (int i = 0; i < size; ++i) {
        result += arr[i].to_string();
        
        if (i + 1 < size)
            result += ", ";
    }
    
    result += "]";
    
    return result;
}

std::string rbb::table_data::to_string() const
{
    std::string result{":["};
    
    for (auto &entry : objtree) {
        result += entry.first->to_string() + " -> " + entry.second.to_string();
        result += ", ";
    }
    
    result.erase(result.size() - 2);
    result += "]";
    
    return result;
}

std::string rbb::block_data::to_string() const
{
    return block_l->to_string();
}


std::string rbb::object::to_string() const
{
    const auto type = __value.type;
    
    switch (type) {
    case value_t::empty_t:
        return "[]";
    case value_t::integer_t:
        return std::to_string(__value.integer);
    case value_t::floating_t:
        return std::to_string(__value.floating);
    case value_t::symbol_t:
        return __value.symbol->to_string();
    case value_t::boolean_t:
        return __value.boolean? "[T]" : "[F]";
    case value_t::data_t:
        return __value.data->to_string();
    default:
        return "[unknown]";
    }
}