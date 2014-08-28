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

#ifndef ALELIB_RIDGE_HPP
#define ALELIB_RIDGE_HPP

#include "labelable.hpp"

namespace alelib
{

// terminology of the word ridge: http://en.wikipedia.org/wiki/Polytope


struct Ridge : private Labelable
{
  template<class,class> friend class SeqList;
  template<typename> friend class Mesh;

  uint8_t  status;     // padding.
  uint8_t  local_id;   // local id of this element on the incident cell
  uint16_t valency;    // number os cells that contain this ridge
  uint16_t padding1; 
  index_t  icell;      // global id of the incident cell  

  enum Masks
  {
    mk_none = 0
  //  mk_inboundary = (1<<0)
  };

public:
  Ridge(index_t   ic,
        uint8_t   loc_id,
        uint8_t   tag = NO_TAG,
        Flags     flags = NO_FLAG,
        uint16_t  valency_ = 0) : Labelable(tag,flags), status(0), local_id(loc_id), valency(valency_), icell(ic)
  {};

  Ridge() : Labelable(), status(mk_none), local_id(NULL_IDX), valency(0), icell(NULL_IDX){}

private:

};

} // end alelib namespace

#endif
































