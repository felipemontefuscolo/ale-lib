// This file is part of generic_array, A lightweight generic
// N-dimensional array library
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef MA_LISTIFY_HPP
#define MA_LISTIFY_HPP

#include <cstddef>

namespace marray {


namespace details
{

template<class T, int N> struct ListInit{};

// expand to `type a0, type a1, type a2, ... `
#define MA_EXPAND_ARGS1(type_)                          type_ a0
#define MA_EXPAND_ARGS2(type_)  MA_EXPAND_ARGS1(type_), type_ a1
#define MA_EXPAND_ARGS3(type_)  MA_EXPAND_ARGS2(type_), type_ a2
#define MA_EXPAND_ARGS4(type_)  MA_EXPAND_ARGS3(type_), type_ a3
#define MA_EXPAND_ARGS5(type_)  MA_EXPAND_ARGS4(type_), type_ a4
#define MA_EXPAND_ARGS6(type_)  MA_EXPAND_ARGS5(type_), type_ a5
#define MA_EXPAND_ARGS7(type_)  MA_EXPAND_ARGS6(type_), type_ a6
#define MA_EXPAND_ARGS8(type_)  MA_EXPAND_ARGS7(type_), type_ a7
#define MA_EXPAND_ARGS9(type_)  MA_EXPAND_ARGS8(type_), type_ a8
#define MA_EXPAND_ARGS10(type_) MA_EXPAND_ARGS9(type_), type_ a9

#define MA_EXPAND_ARGS_(N, type_) MA_EXPAND_ARGS##N(type_)
#define MA_EXPAND_ARGS(N, type_) MA_EXPAND_ARGS_(N, type_)

// expand to `v[0]=a0; v[1]=a1; v[2]=a2; ...`
#define MA_EXPAND_SEQ1                 v[0]=a0;
#define MA_EXPAND_SEQ2  MA_EXPAND_SEQ1 v[1]=a1;
#define MA_EXPAND_SEQ3  MA_EXPAND_SEQ2 v[2]=a2;
#define MA_EXPAND_SEQ4  MA_EXPAND_SEQ3 v[3]=a3;
#define MA_EXPAND_SEQ5  MA_EXPAND_SEQ4 v[4]=a4;
#define MA_EXPAND_SEQ6  MA_EXPAND_SEQ5 v[5]=a5;
#define MA_EXPAND_SEQ7  MA_EXPAND_SEQ6 v[6]=a6;
#define MA_EXPAND_SEQ8  MA_EXPAND_SEQ7 v[7]=a7;
#define MA_EXPAND_SEQ9  MA_EXPAND_SEQ8 v[8]=a8;
#define MA_EXPAND_SEQ10 MA_EXPAND_SEQ9 v[9]=a9;

#define MA_EXPAND_SEQ_(N) MA_EXPAND_SEQ##N
#define MA_EXPAND_SEQ(N) MA_EXPAND_SEQ_(N)

// N = num of args
#define MA_IMPL_LISTAUX(N)                                  \
    template<class T>                                       \
    struct ListInit<T,N>                                    \
    {                                                       \
      T v[N];                                               \
      inline ListInit(MA_EXPAND_ARGS(N, T))                 \
      {                                                     \
        MA_EXPAND_SEQ(N)                                    \
      };                                                    \
      typedef std::size_t size_type;                        \
      typedef T* pointer;                                   \
      typedef T const* const_pointer;                       \
      typedef T& reference;                                 \
      typedef T const& const_reference;                     \
                                                            \
      inline operator pointer () {return v;}                \
      inline operator const_pointer () const {return v;}    \
      inline reference operator[](size_type i)              \
      { return v[i]; }                                      \
      inline const_reference operator[](size_type i) const  \
      { return v[i]; }                                      \
      inline pointer operator() ()                          \
      { return v; }                                         \
      inline const_pointer operator() () const              \
      { return v; }                                         \
    };


MA_IMPL_LISTAUX( 1)
MA_IMPL_LISTAUX( 2)
MA_IMPL_LISTAUX( 3)
MA_IMPL_LISTAUX( 4)
MA_IMPL_LISTAUX( 5)
MA_IMPL_LISTAUX( 6)
MA_IMPL_LISTAUX( 7)
MA_IMPL_LISTAUX( 8)
MA_IMPL_LISTAUX( 9)
MA_IMPL_LISTAUX(10)



} // end namespace details

#define MA_IMPL_LISTOF(N)                                  \
                                                           \
template<class T>                                          \
details::ListInit<T,N> listify(MA_EXPAND_ARGS(N, T))       \
{                                                          \
  return details::ListInit<T,N>(MA_EXPAND_ARGS(N, (T) ));  \
}

MA_IMPL_LISTOF( 1)
MA_IMPL_LISTOF( 2)
MA_IMPL_LISTOF( 3)
MA_IMPL_LISTOF( 4)
MA_IMPL_LISTOF( 5)
MA_IMPL_LISTOF( 6)
MA_IMPL_LISTOF( 7)
MA_IMPL_LISTOF( 8)
MA_IMPL_LISTOF( 9)
MA_IMPL_LISTOF(10)


#undef MA_IMPL_LISTOF


#undef MA_IMPL_LISTAUX
#undef MA_EXPAND_SEQ
#undef MA_EXPAND_SEQ_
#undef MA_EXPAND_SEQ1
#undef MA_EXPAND_SEQ2
#undef MA_EXPAND_SEQ3
#undef MA_EXPAND_SEQ4
#undef MA_EXPAND_SEQ5
#undef MA_EXPAND_SEQ6
#undef MA_EXPAND_SEQ7
#undef MA_EXPAND_SEQ8
#undef MA_EXPAND_SEQ9
#undef MA_EXPAND_SEQ10
#undef MA_EXPAND_ARGS1
#undef MA_EXPAND_ARGS2
#undef MA_EXPAND_ARGS3
#undef MA_EXPAND_ARGS4
#undef MA_EXPAND_ARGS5
#undef MA_EXPAND_ARGS6
#undef MA_EXPAND_ARGS7
#undef MA_EXPAND_ARGS8
#undef MA_EXPAND_ARGS9
#undef MA_EXPAND_ARGS10



} // end namespace

#endif




