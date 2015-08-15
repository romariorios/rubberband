// Rubberband language
// Copyright (C) 2014, 2015  Luiz Romário Santana Rios <luizromario at gmail dot com>
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

#ifndef ERROR_HPP
#define ERROR_HPP

#include "object.hpp"
#include "tokenizer.hpp"

#include <stdexcept>

namespace rbb
{

class tokenization_error : public std::exception
{
public:
    tokenization_error(const tokenization_error &) = default;
    tokenization_error(
        long line,
        long column,
        const std::string &lexem,
        const std::string &extra_message = {}) :
        _line{line},
        _column{column},
        _lexem{lexem},
        _extra_message{extra_message}
    {}
    
    inline long line() const { return _line; }
    inline long column() const { return _column; }
    inline const std::string &lexem() const { return _lexem; }
    inline const std::string &extra_message() const { return _extra_message; }
    inline const char *what() const noexcept
    {
        return (
            "Invalid token (" + _lexem + ") at line " + std::to_string(_line) +
            ", column " + std::to_string(_column) + " (" + _extra_message + ")"
        ).c_str();
    }
    
private:
    long _line;
    long _column;
    std::string _lexem;
    std::string _extra_message;
};

class syntax_error : public std::exception
{
public:
    syntax_error(const syntax_error &other) = default;
    inline explicit syntax_error(const token &t) : _tok{t} {}
    inline const char *what() const noexcept
    {
        return ("Syntax error at line " + std::to_string(line) +
            ", column " + std::to_string(column)).c_str();
    }

    inline const token &t() const { return _tok; }
    long line;
    long column;

private:
    token _tok;
};

class use_without_parent_block : public std::logic_error
{
public:
    use_without_parent_block(char lit) :
        logic_error{"Using " + std::string{lit} + " without a parent block"}
    {}
};

class semantic_error : public std::logic_error
{
public:
    semantic_error(
        const std::string &description,
        const object &receiver,
        const object &message) :
        logic_error{
            "When sending message " + message.to_string() +
            " to object " + receiver.to_string() +
            ": " + description},
        receiver{receiver},
        message{message}
    {}

    object receiver;
    object message;
};

template <const char *name>
class wrong_type_error : public semantic_error
{
public:
    wrong_type_error(const object &receiver, const object &message) :
        semantic_error{std::string{name} + " expected", receiver, message}
    {}
};

class message_not_recognized_error : public semantic_error
{
public:
    message_not_recognized_error(const object &receiver, const object &message) :
        semantic_error{"Message not recognized", receiver, message}
    {}
};

class in_statement_error : public std::logic_error
{
public:
    in_statement_error(const std::string &stm, const std::logic_error &e) :
        logic_error{"In statement \"" + stm + "\": " + e.what()},
        internal_error{e}
    {}

    const std::logic_error &internal_error;
};

class runtime_error : public std::runtime_error
{
public:
    runtime_error(const object &error_obj) :
        std::runtime_error{"Runtime error (error object: " + error_obj.to_string() + ")"},
        error_obj{error_obj}
    {}

    object error_obj;
};

}

#endif
