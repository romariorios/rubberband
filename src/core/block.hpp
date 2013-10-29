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
        list(const expr *obj_array[], int size);
        void set_symbol_table(const object &sym_table);
        void set_arg(const object &arg);
        object eval();
        
    private:
        const expr *_obj_array;
        int _size;
    };
    
    class generic_object : public expr
    {
    public:
        generic_object(const expr *symbol_array[], const expr *obj_array[], int size);
        object eval();
        
    private:
        const expr **_symbol_array;
        const expr **_obj_array;
        int _size;
    };
    
    class symbol_table : public expr
    {
    public:
        inline symbol_table() {}
        inline object eval() { return _sym_table; }
        
    protected:
        object _sym_table;
    };
    
    class block_arg : public expr
    {
    public:
        inline block_arg() {}
        inline object eval() { return _arg; }
        
    protected:
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
        
    private:
        block_private *_p;
    };
}

}

#endif
