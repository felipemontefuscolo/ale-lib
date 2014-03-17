// This file is part of Alelib, a toolbox for finite element codes.
//
// Alelib is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// Alternatively, you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// Alelib is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License and a copy of the GNU General Public License along with
// Alelib. If not, see <http://www.gnu.org/licenses/>.

#ifndef ALELIB_VERTEX_HPP
#define ALELIB_VERTEX_HPP

#include <list>
#include <utility>

#include "labelable.hpp"
#include "set_vector.hpp"

namespace alelib
{



struct Vertex : public Labelable
{
  typedef SetVector<index_t> VectorT;
  
  uint8_t status;    // padding.
  // uint8_t padding1;  // padding.
  // int32_t padding2;  // padding, because sizeof(icells) = 24.
  VectorT icells;


  enum Masks
  {
    mk_none = 0
   // mk_inboundary = (1<<0)
  };

  // Construtor.
  explicit
  Vertex(int8_t tag, Flags flags=NO_FLAG, Masks stat=mk_none) : Labelable(tag, flags), status(static_cast<uint8_t>(stat))
  { }

  /// Construtor.
  ///
  Vertex() : Labelable(), status(mk_none) {}


  inline unsigned valency() const
  { return icells.size(); }

  inline void reserve(SetVector<index_t>::size_type size)
  { icells.reserve(size); }


};





} // end namespace alelib


#endif
