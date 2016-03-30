%include
{
    #include "block.hpp"
    #include "error.hpp"
    #include "object.hpp"
    #include "parse.hpp"
    #include "parse_private.hpp"
    #include "tokenizer.hpp"

    #include <cassert>

    using namespace rbb;

    struct __table_entry
    {
        expr *index;
        expr *object;
    };
}

%name LemonCParser

%token_type {token *}
%token_destructor { delete $$; }

%extra_argument { extra_lemon_args *extra_args }

%syntax_error { throw syntax_error{*yyminor.yy0}; }

%type start {literal::block *}
%type block_body {literal::block *}
%type stm_list {literal::block *}
%type ret_stm {block_statement *}
%type stm {block_statement *}
%type msg_send {block_statement *}
%type expr_value {expr *}
%type literal {expr *}
%type empty {literal::empty *}
%type array {literal::array *}
%type array_body {literal::array *}
%type array_elements {literal::array *}
%type maybe_empty_a {literal::array *}
%type table {literal::table *}
%type table_body {literal::table *}
%type table_entries {literal::table *}
%type maybe_empty_t {literal::table *}
%type table_entry {__table_entry}
%type table_index {block_statement *}
%type block {literal::block *}

start               ::= literal(l). { extra_args->result = l->eval(nullptr); }
start               ::= .
block_body(bl)      ::= stm_list(bl_a) ret_stm(ret).
{
    bl_a->set_return_statement_ptr(ret);
    bl = bl_a;
}
block_body(bl)      ::= stm_list(bl_a). { bl = bl_a; }
block_body(bl)      ::= ret_stm(ret).
{
    bl = new literal::block;
    bl->set_return_statement_ptr(ret);
}
stm_list(bl)        ::= stm_list(bl_a) SEP stm(s).
{
    bl_a->add_statement_ptr(s);
    bl = bl_a;
}
stm_list(bl)        ::= stm(s).
{
    bl = new literal::block;
    bl->add_statement_ptr(s);
}
ret_stm(ret)        ::= EXCLAMATION stm(s). { ret = s; }
stm(s)              ::= msg_send(chain). { s = chain; }
stm(s)              ::= expr_value(e).
{
    s = new block_statement;
    s->add_expr_ptr(e);
}
msg_send(chain)     ::= expr_value(expr) stm(s).
{
    s->add_expr_ptr(expr);
    chain = s;
}
expr_value(e)       ::= literal(l). { e = l; }
expr_value(e)       ::= PARENTHESIS_OPEN stm(s) PARENTHESIS_CLOSE. { e = s; }
expr_value(e)       ::= expr_value(msg) DOT expr_value(rcv).
{
    auto stm = new block_statement;
    stm->add_expr_ptr(msg);
    stm->add_expr_ptr(rcv);
    e = stm;
}
literal(l)          ::= empty(e). { l = e; }
literal(l)          ::= SYMBOL(tok). { l = new literal::symbol{std::string{*tok->lexem.str}}; }
literal(l)          ::= NUMBER(tok).
{
    if (tok->type == token::t::number) {
        l = new literal::number{tok->lexem.integer};
    } else if (tok->type == token::t::number_f) {
        l = new literal::number{tok->lexem.floating};
    } else {
        l = new literal::empty;
    }
}
literal(l)          ::= BOOLEAN(tok). { l = new literal::boolean{tok->lexem.boolean}; }
literal(l)          ::= array(arr). { l = arr; }
literal(l)          ::= table(t). { l = t; }
literal(l)          ::= block(bl). { l = bl; }
literal(l)          ::= DOLLAR. { l = new literal::message; }
literal(l)          ::= TILDE. { l = new literal::context; }
literal(l)          ::= AT. { l = new literal::self_ref; }
literal(l)          ::= PERCENT. { l = new evaluated_object{extra_args->master}; }
literal(l)          ::= CUSTOM_LITERAL(tok). { l = new evaluated_object{*tok->lexem.obj}; }
empty(e)            ::= PARENTHESIS_OPEN PARENTHESIS_CLOSE. { e = new literal::empty; }
array(arr)          ::= BAR maybe_empty_a(arr_a). { arr = arr_a; }
array_elements(arr) ::= stm(e) COMMA array_elements(arr_a).
{
    arr_a->add_element_ptr(e);
    arr = arr_a;
}
array_elements(arr) ::= stm(e).
{
    arr = new literal::array;
    arr->add_element_ptr(e);
}
maybe_empty_a(arr)    ::= . { arr = new literal::array; }
maybe_empty_a(arr)    ::= array_elements(arr_a). { arr = arr_a; }
table(t)            ::= COLON maybe_empty_t(t_body). { t = t_body; }
table_entries(t)    ::= table_entries(t_a) COMMA table_entry(entry).
{
    t_a->add_symbol_ptr(entry.index);
    t_a->add_object_ptr(entry.object);
    t = t_a;
}
table_entries(t)    ::= table_entry(entry).
{
    t = new literal::table;
    t->add_symbol_ptr(entry.index);
    t->add_object_ptr(entry.object);
}
maybe_empty_t(t)    ::= . { t = new literal::table; }
maybe_empty_t(t)    ::= table_entries(t_a). { t = t_a; }
table_entry(entry)  ::= stm(i) ARROW stm(s).
{
    entry.index = i;
    entry.object = s;
}
block(bl)           ::= CURLY_OPEN block_body(bl_a) CURLY_CLOSE. { bl = bl_a; }
block(bl)           ::= CURLY_OPEN CURLY_CLOSE. { bl = new literal::block; }
