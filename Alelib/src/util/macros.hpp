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

#ifndef ALELIB_MACROS_HPP
#define ALELIB_MACROS_HPP

#include "conf/directives.hpp"

/* utils */
#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember)) 




/* @brief this macro create a class that checks whether a class has
 * a specific member function.
 * usage:
 *      "mem_fun_name"   = name os member function
 *      "mem_fun_return" = which type this mem fun return
 *      "... "           = arguments of mem fun
 * macro will create a class called "TypeHas_mem_fun_name"
 * 
 * @example:
 * suppose the following mem fun:
 *    int sum(double x, double y)
 * 
 * So the macro takes the form:
 * 
 *     ALE_BUILD_MEM_FUN_CHECKER(sum, double, double);
 * 
 * then it will create a class named "TypeHas_sum".
 * 
 * To check if a class Foo has this mem function, you should do
 * 
 *      TypeHas_sum<Foo>::value
 * 
 * reference: http://stackoverflow.com/questions/257288/is-it-possible-to-write-a-c-template-to-check-for-a-functions-existence
 */ 

// this is the best version, but is not supported by some compiler .. sad ..
// DO NOT DELETE IT!!
/* 
          #define ALE_BUILD_MEM_FUN_CHECKER(suffix, mem_fun_name, mem_fun_return, qualif, ...)    \
            template <class Type>                                                 \
            class TypeHas_##suffix                                                \
            {                                                                     \
                template <typename T, T> struct TypeCheck;                        \
                                                                                  \
                typedef char Yes;                                                 \
                typedef long No;                                                  \
                                                                                  \
                template <typename T> struct Aux                                  \
                {                                                                 \
                    typedef mem_fun_return (T::*fptr)(__VA_ARGS__) qualif;        \
                };                                                                \
                                                                                  \
                template <typename T> static Yes check(TypeCheck< typename Aux<T>::fptr,  \
                                                       &T::mem_fun_name >*);              \
                template <typename T> static No  check(...);                              \
                                                                                          \
            public:                                                                       \
                static bool const value = (sizeof(check<Type>(0)) == sizeof(Yes));        \
            }

*/

/* about ALE_BUILD_MEM_FUN_CHECKER:
 * 
 * struct TypeCheck:This type won't compile if the second template parameter isn't of type T,
 *        so I can put a function pointer type in the first parameter and the function
 *        itself in the second thus checking that the function has a specific signature.
 * struct AuxA helper struct to hold the declaration of the function pointer.
 *        Change it if the function signature changes.
 */ 





/*
 * 
 * 
 * 
 * 
 *  variants of ALE_BUILD_MEM_FUN_CHECKER in standard C++ 
 * 
 * 
 * 
 * 
 * 
 * */ 

// suffix,mem_fun_name, mem_fun_return, qualif, mem_fun_args
#define ALE_BUILD_MEM_FUN_CHECKER_0ARG(suffix, mem_fun_name, mem_fun_return, qualif)    \
  template <class Type>                                                 \
  class TypeHas_##suffix                                                \
  {                                                                     \
      template <typename T, T> struct TypeCheck;                        \
                                                                        \
      typedef char Yes;                                                 \
      typedef long No;                                                  \
                                                                        \
      template <typename T> struct Aux                                  \
      {                                                                 \
          typedef mem_fun_return (T::*fptr)() qualif;        \
      };                                                                \
                                                                        \
      template <typename T> static Yes check(TypeCheck< typename Aux<T>::fptr,  \
                                             &T::mem_fun_name >*);              \
      template <typename T> static No  check(...);                              \
                                                                                \
  public:                                                                       \
      static bool const value = (sizeof(check<Type>(0)) == sizeof(Yes));        \
  }

// suffix,mem_fun_name, mem_fun_return, qualif, mem_fun_args
#define ALE_BUILD_MEM_FUN_CHECKER_1ARG(suffix, mem_fun_name, mem_fun_return, qualif, arg1)    \
  template <class Type>                                                 \
  class TypeHas_##suffix                                                \
  {                                                                     \
      template <typename T, T> struct TypeCheck;                        \
                                                                        \
      typedef char Yes;                                                 \
      typedef long No;                                                  \
                                                                        \
      template <typename T> struct Aux                                  \
      {                                                                 \
          typedef mem_fun_return (T::*fptr)(arg1) qualif;        \
      };                                                                \
                                                                        \
      template <typename T> static Yes check(TypeCheck< typename Aux<T>::fptr,  \
                                             &T::mem_fun_name >*);              \
      template <typename T> static No  check(...);                              \
                                                                                \
  public:                                                                       \
      static bool const value = (sizeof(check<Type>(0)) == sizeof(Yes));        \
  }





#ifdef __GNUC__
  #define ALE_GNUC_AT_LEAST(x,y) ((__GNUC__==x && __GNUC_MINOR__>=y) || __GNUC__>x)
#else
  #define ALE_GNUC_AT_LEAST(x,y) 0
#endif

#if (defined _MSC_VER) || (defined __INTEL_COMPILER)
#define ALE_STRONG_INLINE __forceinline
#else
#define ALE_STRONG_INLINE inline
#endif

#if ALE_GNUC_AT_LEAST(4,0)
#define ALE_ALWAYS_INLINE __attribute__((always_inline)) inline
#else
#define ALE_ALWAYS_INLINE ALE_STRONG_INLINE
#endif








#endif



