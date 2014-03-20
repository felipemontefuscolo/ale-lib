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

#ifndef ALELIB_CHECK_HPP
#define ALELIB_CHECK_HPP

#include <stdexcept>
#include <string>
#include "../io/io_aux.hpp"
#include "macros.hpp"

namespace alelib
{

template<class Exception>
void assertion_failed(std::string const& expr, std::string const& msg);


// see boost
#ifdef ALELIB_DEBUG_ON
  #define ALELIB_CHECK(ok, mesg, except) if(!(ok)) \
                                         assertion_failed<except>(#ok, mesg, __LINE__, __FILE__, __PRETTY_FUNCTION__)
                                                 
#else
  #define ALELIB_CHECK(ok,msg, except) ((void)0)
#endif

#define ALELIB_ASSERT(ok, mesg, except) if(!(ok)) \
                                          assertion_failed<except>(#ok, mesg, __LINE__, __FILE__, __PRETTY_FUNCTION__)


template<class Exception>
void assertion_failed(std::string const& expr, std::string const& msg, int Line, const char* File, const char* PrettyFunction)
{
  std::string what_arg ="\nERROR: "+std::string(File)+":"+alelib::itos(Line)+": "+msg+"\n"
                          "ERROR: "+std::string(File)+":"+alelib::itos(Line)+": assertion '"+expr+"' failed\n"
                          "ERROR: "+std::string(File)+": in '"+std::string(PrettyFunction)+"' \n";
  throw Exception(what_arg);
}


// STATIC ASSERTIONS

template<bool> struct CompileTimeAssert;
template<> struct CompileTimeAssert
<true> { };
#define ALE_STATIC_ASSERT(e) \
	(CompileTimeAssert <(e) != 0>())

template<bool> struct MUST_BE_A_RANDOM_ACCESS_ITERATOR;
template<> struct MUST_BE_A_RANDOM_ACCESS_ITERATOR
<true> { };
#define ALE_STATIC_ASSERT_ITERATOR(e) \
	(MUST_BE_A_RANDOM_ACCESS_ITERATOR <(e) != 0>())



template<int> struct CompileTimeError;
template<> struct CompileTimeError<true> {};

#define ALE_STATIC_CHECK(expr, msg) \
    { CompileTimeError<((expr) != 0)> ERROR_##msg; (void)ERROR_##msg; } 





} // end namespace

#endif

