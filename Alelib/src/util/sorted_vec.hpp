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


#ifndef ALELIB_SORTED_VEC_HPP
#define ALELIB_SORTED_VEC_HPP

#include <algorithm>
#include <utility>

// SORTED VECTOR OPERATIONS (FOR INTEGERS)


namespace Sorted {

/** insere um valor e retorna um iterator para esse valor
 */  
template<class V, class T>
static
typename V::iterator insert(V &vec, T value)
{
  typename V::iterator it = std::lower_bound(vec.begin(), vec.end(), value);
  return vec.insert(it, value);
}

template<class V>
static
void pop_back(V &vec)
{
  vec.pop_back();
}

template<class V>
static
int& back(V &vec)
{
  return vec.back();
}

template<class V>
static
int& front(V &vec)
{
  return vec.front();
}

template<class V, class T>
static
int pop_closest_to(V &vec, T value)
{
  typename V::iterator it = std::lower_bound(vec.begin(), vec.end(), value);
  int erased;
  if (it != vec.begin())
  {
    if (it == vec.end())
      erased = *(--it);
    else
      erased = *it-value < value-*(it-1) ? *it : *(--it);
    vec.erase(it);
    return erased;
  }
  erased = *it;
  vec.erase(it);
  return erased;
}

/// retorna quantos números estão abaixo do valor passado.
template<class V, class T>
static
int countIfLess(V &vec, T value)
{
  return static_cast<int>(  std::lower_bound(vec.begin(), vec.end(), value) - vec.begin());
}

// suffix,mem_fun_name, mem_fun_return, qualif, mem_fun_args
ALE_BUILD_MEM_FUN_CHECKER_1ARG(reserve,reserve, void, ;char none_qualif , typename T::size_type);


template<class V>
static
void reserve(V &vec, typename V::size_type amount,
						 typename EnableIf<!TypeHas_reserve<V>::value>::type * = NULL)
{

}

template<class V>
void reserve(V &vec, typename V::size_type amount,
             typename EnableIf<TypeHas_reserve<V>::value>::type * = NULL)
{
  vec.reserve(amount);
}

}

// ====================================================================== //
//                                                                        // 
//                Map Vector Routines  (vector<pair<x,y>>)                //   
//                                                                        //
// ====================================================================== //


template<class PairT>
struct pair_key_less {
  
  typedef typename PairT::first_type Key;
  
  bool operator() (PairT const& a, Key const& b)
    { return a.first<b; };
  
  bool operator() (Key const& a, PairT const& b)
    { return a<b.first; };
};

template<class PairT>
struct pair_less {
  
  typedef typename PairT::first_type Key;
  
  bool operator() (PairT const& a, PairT const& b)
    { return a.first < b.first; };
  
  bool operator() (PairT const& a, Key const& b)
    { return a.first < b; };
  
  bool operator() (Key const& a, PairT const& b)
    { return a < b.first; };
  
};

template<class PairT>
struct pair_eq {

  typedef typename PairT::first_type Key;
  
  bool operator() (PairT const& a, PairT const& b)
    { return a.first == b.first; };
  
  bool operator() (PairT const& a, Key const& b)
    { return a.first == b; };
  
  bool operator() (Key const& a, PairT const& b)
    { return a == b.first; };
  
};

template <class ForwardIterator, class Type, class LessComp>
static
ForwardIterator binary_find(ForwardIterator first, ForwardIterator last, const Type& val, LessComp comp)
{
  first = std::lower_bound(first, last, val, comp);
  return (first == last || val!=(*first)) ? last : first;  
}

template <class ForwardIterator, class Type, class LessComp, class EqComp>
static
ForwardIterator binary_find(ForwardIterator first, ForwardIterator last, const Type& val, LessComp comp, EqComp eq)
{
  first = std::lower_bound(first, last, val, comp);
  return (first != last && eq(val,*first)) ? first : last; 
}




#endif


