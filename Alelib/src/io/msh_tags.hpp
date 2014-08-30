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

#ifndef ALELIB_MSH_TAGS_HPP
#define ALELIB_MSH_TAGS_HPP

#include "../mesh/enums.hpp"

//#include "alelib_tags.hpp"
#include "../util/assert.hpp"
#include "../util/misc2.hpp"
#include <tr1/array>

namespace alelib {

namespace fi_MshTagsInitializers
{
  static inline
  std::tr1::array<int, MSH_MAX_INDEX+1> n_nodes_4_elm_T()
  {
    std::tr1::array<int, MSH_MAX_INDEX+1> tab;
    
    tab[MSH_LIN_2   ] = 2     ;
    tab[MSH_TRI_3   ] = 3     ;
    tab[MSH_QUA_4   ] = 4     ;
    tab[MSH_TET_4   ] = 4     ;
    tab[MSH_HEX_8   ] = 8     ;
    tab[MSH_PRI_6   ] = 6     ;
    tab[MSH_PYR_5   ] = 5     ;
    tab[MSH_LIN_3   ] = 3     ;
    tab[MSH_TRI_6   ] = 6     ;
    tab[MSH_QUA_9   ] = 9     ;
    tab[MSH_TET_10  ] = 10    ;
    tab[MSH_HEX_27  ] = 27    ;
    tab[MSH_PRI_18  ] = 18    ;
    tab[MSH_PYR_14  ] = 14    ;
    tab[MSH_QUA_8   ] = 8     ;
    tab[MSH_HEX_20  ] = 20    ;
    tab[MSH_PRI_15  ] = 15    ;
    tab[MSH_PYR_13  ] = 13    ;
    tab[MSH_TRI_9   ] = 9     ;
    tab[MSH_TRI_10  ] = 10    ;
    tab[MSH_TRI_12  ] = 12    ;
    tab[MSH_TRI_15  ] = 15    ;
    tab[MSH_TRI_21  ] = 21    ;
    tab[MSH_LIN_4   ] = 4     ;
    tab[MSH_LIN_5   ] = 5     ;
    tab[MSH_LIN_6   ] = 6     ;
    tab[MSH_TET_20  ] = 20    ;
    tab[MSH_TET_35  ] = 35    ;
    tab[MSH_TET_56  ] = 56    ;
    tab[MSH_TET_22  ] = 22    ;
    tab[MSH_TET_28  ] = 28    ;
    tab[MSH_QUA_16  ] = 16    ;
    tab[MSH_QUA_25  ] = 25    ;
    tab[MSH_QUA_36  ] = 36    ;
    tab[MSH_QUA_12  ] = 12    ;
    tab[MSH_QUA_20  ] = 20    ;
    tab[MSH_TRI_28  ] = 28    ;
    tab[MSH_TRI_36  ] = 36    ;
    tab[MSH_TRI_45  ] = 45    ;
    tab[MSH_TRI_55  ] = 55    ;
    tab[MSH_TRI_66  ] = 66    ;
    tab[MSH_QUA_49  ] = 49    ;
    tab[MSH_QUA_64  ] = 64    ;
    tab[MSH_QUA_81  ] = 81    ;
    tab[MSH_QUA_100 ] = 100   ;
    tab[MSH_QUA_121 ] = 121   ;
    tab[MSH_TRI_18  ] = 18    ;
    tab[MSH_TRI_24  ] = 24    ;
    tab[MSH_TRI_27  ] = 27    ;
    tab[MSH_TRI_30  ] = 30    ;
    tab[MSH_QUA_24  ] = 24    ;
    tab[MSH_QUA_28  ] = 28    ;
    tab[MSH_QUA_32  ] = 32    ;
    tab[MSH_QUA_40  ] = 40    ;
    tab[MSH_LIN_7   ] = 7     ;
    tab[MSH_LIN_8   ] = 8     ;
    tab[MSH_LIN_9   ] = 9     ;
    tab[MSH_LIN_10  ] = 10    ;
    tab[MSH_LIN_11  ] = 11    ;
    tab[MSH_TET_84  ] = 84    ;
    tab[MSH_TET_120 ] = 120   ;
    tab[MSH_TET_165 ] = 165   ;
    tab[MSH_TET_220 ] = 220   ;
    tab[MSH_TET_286 ] = 286   ;
    tab[MSH_TET_34  ] = 34    ;
    tab[MSH_TET_40  ] = 40    ;
    tab[MSH_TET_46  ] = 46    ;
    tab[MSH_TET_52  ] = 52    ;
    tab[MSH_TET_58  ] = 58    ;
    tab[MSH_LIN_1   ] = 1     ;
    tab[MSH_TRI_1   ] = 1     ;
    tab[MSH_QUA_1   ] = 1     ;
    tab[MSH_TET_1   ] = 1     ;
    tab[MSH_HEX_1   ] = 1     ;
    tab[MSH_PRI_1   ] = 1     ;
    tab[MSH_PRI_40  ] = 40    ;
    tab[MSH_PRI_75  ] = 75    ;
    tab[MSH_HEX_64  ] = 64    ;
    tab[MSH_HEX_125 ] = 125   ;
    tab[MSH_HEX_216 ] = 216   ;
    tab[MSH_HEX_343 ] = 343   ;
    tab[MSH_HEX_512 ] = 512   ;
    tab[MSH_HEX_729 ] = 729   ;
    tab[MSH_HEX_1000] = 1000  ;
    tab[MSH_HEX_32  ] = 32    ;
    tab[MSH_HEX_44  ] = 44    ;
    tab[MSH_HEX_56  ] = 56    ;
    tab[MSH_HEX_68  ] = 68    ;
    tab[MSH_HEX_80  ] = 80    ;
    tab[MSH_HEX_92  ] = 92    ;
    tab[MSH_HEX_104 ] = 104   ;
    tab[MSH_PRI_126 ] = 126   ;
    tab[MSH_PRI_196 ] = 196   ;
    tab[MSH_PRI_288 ] = 288   ;
    tab[MSH_PRI_405 ] = 405   ;
    tab[MSH_PRI_550 ] = 550   ;
    tab[MSH_PRI_24  ] = 24    ;
    tab[MSH_PRI_33  ] = 33    ;
    tab[MSH_PRI_42  ] = 42    ;
    tab[MSH_PRI_51  ] = 51    ;
    tab[MSH_PRI_60  ] = 60    ;
    tab[MSH_PRI_69  ] = 69    ;
    tab[MSH_PRI_78  ] = 78    ;
    tab[MSH_PYR_30  ] = 30    ;
    tab[MSH_PYR_55  ] = 55    ;
    tab[MSH_PYR_91  ] = 91    ;
    tab[MSH_PYR_140 ] = 140   ;
    tab[MSH_PYR_204 ] = 204   ;
    tab[MSH_PYR_285 ] = 285   ;
    tab[MSH_PYR_385 ] = 385   ;
    tab[MSH_PYR_21  ] = 21    ;
    tab[MSH_PYR_29  ] = 29    ;
    tab[MSH_PYR_37  ] = 37    ;
    tab[MSH_PYR_45  ] = 45    ;
    tab[MSH_PYR_53  ] = 53    ;
    tab[MSH_PYR_61  ] = 61    ;
    tab[MSH_PYR_69  ] = 69    ;
    tab[MSH_PYR_1   ] = 1     ;
    tab[MSH_TET_16  ] = 16    ;
    
    return tab;
  }
 

  static inline
  std::tr1::array<int, MSH_MAX_INDEX+1> dim_4_elm_T()
  {
    std::tr1::array<int, MSH_MAX_INDEX+1> tab;
    
    tab[MSH_LIN_2   ] = 1;
    tab[MSH_TRI_3   ] = 2;
    tab[MSH_QUA_4   ] = 2;
    tab[MSH_TET_4   ] = 3;
    tab[MSH_HEX_8   ] = 3;
    tab[MSH_PRI_6   ] = 3;
    tab[MSH_PYR_5   ] = 3;
    tab[MSH_LIN_3   ] = 1;
    tab[MSH_TRI_6   ] = 2;
    tab[MSH_QUA_9   ] = 2;
    tab[MSH_TET_10  ] = 3;
    tab[MSH_HEX_27  ] = 3;
    tab[MSH_PRI_18  ] = 3;
    tab[MSH_PYR_14  ] = 3;
    tab[MSH_PNT     ] = 0;
    tab[MSH_QUA_8   ] = 2;
    tab[MSH_HEX_20  ] = 3;
    tab[MSH_PRI_15  ] = 3;
    tab[MSH_PYR_13  ] = 3;
    tab[MSH_TRI_9   ] = 2;
    tab[MSH_TRI_10  ] = 2;
    tab[MSH_TRI_12  ] = 2;
    tab[MSH_TRI_15  ] = 2;
    tab[MSH_TRI_21  ] = 2;
    tab[MSH_LIN_4   ] = 1;
    tab[MSH_LIN_5   ] = 1;
    tab[MSH_LIN_6   ] = 1;
    tab[MSH_TET_20  ] = 3;
    tab[MSH_TET_35  ] = 3;
    tab[MSH_TET_56  ] = 3;
    tab[MSH_TET_22  ] = 3;
    tab[MSH_TET_28  ] = 3;
    tab[MSH_QUA_16  ] = 2;
    tab[MSH_QUA_25  ] = 2;
    tab[MSH_QUA_36  ] = 2;
    tab[MSH_QUA_12  ] = 2;
    tab[MSH_QUA_20  ] = 2;
    tab[MSH_TRI_28  ] = 2;
    tab[MSH_TRI_36  ] = 2;
    tab[MSH_TRI_45  ] = 2;
    tab[MSH_TRI_55  ] = 2;
    tab[MSH_TRI_66  ] = 2;
    tab[MSH_QUA_49  ] = 2;
    tab[MSH_QUA_64  ] = 2;
    tab[MSH_QUA_81  ] = 2;
    tab[MSH_QUA_100 ] = 2;
    tab[MSH_QUA_121 ] = 2;
    tab[MSH_TRI_18  ] = 2;
    tab[MSH_TRI_24  ] = 2;
    tab[MSH_TRI_27  ] = 2;
    tab[MSH_TRI_30  ] = 2;
    tab[MSH_QUA_24  ] = 2;
    tab[MSH_QUA_28  ] = 2;
    tab[MSH_QUA_32  ] = 2;
    tab[MSH_QUA_40  ] = 2;
    tab[MSH_LIN_7   ] = 1;
    tab[MSH_LIN_8   ] = 1;
    tab[MSH_LIN_9   ] = 1;
    tab[MSH_LIN_10  ] = 1;
    tab[MSH_LIN_11  ] = 1;
    tab[MSH_TET_84  ] = 3;
    tab[MSH_TET_120 ] = 3;
    tab[MSH_TET_165 ] = 3;
    tab[MSH_TET_220 ] = 3;
    tab[MSH_TET_286 ] = 3;
    tab[MSH_TET_34  ] = 3;
    tab[MSH_TET_40  ] = 3;
    tab[MSH_TET_46  ] = 3;
    tab[MSH_TET_52  ] = 3;
    tab[MSH_TET_58  ] = 3;
    tab[MSH_LIN_1   ] = 1;
    tab[MSH_TRI_1   ] = 2;
    tab[MSH_QUA_1   ] = 2;
    tab[MSH_TET_1   ] = 3;
    tab[MSH_HEX_1   ] = 3;
    tab[MSH_PRI_1   ] = 3;
    tab[MSH_PRI_40  ] = 3;
    tab[MSH_PRI_75  ] = 3;
    tab[MSH_HEX_64  ] = 3;
    tab[MSH_HEX_125 ] = 3;
    tab[MSH_HEX_216 ] = 3;
    tab[MSH_HEX_343 ] = 3;
    tab[MSH_HEX_512 ] = 3;
    tab[MSH_HEX_729 ] = 3;
    tab[MSH_HEX_1000] = 3;
    tab[MSH_HEX_32  ] = 3;
    tab[MSH_HEX_44  ] = 3;
    tab[MSH_HEX_56  ] = 3;
    tab[MSH_HEX_68  ] = 3;
    tab[MSH_HEX_80  ] = 3;
    tab[MSH_HEX_92  ] = 3;
    tab[MSH_HEX_104 ] = 3;
    tab[MSH_PRI_126 ] = 3;
    tab[MSH_PRI_196 ] = 3;
    tab[MSH_PRI_288 ] = 3;
    tab[MSH_PRI_405 ] = 3;
    tab[MSH_PRI_550 ] = 3;
    tab[MSH_PRI_24  ] = 3;
    tab[MSH_PRI_33  ] = 3;
    tab[MSH_PRI_42  ] = 3;
    tab[MSH_PRI_51  ] = 3;
    tab[MSH_PRI_60  ] = 3;
    tab[MSH_PRI_69  ] = 3;
    tab[MSH_PRI_78  ] = 3;
    tab[MSH_PYR_30  ] = 3;
    tab[MSH_PYR_55  ] = 3;
    tab[MSH_PYR_91  ] = 3;
    tab[MSH_PYR_140 ] = 3;
    tab[MSH_PYR_204 ] = 3;
    tab[MSH_PYR_285 ] = 3;
    tab[MSH_PYR_385 ] = 3;
    tab[MSH_PYR_21  ] = 3;
    tab[MSH_PYR_29  ] = 3;
    tab[MSH_PYR_37  ] = 3;
    tab[MSH_PYR_45  ] = 3;
    tab[MSH_PYR_53  ] = 3;
    tab[MSH_PYR_61  ] = 3;
    tab[MSH_PYR_69  ] = 3;
    tab[MSH_PYR_1   ] = 3;
    tab[MSH_TET_16  ] = 3;
    
    return tab;
  }
  
}

static inline
int numNodeForMshTag(EMshTag type)
{
  static const
  std::tr1::array<int, MSH_MAX_INDEX+1> nds = fi_MshTagsInitializers::n_nodes_4_elm_T();

  unsigned idx = static_cast<unsigned>(type);
  if (idx-1 >= MSH_MAX_INDEX)
  {
    ALELIB_CHECK(false, "invalid cell type", std::invalid_argument);
    return -1;
  }

  return nds[idx];
}


static inline
int dimForMshTag(EMshTag type)
{
  static const
  std::tr1::array<int, MSH_MAX_INDEX+1> dims = fi_MshTagsInitializers::dim_4_elm_T();

  unsigned idx = static_cast<unsigned>(type);
  if (idx-1 >= MSH_MAX_INDEX)
  {
    ALELIB_CHECK(false, "invalid cell type", std::invalid_argument);
    return -1;
  }

  return dims[idx];
}



} // end namespace


#endif
