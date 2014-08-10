// Rubberband language
// Copyright (C) 2013, 2014  Luiz Romário Santana Rios <luizromario at gmail dot com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "object.hpp"

#include <memory>

namespace rbb
{

namespace literal
{
    class block;
}

class expr
{
public:
    expr() {}
    virtual ~expr() {}

    virtual object eval(literal::block *parent_block) = 0;
};

class block_statement_private;
class block_statement : public expr
{
public:
    block_statement();
    void add_expr(expr *e);
    object eval(literal::block *parent_block);

    std::unique_ptr<block_statement_private> _p;
};

namespace literal
{
    class boolean : public expr
    {
    public:
        inline boolean(bool val) : _val(val) {}
        inline object eval(block *) { return rbb::boolean(_val); }

    private:
        bool _val;
    };

    class empty : public expr
    {
    public:
        inline empty() {}
        inline object eval(block *) { return rbb::empty(); }
    };

    class number : public expr
    {
    public:
        inline number(double val) : _val(val) {}
        inline object eval(block *) { return rbb::number(_val); }

    private:
        double _val;
    };

    class symbol : public expr
    {
    public:
        inline symbol(const std::string &str) : _sym(rbb::symbol(str)) {}
        inline object eval(block *) { return _sym; }

    private:
        object _sym;
    };

    class array : public expr
    {
    public:
        array(rbb::expr *obj_array[], int size);
        ~array();
        object eval(block *parent_block);

    private:
        expr **_obj_array;
        int _size;
    };

    class table : public expr
    {
    public:
        table(rbb::expr *symbol_array[], rbb::expr *obj_array[], int size);
        ~table();
        object eval(block *parent_block);

    private:
        expr **_symbol_array;
        expr **_obj_array;
        int _size;
    };

    class self_ref : public expr
    {
    public:
        inline self_ref() {}
        object eval(block *parent_block);
    };

    class context : public expr
    {
    public:
        inline context() {}
        object eval(block *parent_block);
    };

    class message : public expr
    {
    public:
        inline message() {}
        object eval(block *parent_block);
    };

    class block_private;
    class block : public expr
    {
    public:
        block();
        block(const block &other);
        void add_statement(block_statement *stm);
        void set_return_expression(expr *expr);
        void set_block_context(const object &context);
        void set_block_message(const object &msg);
        object eval(block * = nullptr); // blocks don't depend on their parent block
        object run();

        std::shared_ptr<block_private> _p;
        object _context;
        object _message;
    };
}

}

#endif
