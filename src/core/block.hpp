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

#include <forward_list>
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
    typedef std::unique_ptr<expr> ptr;
    
    expr() {}
    expr(const expr &other) = delete;
    virtual ~expr() {}

    virtual object const_eval() const
    {
        return rbb::empty();
    }
    
    virtual object eval(literal::block *parent_block)
    {
        return const_eval();
    }
    
    virtual std::string to_string() const
    {
        return const_eval().to_string();
    }
};

class expr_list : public std::forward_list<expr::ptr>
{
public:
    template <class T, class... Args>
    T &append(Args&&... a)
    {
        list_end = emplace_after(list_end, new T{a...});
        return *static_cast<T *>(list_end->get());
    }
    
private:
    std::forward_list<expr::ptr>::iterator list_end = before_begin();
};

class block_data;
class block_statement : public expr
{
public:
    template <class T, class... Args>
    T &add_expr(Args&&... a)
    {
        return expressions.append<T>(a...);
    }
    
    object eval(literal::block *parent_block);

    expr_list expressions;
};

namespace literal
{
    class boolean : public expr
    {
    public:
        inline boolean(bool val) : _val(val) {}
        inline object const_eval() const { return rbb::boolean(_val); }

    private:
        bool _val;
    };

    class empty : public expr
    {
    public:
        inline empty() {}
        inline object const_eval() const { return rbb::empty(); }
    };

    class number : public expr
    {
    public:
        inline number(double val) : _val(val) {}
        inline number(int val) : _val{static_cast<double>(val)} {}
        inline object const_eval() const { return rbb::number(_val); }

    private:
        double _val;
    };

    class symbol : public expr
    {
    public:
        inline symbol(const std::string &str) : _sym(rbb::symbol(str)) {}
        inline object const_eval() const { return _sym; }

    private:
        object _sym;
    };

    class array : public expr
    {
    public:
        object eval(block *parent_block);
        
        template<class T, class... Args>
        T &add_element(Args&&... a)
        {
            return _objects.append<T>(a...);
        }

    private:
        expr_list _objects;
    };

    class table : public expr
    {
    public:
        object eval(block *parent_block);
        
        template <class T, class... Args>
        T &add_symbol(Args&&... a)
        {
            return _symbols.append<T>(a...);
        }
        
        template <class T, class... Args>
        T &add_object(Args&&... a)
        {
            return _objects.append<T>(a...);
        }

    private:
        expr_list _symbols;
        expr_list _objects;
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
        block_statement &add_statement();
        block_statement &return_statement();
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

