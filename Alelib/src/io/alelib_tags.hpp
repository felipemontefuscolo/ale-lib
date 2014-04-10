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


#ifndef ALELIB_ALE_TAGS_HPP
#define ALELIB_ALE_TAGS_HPP

#include <tr1/array>
#include "../util/misc2.hpp"
#include "../util/assert.hpp"

namespace alelib {

namespace fi_AlelibTagsInitializers
{
  std::tr1::array<int, N_CELL_TYPES> ctypeNumNodes_ini()
  {
    std::tr1::array<int, N_CELL_TYPES> tab;

    tab[log2_i32(POINT        ) ] = 2;
    tab[log2_i32(EDGE         ) ] = 2;
    tab[log2_i32(TRIANGLE     ) ] = 3;
    tab[log2_i32(QUADRANGLE   ) ] = 4;
    tab[log2_i32(TETRAHEDRON  ) ] = 4;
    tab[log2_i32(HEXAHEDRON   ) ] = 8;

    return tab;
  }

  std::tr1::array<int, N_CELL_TYPES> ctypeDim_ini()
  {
    std::tr1::array<int, N_CELL_TYPES> tab;

    tab[log2_i32(POINT        ) ] = 0;
    tab[log2_i32(EDGE         ) ] = 1;
    tab[log2_i32(TRIANGLE     ) ] = 2;
    tab[log2_i32(QUADRANGLE   ) ] = 2;
    tab[log2_i32(TETRAHEDRON  ) ] = 3;
    tab[log2_i32(HEXAHEDRON   ) ] = 3;

    return tab;
  }

  std::tr1::array<const char*, N_CELL_TYPES> ctypeName_ini()
  {
    std::tr1::array<const char*, N_CELL_TYPES> tab;

    tab[log2_i32(POINT        ) ] = "Point";
    tab[log2_i32(EDGE         ) ] = "Edge";
    tab[log2_i32(TRIANGLE     ) ] = "Triangle";
    tab[log2_i32(QUADRANGLE   ) ] = "Quadrangle";
    tab[log2_i32(TETRAHEDRON  ) ] = "Tetrahedron";
    tab[log2_i32(HEXAHEDRON   ) ] = "Hexahedron";

    return tab;
  }

  std::tr1::array<ECellType, MSH_MAX_INDEX+1> mshTag2ctype_ini()
  {
    std::tr1::array<ECellType, MSH_MAX_INDEX+1> tab;

    tab[MSH_PNT   ] = POINT;
    tab[MSH_LIN_2 ] = EDGE;
    tab[MSH_TRI_3 ] = TRIANGLE;
    tab[MSH_QUA_4 ] = QUADRANGLE;
    tab[MSH_TET_4 ] = TETRAHEDRON;
    tab[MSH_HEX_8 ] = HEXAHEDRON;

    return tab;
  }

  std::tr1::array<EMshTag, N_CELL_TYPES> ctype2mshTag_ini()
  {
    std::tr1::array<EMshTag, N_CELL_TYPES> tab;

    tab[log2_i32(POINT      ) ] = MSH_PNT;
    tab[log2_i32(EDGE       ) ] = MSH_LIN_2 ;
    tab[log2_i32(TRIANGLE   ) ] = MSH_TRI_3 ;
    tab[log2_i32(QUADRANGLE ) ] = MSH_QUA_4 ;
    tab[log2_i32(TETRAHEDRON) ] = MSH_TET_4 ;
    tab[log2_i32(HEXAHEDRON ) ] = MSH_HEX_8 ;

    return tab;
  }

    std::tr1::array<ECellFamily, N_CELL_TYPES> ctype2cfamily_ini()
  {
    std::tr1::array<ECellFamily, N_CELL_TYPES> tab;

    tab[log2_i32(POINT      ) ] = static_cast<ECellFamily>(SIMPLEX | HCUBE);
    tab[log2_i32(EDGE       ) ] = static_cast<ECellFamily>(SIMPLEX | HCUBE);
    tab[log2_i32(TRIANGLE   ) ] = SIMPLEX;
    tab[log2_i32(QUADRANGLE ) ] = HCUBE;
    tab[log2_i32(TETRAHEDRON) ] = SIMPLEX;
    tab[log2_i32(HEXAHEDRON ) ] = HCUBE;

    return tab;
  }

    std::tr1::array<ECellType, N_CELL_TYPES> facetof_ini()
  {
    std::tr1::array<ECellType, N_CELL_TYPES> tab;

    tab[log2_i32(POINT        ) ] = UNDEFINED_CELLT;
    tab[log2_i32(EDGE         ) ] = POINT;
    tab[log2_i32(TRIANGLE     ) ] = EDGE;
    tab[log2_i32(QUADRANGLE   ) ] = EDGE;
    tab[log2_i32(TETRAHEDRON  ) ] = TRIANGLE;
    tab[log2_i32(HEXAHEDRON   ) ] = QUADRANGLE;

    return tab;
  }

}

int ctypeNumNodes(ECellType type)
{
  static const
  std::tr1::array<int, N_CELL_TYPES> n_nds = fi_AlelibTagsInitializers::ctypeNumNodes_ini();

  unsigned idx = log2_i32(type);
  if (idx >= N_CELL_TYPES)
  {
    ALELIB_CHECK(false, "invalid cell type", std::invalid_argument);
    return -1;
  }

  return n_nds[idx];

}

int ctypeDim(ECellType type)
{

  static const
  std::tr1::array<int, N_CELL_TYPES> dims = fi_AlelibTagsInitializers::ctypeDim_ini();

  unsigned idx = log2_i32(type);
  if (idx >= N_CELL_TYPES)
  {
    ALELIB_CHECK(false, "invalid cell type", std::invalid_argument);
    return -1;
  }

  return dims[idx];
}

const char* ctypeName(ECellType type)
{
  static const
  std::tr1::array<const char*, N_CELL_TYPES> names = fi_AlelibTagsInitializers::ctypeName_ini();

  unsigned idx = log2_i32(type);
  if (idx >= N_CELL_TYPES)
  {
    ALELIB_CHECK(false, "invalid cell type", std::invalid_argument);
    return NULL;
  }

  return names[idx];
}

ECellType mshTag2ctype(EMshTag type)
{
  static const
  std::tr1::array<ECellType, MSH_MAX_INDEX+1> tab = fi_AlelibTagsInitializers::mshTag2ctype_ini();

  unsigned idx = static_cast<unsigned>(type);
  if (idx-1 >= MSH_MAX_INDEX)
  {
    ALELIB_CHECK(false, "invalid cell type", std::invalid_argument);
    return UNDEFINED_CELLT;
  }

  return tab[idx];

}

EMshTag ctype2mshTag(ECellType type)
{
  static const
  std::tr1::array<EMshTag, N_CELL_TYPES> tab = fi_AlelibTagsInitializers::ctype2mshTag_ini();

  unsigned idx = log2_i32(type);
  if (idx >= N_CELL_TYPES)
  {
    ALELIB_CHECK(false, "invalid cell type", std::invalid_argument);
    return MSH_UNDEFINED_ELEM;
  }

  return tab[idx];
}


ECellFamily ctype2cfamily(ECellType type)
{
  std::tr1::array<ECellFamily, N_CELL_TYPES> tab = fi_AlelibTagsInitializers::ctype2cfamily_ini();

  unsigned idx = log2_i32(type);
  if (idx >= N_CELL_TYPES)
  {
    ALELIB_CHECK(false, "invalid cell type", std::invalid_argument);
    return UNDEFINED_CELLF;
  }
  return tab[idx];
}

ECellType facetof(ECellType type)
{
  std::tr1::array<ECellType, N_CELL_TYPES> tab = fi_AlelibTagsInitializers::facetof_ini();

  unsigned idx = log2_i32(type);
  if (idx >= N_CELL_TYPES)
  {
    ALELIB_CHECK(false, "invalid cell type", std::invalid_argument);
    return UNDEFINED_CELLT;
  }
  return tab[idx];
}


} // end namespace alelib

#endif
