// Rubberband language
// Copyright (C) 2013  Luiz Romário Santana Rios <luizromario at gmail dot com>
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

namespace rbb
{

class expr
{
public:
    expr();
    virtual ~expr() {}
    
    virtual void set_symbol_table(const object &) {}
    virtual void set_arg(const object &) {}
    virtual object eval() = 0;
};

class block_statement_private;
class block_statement : public expr
{
public:
    block_statement();
    ~block_statement();
    void set_symbol_table(const object &sym_table);
    void set_arg(const object &arg);
    void add_expr(expr *e);
    object eval();
    
private:
    block_statement_private *_p;
};

namespace literal
{
    class boolean : public expr
    {
    public:
        inline boolean(bool val) : _val(val) {}
        inline object eval() { return rbb::boolean(_val); }
        
    private:
        bool _val;
    };
    
    class empty : public expr
    {
    public:
        inline empty() {}
        inline object eval() { return rbb::empty(); }
    };
    
    class number : public expr
    {
    public:
        inline number(double val) : _val(val) {}
        inline object eval() { return rbb::number(_val); }
        
    private:
        double _val;
    };
    
    class symbol : public expr
    {
    public:
        inline symbol(const char *str) : _sym(rbb::symbol(str)) {}
        inline symbol(char *str) : _sym(rbb::symbol(str)) {}
        inline object eval() { return _sym; }
        
    private:
        object _sym;
    };
    
    class list : public expr
    {
    public:
        list(rbb::expr *obj_array[], int size);
        ~list();
        void set_symbol_table(const object &sym_table);
        void set_arg(const object &arg);
        object eval();
        
    private:
        expr **_obj_array;
        int _size;
    };
    
    class generic_object : public expr
    {
    public:
        generic_object(rbb::expr *symbol_array[], rbb::expr *obj_array[], int size);
        ~generic_object();
        void set_symbol_table(const object &sym_table);
        void set_arg(const object &arg);
        object eval();
        
    private:
        expr **_symbol_array;
        expr **_obj_array;
        int _size;
    };
    
    class symbol_table : public expr
    {
    public:
        inline symbol_table() {}
        inline object eval() { return _sym_table; }
        
    private:
        object _sym_table;
    };
    
    class block_arg : public expr
    {
    public:
        inline block_arg() {}
        inline object eval() { return _arg; }
        
    private:
        object _arg;
    };
    
    class block_private;
    class block : public expr // blocks don't depend on an external context
    {
    public:
        block();
        ~block();
        void add_statement(block_statement *stm);
        void set_return_expression(expr *expr);
        void set_block_symbol_table(const object &sym_table);
        void set_block_arg(const object &arg);
        object eval();
        object run();
        
    private:
        block_private *_p;
    };
}

}

#endif
