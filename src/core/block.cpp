#include "block.hpp"

using namespace rbb;

class rbb::block_statement_private
{
public:
    block_statement_private() :
        expressions(0),
        expressions_tail(0)
    {}
    
    linked_list<expr *> *expressions;
    linked_list<expr *> *expressions_tail;
};

block_statement::block_statement() :
    _p(new block_statement_private)
{}

block_statement::~block_statement()
{
    delete _p->expressions;
    delete _p;
}

void block_statement::set_symbol_table(const object& sym_table)
{
    for (linked_list<expr *> *cur_expr = _p->expressions;
         cur_expr; cur_expr = cur_expr->next)
        cur_expr->value->set_symbol_table(sym_table);
}

void block_statement::set_arg(const object& arg)
{
    for (linked_list<expr *> *cur_expr = _p->expressions;
         cur_expr; cur_expr = cur_expr->next)
        cur_expr->value->set_arg(arg);
}

void block_statement::add_expr(expr* e)
{
    if (!_p->expressions) {
        _p->expressions = new linked_list<expr *>(e);
        _p->expressions_tail = _p->expressions;
        return;
    }
    
    _p->expressions_tail->next = new linked_list<expr *>(e);
    _p->expressions_tail = _p->expressions_tail->next;
}

object block_statement::eval()
{
    object cur_obj = empty();
    
    for (linked_list<expr *> *cur_expr = _p->expressions;
         cur_expr; cur_expr = cur_expr->next)
        cur_obj = cur_expr->value->eval();
    
    return cur_obj;
}
