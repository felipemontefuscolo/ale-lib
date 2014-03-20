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

#ifndef ALELIB_IO_AUX_HPP
#define ALELIB_IO_AUX_HPP

#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <cstdio>
#include <string>

namespace alelib
{
  

/** @brief Searches the stream for a key-word and return the position after
 *  the key-word. If the key-word was not found, return ULONG_MAX.
 *  @param word the key-word.
 *  @param word_size size of the key-word.
 *  @param fp the stream.
 *  @note a key-word is a word that is at the beginning of a line.
 */
static inline
long int find_keyword(const char* word, int word_size, FILE *fp)
{
  char buff[256];
  
  while (fgets(buff, sizeof(buff), fp) && !feof(fp))
  {
    if(!strncmp(buff, word, word_size)) // if they are equal
    {
      return ftell(fp);
    }
  }
  
  return -1;
}

/**
 * C++ version 0.4 std::string style "itoa":
 * Contributions from Stuart Lowe, Ray-Yuan Sheu,
 * Rodrigo de Salvo Braz, Luc Gallant, John Maloney
 * and Brian Hunt
 * 
 * modified: Felipe Montefuscolo
 */
static inline std::string itos(int value)
{

	std::string buf;

	enum { kMaxDigits = 35, base=10 };
	buf.reserve( kMaxDigits ); // Pre-allocate enough space.

	int quotient = value;

	// Translating number to string with base:
	do {
		buf += "0123456789"[ std::abs( quotient % base ) ];
		quotient /= base;
	} while ( quotient );

	// Append the negative sign
	if ( value < 0) buf += '-';

	std::reverse( buf.begin(), buf.end() );
	return buf;
}

static inline std::string itoafill0(int value, int fill)
{
	std::string buf;

	enum { kMaxDigits = 35, base=10 };
	buf.reserve( kMaxDigits ); // Pre-allocate enough space.

  fill = fill < kMaxDigits ? fill : kMaxDigits;

	int quotient = value;

	// Translating number to string with base:
	do {
		buf += "0123456789"[ std::abs( quotient % base ) ];
		quotient /= base;
	} while ( quotient );
  
  int i(fill - buf.size());
  
  while(i>0)
  {
    buf += '0';
    i=fill - buf.size();
  }
  
  std::reverse( buf.begin(), buf.end() );
  return buf;
}

// remove espaços finais de strings
static inline std::string stripTrailingSpaces(std::string name)
{
  //if (name.empty())
  //  return name;
  while(!name.empty() && *(name.end()-1) == ' ')
    name.erase(name.end()-1);
  return name;
}

/** @example <tt>user/file.dat</tt> returns <tt>user/</tt>
 *  @note aassumes that is a valid file name.
 */ 
static inline std::string getRelativePath(std::string const& name)
{
  size_t bar = name.rfind("/");
  
  if (bar==std::string::npos)
    return "./";
  else
    return name.substr(0,bar+1);
}

/** @example <tt>user/file.dat</tt> returns <tt>.dat</tt>
 *  @note assumes that is a valid file name.
 */ 
static inline std::string getExtension(std::string const& name)
{
  int size = name.size();
  if (size==0)
    return name;
  
  size_t dot = name.rfind(".");
  size_t bar = name.rfind("/");
  if (dot==std::string::npos)
    return "";
  else if ((bar==dot-1) || (dot==0) || (name.at(dot-1)=='.')  || 
           ((bar!=std::string::npos) && (bar>dot)) || (dot==name.size()-1) )
    return "";
  else 
    return name.substr(dot);
}

/** @example <tt>user/file.dat</tt> returns <tt>file</tt>
 *  @note assumes that is a valid file name.
 */ 
static inline std::string getBaseName(std::string name)
{
  size_t bar = name.rfind("/");
  int npath;
  
  if (bar==std::string::npos)
    npath=0;
  else
    npath = name.substr(0,bar+1).size();

  int nexte = getExtension(name).size();
  
  for (int i = 0; i < nexte; ++i)
    name.erase(name.end()-1);
  for (int i = 0; i < npath; ++i)
    name.erase(name.begin());
  
  return name;
}

  
  
  
  
  
}


#endif


