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

#ifndef ALELIB_MISC_HPP
#define ALELIB_MISC_HPP

#include <cstdio>
#include <string>

  /* vetores para representar os espaços R² e R³ são os do Eigen*/

namespace alelib {
  
  
  template <class T>
  const T& max ( const T& a) {
    return a;
  }

  //template <class T, class ... O>
  //const T& max ( const T& a, const O& ... b ) {
    //auto& c(max(b...));
    //return (c<a)?a:c;
  //}



template<bool Bool, typename CaseTrue, typename CaseFalse>
struct IfThenElse;

template<typename CaseTrue, typename CaseFalse>
struct IfThenElse<true, CaseTrue, CaseFalse> {
  typedef CaseTrue type;
};
template<typename CaseTrue, typename CaseFalse>
struct IfThenElse<false, CaseTrue, CaseFalse> {
  typedef CaseFalse type;
};


template<bool, typename T = void> 
struct EnableIf {};

template<typename T>
struct EnableIf<true, T> {
  typedef T type;
};

/** função parecida com a copy do STL. \n
 * copia o conteúdo de uma sequência `a' para outra `c' no intervalo [c_begin, c_end).
 * @param a_begin iterador na posição inicial da sequência `a'
 * @param c_begin iterador na posição inicial da sequência `c' (c de cópia)
 * @param c_end iterador na posição final da seguência (excluindo o último elemento)
 */
template<class In_it, class Out_it>
inline
void copy_from(In_it a_begin, Out_it c_begin, Out_it c_end)
{
  while(c_begin != c_end) *c_begin++ = *a_begin++;
}



/** Checks if two vectors are cyclically equal.
 * @param beg1 iterator to beginning of first vector.
 * @param end1 iterator to end of fisrt vector.
 * @param beg2 iterator to beginning of second vector.
 * @param end2 iterator to end of second vector.
 * @return a <EM>bool</EM>: if the vectors are C. eq. or not.
 * @note each vector must contain unique elements, i.e., elements cannot repeat.
 * @note vectors must have at least one element.
 */ 
template<class Iterator1, class Iterator2>
bool arrayIsCyclicallyEqual(Iterator1 beg1, Iterator1 end1, Iterator2 beg2, Iterator2 end2)
{
  Iterator1 beg1_i = beg1;
  Iterator2 beg2_i = beg2;
  
  while (beg2 != end2)
  {
    if (*beg2 == *beg1)
      break;
    ++beg2;  
  }
  if (beg2==end2) return false;
  
  Iterator2 mid = beg2;
  
  // cyclic verification
  while (++beg1 != end1)
  {
    if (++beg2 == end2)
      beg2 = beg2_i;
    if (*beg1 != *beg2)
      break;
  }
  if (beg1 == end1) return true;
  
  // anti-cyclic verification
  beg1 = beg1_i;
  beg2 = mid;
  while (++beg1 != end1)
  {
    if (beg2 == beg2_i)
      beg2 = end2 - 1;
    else
      --beg2;
    if (*beg1 != *beg2)
      return false;
  }
  return true;
}


/** Checks if tow vectors have the same elements.
 * @param beg1 iterator to beginning of first vector.
 * @param end1 iterator to end of fisrt vector.
 * @param beg2 iterator to beginning of second vector.
 * @param end2 iterator to end of second vector.
 * @return a <EM>bool</EM>: if the vectors have the same elements.
 * @note each vector must contain unique elements, i.e., elements cannot repeat.
 * @note vectors must have at least one element and they must have the same size.
 */ 
template<class Iterator1, class Iterator2>
bool sameElements(Iterator1 beg1, Iterator1 end1, Iterator2 beg2, Iterator2 end2)
{
  Iterator2 beg2_ini = beg2;
  for (; beg1 != end1; ++beg1)
  {
    for (; ; )
    {
      if (*beg1 == *beg2)
        break;
      ++beg2;
      if (beg2 == end2)
        return false;
    }
    beg2 = beg2_ini;
  }
  return true;
}

/** @brief Checks if a range [first, last) has the value.
 */
template<typename Iterator, typename Type>
bool checkValue(Iterator first, Iterator last, Type val)
{
  while(first != last)
  {
    if (*first++ == val)
      return true;
  }
  return false;
}

/** Retorna o n-ésimo bit da constante c passada.
 */
template<class T>
inline bool get_bit(T const& c, unsigned bitoffset)
{
    T bitmask = ( 1 << bitoffset) ;
    return ((c & bitmask)!=0) ? 1 : 0;
}

/** Seta o n-ésimo bit da constante c passada.
 */
template<class T>
inline void set_bit(T & c, unsigned bitoffset)
{
    T bitmask = 1 << bitoffset;
    c = (c | bitmask);
}

/** Zera o n-ésimo bit da constante c passada.
 */
template<class T>
inline void unset_bit(T & c, unsigned bitoffset)
{
    T bitmask = 1 << bitoffset;
    c = (c & (~bitmask));
}


/** @brief Searches the stream for a key-word and return the position after
 *  the key-word. If the key-word was not found, return ULONG_MAX.
 *  @param word the key-word.
 *  @param word_size size of the key-word.
 *  @param fp the stream.
 *  @note a key-word is a word that is at the beginning of a line.
 */ 
long int find_keyword(const char* word, int word_size, FILE *fp);


/**
 * C++ version 0.4 std::string style "itoa":
 * Contributions from Stuart Lowe, Ray-Yuan Sheu,
 * Rodrigo de Salvo Braz, Luc Gallant, John Maloney
 * and Brian Hunt
 * 
 * modified: Felipe Montefuscolo
 */
std::string itos(int value);

std::string itoafill0(int value, int fill);

// remove espaços finais de strings
std::string stripTrailingSpaces(std::string name);

/** @example <tt>user/file.dat</tt> returns <tt>user/</tt>
 *  @note aassumes that is a valid file name.
 */ 
std::string getRelativePath(std::string const& name);

/** @example <tt>user/file.dat</tt> returns <tt>.dat</tt>
 *  @note assumes that is a valid file name.
 */ 
std::string getExtension(std::string const& name);

/** @example <tt>user/file.dat</tt> returns <tt>file</tt>
 *  @note assumes that is a valid file name.
 */ 
std::string getBaseName(std::string name);

} // end namespace

#endif
