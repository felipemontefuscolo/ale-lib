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

#ifndef ALELIB_MISC2_HPP
#define ALELIB_MISC2_HPP

#include <ciso646>  // detect std::lib
#ifdef _LIBCPP_VERSION
// using libc++
#define MULTI_HAVE_TYPE_TRAITS
#else
// using libstdc++
#define MULTI_HAVE_TR1_TYPE_TRAITS
#endif

#ifdef MULTI_HAVE_TYPE_TRAITS
#include <type_traits>
namespace Tr1 = std;
#else
#include <tr1/type_traits>
namespace Tr1 = std::tr1;

#endif

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
  unsigned int t, tt; // temporaries
  
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
  
  uint64_t t, tt; // temporaries
  
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

