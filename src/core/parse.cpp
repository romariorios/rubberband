#include "parse.hpp"

#include "block.hpp"
#include "object_private.hpp"
#include "symbol.hpp"

#include <typeinfo>

#include <cstdio>

using namespace rbb;

std::string symbol_node_to_string(symbol_node *sym)
{
    if (!sym->up)
        return std::string{};
    
    return symbol_node_to_string(sym->up) + std::string{sym->ch};
}

std::string array_data_to_string(array_data *data)
{
    std::string result{"|["};
    
    for (int i = 0; i < data->size; ++i) {
        result += object_to_string(data->arr[i]);
        
        if (i + 1 < data->size)
            result += ", ";
    }
    
    result += "]";
    
    return result;
}

std::string table_data_to_string(table_data *data)
{
    std::string result{":["};
    
    for (auto &entry : data->objtree) {
        result += symbol_node_to_string(entry.first) + " -> " + object_to_string(entry.second);
        result += ", ";
    }
    
    result.erase(result.size() - 2);
    result += "]";
    
    return result;
}

std::string data_to_string(shared_data_t *data)
{
    if (typeid(*data) == typeid(array_data))
        return array_data_to_string(static_cast<array_data *>(data));
    else if (typeid(*data) == typeid(table_data))
        return table_data_to_string(static_cast<table_data *>(data));
    // TODO block
    else
        return "[unknown data]";
}

std::string rbb::object_to_string(const object& obj)
{
    const auto type = obj.__value.type;
    
    switch (type) {
    case value_t::empty_t:
        return "[]";
    case value_t::integer_t:
        return std::to_string(obj.__value.integer);
    case value_t::floating_t:
        return std::to_string(obj.__value.floating);
    case value_t::symbol_t:
        return symbol_node_to_string(obj.__value.symbol);
    case value_t::boolean_t:
        return obj.__value.boolean? "[T]" : "[F]";
    case value_t::data_t:
        return data_to_string(obj.__value.data);
    default:
        return "[unknown]";
    }
}

object rbb::parse(const std::string &code)
{
    literal::block bl;
    
    return bl.eval();
}