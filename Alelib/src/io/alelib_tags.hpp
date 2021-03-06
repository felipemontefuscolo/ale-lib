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

#include "../util/misc2.hpp"
#include "../util/assert.hpp"

#include <ciso646>  // detect std::lib
#ifdef _LIBCPP_VERSION
// using libc++
#define MULTI_HAVE_TYPE_TRAITS
#else
// using libstdc++
#define MULTI_HAVE_TR1_TYPE_TRAITS
#endif

#ifdef MULTI_HAVE_TYPE_TRAITS
#include <array>
namespace Tr1 = std;
#else
#include <tr1/array>
namespace Tr1 = std::tr1;
#endif


namespace alelib
{

namespace fi_AlelibTagsInitializers
{
  static inline
  Tr1::array<int, N_CELL_TYPES> CTypeNumNodes_ini()
  {
    Tr1::array<int, N_CELL_TYPES> tab;

    tab[log2_i32(POINT        ) ] = 2;
    tab[log2_i32(EDGE         ) ] = 2;
    tab[log2_i32(TRIANGLE     ) ] = 3;
    tab[log2_i32(QUADRANGLE   ) ] = 4;
    tab[log2_i32(TETRAHEDRON  ) ] = 4;
    tab[log2_i32(HEXAHEDRON   ) ] = 8;

    return tab;
  }

  static inline
  Tr1::array<int, N_CELL_TYPES> CTypeDim_ini()
  {
    Tr1::array<int, N_CELL_TYPES> tab;

    tab[log2_i32(POINT        ) ] = 0;
    tab[log2_i32(EDGE         ) ] = 1;
    tab[log2_i32(TRIANGLE     ) ] = 2;
    tab[log2_i32(QUADRANGLE   ) ] = 2;
    tab[log2_i32(TETRAHEDRON  ) ] = 3;
    tab[log2_i32(HEXAHEDRON   ) ] = 3;

    return tab;
  }

  static inline
  Tr1::array<const char*, N_CELL_TYPES> CTypeName_ini()
  {
    Tr1::array<const char*, N_CELL_TYPES> tab;

    tab[log2_i32(POINT        ) ] = "Point";
    tab[log2_i32(EDGE         ) ] = "Edge";
    tab[log2_i32(TRIANGLE     ) ] = "Triangle";
    tab[log2_i32(QUADRANGLE   ) ] = "Quadrangle";
    tab[log2_i32(TETRAHEDRON  ) ] = "Tetrahedron";
    tab[log2_i32(HEXAHEDRON   ) ] = "Hexahedron";

    return tab;
  }

  static inline
  Tr1::array<ECellType, MSH_MAX_INDEX+1> mshTag2CType_ini()
  {
    Tr1::array<ECellType, MSH_MAX_INDEX+1> tab;

    tab[MSH_PNT   ] = POINT;
    tab[MSH_LIN_2 ] = EDGE;
    tab[MSH_TRI_3 ] = TRIANGLE;
    tab[MSH_QUA_4 ] = QUADRANGLE;
    tab[MSH_TET_4 ] = TETRAHEDRON;
    tab[MSH_HEX_8 ] = HEXAHEDRON;

    return tab;
  }

  static inline
  Tr1::array<EMshTag, N_CELL_TYPES> CType2mshTag_ini()
  {
    Tr1::array<EMshTag, N_CELL_TYPES> tab;

    tab[log2_i32(POINT      ) ] = MSH_PNT;
    tab[log2_i32(EDGE       ) ] = MSH_LIN_2 ;
    tab[log2_i32(TRIANGLE   ) ] = MSH_TRI_3 ;
    tab[log2_i32(QUADRANGLE ) ] = MSH_QUA_4 ;
    tab[log2_i32(TETRAHEDRON) ] = MSH_TET_4 ;
    tab[log2_i32(HEXAHEDRON ) ] = MSH_HEX_8 ;

    return tab;
  }

  static inline
  Tr1::array<ECellFamily, N_CELL_TYPES> CType2cfamily_ini()
  {
    Tr1::array<ECellFamily, N_CELL_TYPES> tab;

    tab[log2_i32(POINT      ) ] = static_cast<ECellFamily>(SIMPLEX | HCUBE);
    tab[log2_i32(EDGE       ) ] = static_cast<ECellFamily>(SIMPLEX | HCUBE);
    tab[log2_i32(TRIANGLE   ) ] = SIMPLEX;
    tab[log2_i32(QUADRANGLE ) ] = HCUBE;
    tab[log2_i32(TETRAHEDRON) ] = SIMPLEX;
    tab[log2_i32(HEXAHEDRON ) ] = HCUBE;

    return tab;
  }

  static inline
  Tr1::array<ECellType, N_CELL_TYPES> facetof_ini()
  {
    Tr1::array<ECellType, N_CELL_TYPES> tab;

    tab[log2_i32(POINT        ) ] = UNDEFINED_CELLT;
    tab[log2_i32(EDGE         ) ] = POINT;
    tab[log2_i32(TRIANGLE     ) ] = EDGE;
    tab[log2_i32(QUADRANGLE   ) ] = EDGE;
    tab[log2_i32(TETRAHEDRON  ) ] = TRIANGLE;
    tab[log2_i32(HEXAHEDRON   ) ] = QUADRANGLE;

    return tab;
  }

}

static inline
int CTypeNumNodes(ECellType type)
{
  static const
  Tr1::array<int, N_CELL_TYPES> n_nds = fi_AlelibTagsInitializers::CTypeNumNodes_ini();

  unsigned idx = log2_i32(type);
  if (idx >= N_CELL_TYPES)
  {
    ALELIB_CHECK(false, "invalid cell type", std::invalid_argument);
    return -1;
  }

  return n_nds[idx];

}

static inline
int CTypeDim(ECellType type)
{

  static const
  Tr1::array<int, N_CELL_TYPES> dims = fi_AlelibTagsInitializers::CTypeDim_ini();

  unsigned idx = log2_i32(type);
  if (idx >= N_CELL_TYPES)
  {
    ALELIB_CHECK(false, "invalid cell type", std::invalid_argument);
    return -1;
  }

  return dims[idx];
}

static inline
const char* CTypeName(ECellType type)
{
  static const
  Tr1::array<const char*, N_CELL_TYPES> names = fi_AlelibTagsInitializers::CTypeName_ini();

  unsigned idx = log2_i32(type);
  if (idx >= N_CELL_TYPES)
  {
    ALELIB_CHECK(false, "invalid cell type", std::invalid_argument);
    return NULL;
  }

  return names[idx];
}

static inline
ECellType mshTag2CType(EMshTag type)
{
  static const
  Tr1::array<ECellType, MSH_MAX_INDEX+1> tab = fi_AlelibTagsInitializers::mshTag2CType_ini();

  unsigned idx = static_cast<unsigned>(type);
  if (idx-1 >= MSH_MAX_INDEX)
  {
    ALELIB_CHECK(false, "invalid cell type", std::invalid_argument);
    return UNDEFINED_CELLT;
  }

  return tab[idx];

}

static inline
EMshTag CType2mshTag(ECellType type)
{
  static const
  Tr1::array<EMshTag, N_CELL_TYPES> tab = fi_AlelibTagsInitializers::CType2mshTag_ini();

  unsigned idx = log2_i32(type);
  if (idx >= N_CELL_TYPES)
  {
    ALELIB_CHECK(false, "invalid cell type", std::invalid_argument);
    return MSH_UNDEFINED_ELEM;
  }

  return tab[idx];
}

static inline
void mshTypeAndOrder(EMshTag msh, /*out*/ ECellType& type, /*out*/int& order , /*out*/int &dim )
{
  switch (msh)
  {
    case MSH_LIN_2      : type = EDGE;            order =  1; dim = 1; break;
    case MSH_TRI_3      : type = TRIANGLE;        order =  1; dim = 2; break;
    case MSH_QUA_4      : type = QUADRANGLE;      order =  1; dim = 2; break;
    case MSH_TET_4      : type = TETRAHEDRON;     order =  1; dim = 3; break;
    case MSH_HEX_8      : type = HEXAHEDRON;      order =  1; dim = 3; break;
    case MSH_PRI_6      : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_PYR_5      : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_LIN_3      : type = EDGE;            order =  2; dim = 1; break;
    case MSH_TRI_6      : type = TRIANGLE;        order =  2; dim = 2; break;
    case MSH_QUA_9      : type = QUADRANGLE;      order =  2; dim = 2; break;
    case MSH_TET_10     : type = TETRAHEDRON;     order =  2; dim = 3; break;
    case MSH_HEX_27     : type = HEXAHEDRON;      order =  2; dim = 3; break;
    case MSH_PRI_18     : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_PYR_14     : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_PNT        : type = POINT;           order =  1; dim = 0; break;
    case MSH_QUA_8      : type = QUADRANGLE;      order = -1; dim = 2; break;
    case MSH_HEX_20     : type = HEXAHEDRON;      order = -1; dim = 3; break;
    case MSH_PRI_15     : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_PYR_13     : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_TRI_9      : type = TRIANGLE;        order = -1; dim = 2; break;
    case MSH_TRI_10     : type = TRIANGLE;        order =  3; dim = 2; break;
    case MSH_TRI_12     : type = TRIANGLE;        order = -1; dim = 2; break;
    case MSH_TRI_15     : type = TRIANGLE;        order =  4; dim = 2; break;
    case MSH_TRI_15I    : type = TRIANGLE;        order = -1; dim = 2; break;
    case MSH_TRI_21     : type = TRIANGLE;        order =  5; dim = 2; break;
    case MSH_LIN_4      : type = EDGE;            order =  3; dim = 1; break;
    case MSH_LIN_5      : type = EDGE;            order =  4; dim = 1; break;
    case MSH_LIN_6      : type = EDGE;            order =  5; dim = 1; break;
    case MSH_TET_20     : type = TETRAHEDRON;     order =  3; dim = 3; break;
    case MSH_TET_35     : type = TETRAHEDRON;     order =  4; dim = 3; break;
    case MSH_TET_56     : type = TETRAHEDRON;     order =  5; dim = 3; break;
    case MSH_TET_22     : type = TETRAHEDRON;     order = -1; dim = 3; break;
    case MSH_TET_28     : type = TETRAHEDRON;     order = -1; dim = 3; break;
    case MSH_POLYG_     : type = UNDEFINED_CELLT; order = -1; dim = 2; break;
    case MSH_POLYH_     : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_QUA_16     : type = QUADRANGLE;      order =  3; dim = 2; break;
    case MSH_QUA_25     : type = QUADRANGLE;      order =  4; dim = 2; break;
    case MSH_QUA_36     : type = QUADRANGLE;      order =  5; dim = 2; break;
    case MSH_QUA_12     : type = QUADRANGLE;      order = -1; dim = 2; break;
    case MSH_QUA_16I    : type = QUADRANGLE;      order = -1; dim = 2; break;
    case MSH_QUA_20     : type = QUADRANGLE;      order = -1; dim = 2; break;
    case MSH_TRI_28     : type = TRIANGLE;        order =  6; dim = 2; break;
    case MSH_TRI_36     : type = TRIANGLE;        order =  7; dim = 2; break;
    case MSH_TRI_45     : type = TRIANGLE;        order =  8; dim = 2; break;
    case MSH_TRI_55     : type = TRIANGLE;        order =  9; dim = 2; break;
    case MSH_TRI_66     : type = TRIANGLE;        order =  10;dim = 2; break;
    case MSH_QUA_49     : type = QUADRANGLE;      order =  6; dim = 2; break;
    case MSH_QUA_64     : type = QUADRANGLE;      order =  7; dim = 2; break;
    case MSH_QUA_81     : type = QUADRANGLE;      order =  8; dim = 2; break;
    case MSH_QUA_100    : type = QUADRANGLE;      order =  9; dim = 2; break;
    case MSH_QUA_121    : type = QUADRANGLE;      order =  10;dim = 2; break;
    case MSH_TRI_18     : type = TRIANGLE;        order = -1; dim = 2; break;
    case MSH_TRI_21I    : type = TRIANGLE;        order = -1; dim = 2; break;
    case MSH_TRI_24     : type = TRIANGLE;        order = -1; dim = 2; break;
    case MSH_TRI_27     : type = TRIANGLE;        order = -1; dim = 2; break;
    case MSH_TRI_30     : type = TRIANGLE;        order = -1; dim = 2; break;
    case MSH_QUA_24     : type = QUADRANGLE;      order = -1; dim = 2; break;
    case MSH_QUA_28     : type = QUADRANGLE;      order = -1; dim = 2; break;
    case MSH_QUA_32     : type = QUADRANGLE;      order = -1; dim = 2; break;
    case MSH_QUA_36I    : type = QUADRANGLE;      order = -1; dim = 2; break;
    case MSH_QUA_40     : type = QUADRANGLE;      order = -1; dim = 2; break;
    case MSH_LIN_7      : type = EDGE;            order = 6;  dim = 1; break;
    case MSH_LIN_8      : type = EDGE;            order = 7;  dim = 1; break;
    case MSH_LIN_9      : type = EDGE;            order = 8;  dim = 1; break;
    case MSH_LIN_10     : type = EDGE;            order = 9;  dim = 1; break;
    case MSH_LIN_11     : type = EDGE;            order = 10; dim = 1; break;
    case MSH_LIN_B      : type = EDGE;            order = -1; dim = 1; break;
    case MSH_TRI_B      : type = TRIANGLE;        order = -1; dim = 2; break;
    case MSH_POLYG_B    : type = UNDEFINED_CELLT; order = -1; dim = 2; break;
    case MSH_LIN_C      : type = EDGE;            order = -1; dim = 1; break;
    case MSH_TET_84     : type = TETRAHEDRON;     order = 6;  dim = 3; break;
    case MSH_TET_120    : type = TETRAHEDRON;     order = 7;  dim = 3; break;
    case MSH_TET_165    : type = TETRAHEDRON;     order = 8;  dim = 3; break;
    case MSH_TET_220    : type = TETRAHEDRON;     order = 9;  dim = 3; break;
    case MSH_TET_286    : type = TETRAHEDRON;     order = 10; dim = 3; break;
    case MSH_TET_34     : type = TETRAHEDRON;     order = -1; dim = 3; break;
    case MSH_TET_40     : type = TETRAHEDRON;     order = -1; dim = 3; break;
    case MSH_TET_46     : type = TETRAHEDRON;     order = -1; dim = 3; break;
    case MSH_TET_52     : type = TETRAHEDRON;     order = -1; dim = 3; break;
    case MSH_TET_58     : type = TETRAHEDRON;     order = -1; dim = 3; break;
    case MSH_LIN_1      : type = EDGE;            order = 0;  dim = 1; break;
    case MSH_TRI_1      : type = TRIANGLE;        order = 0;  dim = 2; break;
    case MSH_QUA_1      : type = QUADRANGLE;      order = 0;  dim = 2; break;
    case MSH_TET_1      : type = TETRAHEDRON;     order = 0;  dim = 3; break;
    case MSH_HEX_1      : type = HEXAHEDRON;      order = 0;  dim = 3; break;
    case MSH_PRI_1      : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_PRI_40     : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_PRI_75     : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_HEX_64     : type = HEXAHEDRON;      order = 3;  dim = 3; break;
    case MSH_HEX_125    : type = HEXAHEDRON;      order = 4;  dim = 3; break;
    case MSH_HEX_216    : type = HEXAHEDRON;      order = 5;  dim = 3; break;
    case MSH_HEX_343    : type = HEXAHEDRON;      order = 6;  dim = 3; break;
    case MSH_HEX_512    : type = HEXAHEDRON;      order = 7;  dim = 3; break;
    case MSH_HEX_729    : type = HEXAHEDRON;      order = 8;  dim = 3; break;
    case MSH_HEX_1000   : type = HEXAHEDRON;      order = 9;  dim = 3; break;
    case MSH_HEX_32     : type = HEXAHEDRON;      order = -1; dim = 3; break;
    case MSH_HEX_44     : type = HEXAHEDRON;      order = -1; dim = 3; break;
    case MSH_HEX_56     : type = HEXAHEDRON;      order = -1; dim = 3; break;
    case MSH_HEX_68     : type = HEXAHEDRON;      order = -1; dim = 3; break;
    case MSH_HEX_80     : type = HEXAHEDRON;      order = -1; dim = 3; break;
    case MSH_HEX_92     : type = HEXAHEDRON;      order = -1; dim = 3; break;
    case MSH_HEX_104    : type = HEXAHEDRON;      order = -1; dim = 3; break;
    case MSH_PRI_126    : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_PRI_196    : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_PRI_288    : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_PRI_405    : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_PRI_550    : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_PRI_24     : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_PRI_33     : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_PRI_42     : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_PRI_51     : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_PRI_60     : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_PRI_69     : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_PRI_78     : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_PYR_30     : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_PYR_55     : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_PYR_91     : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_PYR_140    : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_PYR_204    : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_PYR_285    : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_PYR_385    : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_PYR_21     : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_PYR_29     : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_PYR_37     : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_PYR_45     : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_PYR_53     : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_PYR_61     : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_PYR_69     : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_PYR_1      : type = UNDEFINED_CELLT; order = -1; dim = 3; break;
    case MSH_PNT_SUB    : type = UNDEFINED_CELLT; order = -1; dim = 0; break;
    case MSH_LIN_SUB    : type = EDGE;            order = 1;  dim = 1; break;
    case MSH_TRI_SUB    : type = TRIANGLE;        order = 1;  dim = 2; break;
    case MSH_TET_SUB    : type = TETRAHEDRON;     order = 1;  dim = 3; break;
    case MSH_TET_16     : type = TETRAHEDRON;     order = 2;  dim = 3; break;
    case MSH_TRI_MINI   : type = TRIANGLE;        order = 1;  dim = 2; break;
    default: type = UNDEFINED_CELLT; order = -1; break;
  }
  
  

}


static inline
ECellFamily CType2cfamily(ECellType type)
{
  Tr1::array<ECellFamily, N_CELL_TYPES> tab = fi_AlelibTagsInitializers::CType2cfamily_ini();

  unsigned idx = log2_i32(type);
  if (idx >= N_CELL_TYPES)
  {
    ALELIB_CHECK(false, "invalid cell type", std::invalid_argument);
    return UNDEFINED_CELLF;
  }
  return tab[idx];
}

static inline
ECellType facetof(ECellType type)
{
  Tr1::array<ECellType, N_CELL_TYPES> tab = fi_AlelibTagsInitializers::facetof_ini();

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
