#include "parse.hpp"

#include "block.hpp"

using namespace rbb;

object rbb::parse(const std::string &code)
{
    literal::block bl;
    
    return bl.eval();
}