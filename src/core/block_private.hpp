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

#include <forward_list>
#include <memory>

namespace rbb
{

class block_statement;

namespace literal
{
    class block_private
    {
    public:
        std::forward_list<block_statement> statements;
        std::forward_list<block_statement>::iterator statements_tail =
            statements.before_begin();

        std::unique_ptr<block_statement> return_statement{new block_statement};
    };
}

}

#endif
