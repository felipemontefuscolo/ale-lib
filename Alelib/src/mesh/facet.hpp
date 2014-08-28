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

#ifndef ALELIB_FACET_HPP
#define ALELIB_FACET_HPP

#include "labelable.hpp"

namespace alelib
{



struct Facet : private Labelable
{
  template<class,class> friend class SeqList;
  template<typename> friend class Mesh;

  uint8_t   valency;   // padding. > 2 means non-manifold
  uint8_t   local_id;  // local id of this element on the incident cell
  index_t   icell;     // global id of the incident cell  
  index_t   opp_cell;  // opposite cell

public:
  Facet(index_t   ic,
        uint8_t   loc_id,
        index_t   oc,
        uint8_t   tag,// = NO_TAG,
        Flags     flags,// = NO_FLAG,
        uint16_t  valency_) : Labelable(tag,flags), valency(valency_), local_id(loc_id),  icell(ic), opp_cell(oc)
  {};

  Facet() : Labelable(), valency(0), local_id(NULL_IDX), icell(NULL_IDX), opp_cell(NULL_IDX) {}

private:

};

} // end namespace

#endif




