// Rubberband language
// Copyright (C) 2014  Luiz Romário Santana Rios <luizromario at gmail dot com>
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

#ifndef BLOCK_PRIVATE
#define BLOCK_PRIVATE

#include "data_templates.hpp"

namespace rbb
{

class block_statement;
class expr;

class block_statement_private
{
public:
    block_statement_private() :
        expressions(0),
        expressions_tail(0)
    {}
    ~block_statement_private()
    {
        if (expressions)
            delete expressions;
    }

    linked_list<expr *> *expressions;
    linked_list<expr *> *expressions_tail;
};

namespace literal
{
    class block_private
    {
    public:
        block_private() :
        statements(0),
        statements_tail(0),
        _return_expression(new literal::empty)
        {}

        ~block_private()
        {
            if (statements)
                delete statements;

            delete _return_expression;
        }

        void set_return_expression(expr *ret_exp)
        {
            delete _return_expression;
            _return_expression = ret_exp;
        }

        expr *return_expression() const
        {
            return _return_expression;
        }

        linked_list<block_statement *> *statements;
        linked_list<block_statement *> *statements_tail;

    private:
        expr *_return_expression;
    };
}

}

#endif
