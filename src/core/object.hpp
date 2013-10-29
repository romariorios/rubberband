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

#ifndef OBJECT_HPP
#define OBJECT_HPP

namespace rbb
{

class symbol_node;

class shared_data_t;

struct value_t
{
    enum
    {
        no_data_t  = 0x10,
        empty_t    = 0x11,
        integer_t  = 0x12,
        floating_t = 0x13,
        boolean_t  = 0x14,
        symbol_t   = 0x15,
        data_t     = 0x20
    } type;
    
    union
    {
        long integer;
        double floating;
        bool boolean;
        symbol_node *symbol;
        shared_data_t *data;
    };
};

// Object
class object
{
public:
    object();
    object(const object &other);
    ~object();
    
    bool operator==(const object &other) const;
    inline bool operator!=(const object &other) const { return !(other == *this); }
    
    object send_msg(const object &msg);
    
    value_t __value;
    object (*__send_msg)(object *, const object &);
    
private:
    void destroy();
    void ref();
    int deref();
};

object empty();

object number(double val);
double number_to_double(const object &num);

object symbol(char *val);
object symbol(const char *val);

object boolean(bool val);

// NEVER assign pointers directly to other objects

object list(const object obj_array[], int size);

object generic_object(const object symbol_array[], const object obj_array[], int size);

class expr
{
public:
    expr();
    virtual ~expr() {}
    
    virtual void set_symbol_table(const object &) {}
    virtual void set_arg(const object &) {}
    virtual object eval() = 0;
};

class context_dependent_expr : public expr
{
public:
    context_dependent_expr() {}
    
    virtual void set_symbol_table(const object &sym_table) { _sym_table = sym_table; }
    virtual void set_arg(const object &arg) { _arg = arg; }
    
    object _sym_table;
    object _arg;
};

class block_statement_private;
class block_statement : public context_dependent_expr
{
public:
    block_statement();
    ~block_statement();
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
        empty() {}
        object eval() { return rbb::empty(); }
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
        symbol(const char *str);
        object eval();
        
    private:
        symbol_node *_sym;
    };
    
    class list : public context_dependent_expr
    {
    public:
        list(const expr *obj_array[], int size);
        object eval();
        
    private:
        const expr *_obj_array;
        int _size;
    };
    
    class generic_object : public context_dependent_expr
    {
    public:
        generic_object(const expr *symbol_array[], const expr *obj_array[], int size);
        object eval();
        
    private:
        const expr **_symbol_array;
        const expr **_obj_array;
        int _size;
    };
    
    class symbol_table : public context_dependent_expr
    {
    public:
        inline symbol_table() {}
        inline object eval() { return _sym_table; }
    };
    
    class block_arg : public context_dependent_expr
    {
    public:
        inline block_arg() {}
        inline object eval() { return _arg; }
    };
    
    class block_private;
    class block : public expr // blocks don't depend on an external context
    {
    public:
        block();
        ~block();
        void add_statement(block_statement *stm);
        void set_return_expression(expr *expr);
        void set_symbol_table(const object &sym_table);
        void set_arg(const object &arg);
        object eval();
        
    private:
        block_private *_p;
    };
}

}

#endif
