// This file is part of FEPiC++, a toolbox for finite element codes.
//
// FEPiC++ is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// Alternatively, you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// FEPiC++ is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License and a copy of the GNU General Public License along with
// FEPiC++. If not, see <http://www.gnu.org/licenses/>.

#ifndef ALELIB_DIRECTIVES_HPP
#define ALELIB_DIRECTIVES_HPP

/* user config */
#ifdef _OPENMP
#  define ALE_HAS_OPENMP
#endif

// #define ALE_HAS_OMPTL
// #define ALE_HAS_METIS
// #define ALE_HAS_BOOST

#define ALELIB_DEBUG_ON

#define ALELIB_SCALAR_TYPE double

#define ALELIB_INDEX_TYPE int //std::size_t


#ifdef ALE_HAS_OPENMP
#   define ALE_PRAGMA_OMP(x) ALE_DO_PRAGMA(omp x)
#   define ALE_DO_PRAGMA(x) _Pragma ( #x ) // _Pragma eh soh pra linux?
#else
#   define ALE_PRAGMA_OMP(x)
#endif

// TODO: put this in another file
// -----------------  TYPEDEFS -------------------------

#include <cstddef> // std::size_t
#include <stdint.h> // std::int8_t

namespace alelib{
  
typedef ALELIB_SCALAR_TYPE Real;

typedef ALELIB_INDEX_TYPE index_t;

#define NULL_IDX (-1)

}

#endif
