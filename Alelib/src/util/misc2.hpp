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

#ifndef ALELIB_MISC2_HPP
#define ALELIB_MISC2_HPP

#include <tr1/cstdint>

/** calculate the log2 of a number.
 *  reference: http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogLookup
 */
static
int log2_i32(uint32_t v)
{
  static const char LogTable256[256] = 
  {
  #define LT(n) n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n
	    -1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
	    LT(4), LT(5), LT(5), LT(6), LT(6), LT(6), LT(6),
	    LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7)
  };
  
  //unsigned int v; // 32-bit word to find the log of
  register unsigned int t, tt; // temporaries
  
  if ((tt = (v >> 16)))
  {
    return (t = tt >> 8) ? 24 + LogTable256[t] : 16 + LogTable256[tt];
  }
  else 
  {
    return  (t = v >> 8) ? 8 + LogTable256[t] : LogTable256[v];
  }
  
}



static
int log2_i64(uint64_t v)
{
  static const char LogTable256[256] = 
  {
  #define LT(n) n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n
	    -1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
	    LT(4), LT(5), LT(5), LT(6), LT(6), LT(6), LT(6),
	    LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7)
  };
  
  register uint64_t t, tt; // temporaries
  
  if ((tt = (v >> 32)))
  {
    v = tt;
    if ((tt = (v >> 16)))
    {
      return  (t = tt >> 8) ? 56 + LogTable256[t] : 48 + LogTable256[tt];
    }
    else 
    {
      return  (t = v >> 8) ? 40 + LogTable256[t] : 32 + LogTable256[v];
    }
  }
  else
  if ((tt = (v >> 16)))
  {
    return  (t = tt >> 8) ? 24 + LogTable256[t] : 16 + LogTable256[tt];
  }
  else 
  {
    return  (t = v >> 8) ? 8 + LogTable256[t] : LogTable256[v];
  }
  
}



//static
//int log2_i128(__uint128_t v)
//{
//  static const char LogTable256[256] = 
//  {
//  #define LT(n) n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n
//	    -1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
//	    LT(4), LT(5), LT(5), LT(6), LT(6), LT(6), LT(6),
//	    LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7)
//  };
//  
//  register __uint128_t t, tt; // temporaries
//  
//  if ((tt = (v >> 64)))
//  {
//    v = tt;
//    if ((tt = (v >> 32)))
//    {
//      v = tt;
//      if ((tt = (v >> 16)))
//      {
//	return  (t = tt >> 8) ? 120 + LogTable256[t] : 112 + LogTable256[tt];
//      }
//      else 
//      {
//	return  (t = v >> 8) ? 104 + LogTable256[t] : 96 + LogTable256[v];
//      }
//    }
//    else
//    if ((tt = (v >> 16)))
//    {
//      return  (t = tt >> 8) ? 88 + LogTable256[t] : 80 + LogTable256[tt];
//    }
//    else 
//    {
//      return  (t = v >> 8) ? 72 + LogTable256[t] : 64 + LogTable256[v];
//    }
//  }
//  if ((tt = (v >> 32)))
//  {
//    v = tt;
//    if ((tt = (v >> 16)))
//    {
//      return  (t = tt >> 8) ? 56 + LogTable256[t] : 48 + LogTable256[tt];
//    }
//    else 
//    {
//      return  (t = v >> 8) ? 40 + LogTable256[t] : 32 + LogTable256[v];
//    }
//  }
//  else
//  if ((tt = (v >> 16)))
//  {
//    return  (t = tt >> 8) ? 24 + LogTable256[t] : 16 + LogTable256[tt];
//  }
//  else 
//  {
//    return  (t = v >> 8) ? 8 + LogTable256[t] : LogTable256[v];
//  }
//  
//}





class fi_RemoveWarningMisc2 {
  fi_RemoveWarningMisc2() {log2_i32(0); log2_i64(0); /* log2_i128(0);*/ };
};








#endif

