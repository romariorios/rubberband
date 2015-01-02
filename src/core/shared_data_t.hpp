// Rubberband language
// Copyright (C) 2015  Luiz Romário Santana Rios <luizromario at gmail dot com>
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

#ifndef SHARED_DATA_T
#define SHARED_DATA_T

namespace rbb
{

class shared_data_t
{
public:
    shared_data_t() :
        refc(1)
    {}

    virtual ~shared_data_t()
    {}
    
    virtual std::string to_string() const {
        return "[unknown data]";
    }

    int refc;
};

}

#endif

