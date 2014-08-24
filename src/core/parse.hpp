#ifndef PARSE_HPP
#define PARSE_HPP

#include "object.hpp"

#include <string>

namespace rbb
{

std::string object_to_string(const object &obj);
object parse(const std::string &code);

}

#endif // PARSE_HPP