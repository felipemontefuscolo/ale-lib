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

#ifndef ALELIB_LABELABLE_HPP
#define ALELIB_LABELABLE_HPP

#include "../util/assert.hpp"
#include "enums.hpp"
#include <stdint.h>



namespace alelib
{

template<class,class> class SeqList;

enum Flags
{
  NO_FLAG = 0,
  DISABLED = (1<<0),
  MARKED   = (1<<1),
  VISITED  = (1<<2),
  BLOCKED  = (1<<3)
};
static const int NO_TAG = 0;


class Labelable
{
  template<class,class> friend class SeqList;
  
  template<ECellType> friend class Mesh;
  
  // poderia usar bit-field, mas não é portável
  int8_t  m_tag; // 0 a 256
  uint8_t m_flags; // 8 flags ...

public:
  enum {tag_size = 256,
       flags_size = 256};

  enum Masks {
    mk_none      = NO_FLAG,
    mk_disabled  = DISABLED,
    mk_marked    = MARKED,
    mk_visited   = VISITED,
    mk_blocked   = BLOCKED
  };

protected:
  explicit Labelable(int8_t tag, Flags flags=NO_FLAG) : m_tag(tag), m_flags(flags)
  {
    //ALELIB_CHECK((m_tag>=0)&&(m_tag<tag_size), "tag number must be less than "+itos(tag_size)+" and greater than 0", std::out_of_range);
    //ALELIB_CHECK((m_flags>=0)&&(m_flags<flags_size), "wrong flags", std::out_of_range);
  }

  Labelable() : m_tag(NO_TAG), m_flags(NO_FLAG) {};

public:

  int getTag() const
  { return m_tag; }

  void setTag(int8_t tag)
  {
    //ALELIB_CHECK(unsigned(tag)<tag_size, "tag number must be less or equal "+itos(tag_size), std::out_of_range);
    m_tag = tag;
  }

public:

  bool isDisabled() const
  { return m_flags & mk_disabled; }

protected:
  void setDisabledTo(bool disable_this)
  {
    m_flags = disable_this ? (m_flags | mk_disabled) : (m_flags & (~mk_disabled))  ;
  }

public:
  bool isMarked() const
  { return m_flags & mk_marked; }

  void setMarkedTo(bool mark_this)
  { m_flags = mark_this ? (m_flags | mk_marked) : (m_flags & (~mk_marked))  ; }

  bool isVisited() const
  { return m_flags & mk_visited; }

  void setVisitedTo(bool visit)
  { m_flags =  visit ? (m_flags | mk_visited) : (m_flags & (~mk_visited))  ; }

  bool isBlocked() const
  { return m_flags & mk_blocked; }

  void setBlockedTo(bool visit)
  { m_flags =  visit ? (m_flags | mk_blocked) : (m_flags & (~mk_blocked))  ; }

  bool getFlag(unsigned flag_no) const
  { return static_cast<bool>(m_flags & ( 1 << flag_no)); }

  Flags getFlags() const
  { return static_cast<Flags>(m_flags); }

protected:
  void setFlag(int flag_no, bool set=true)
  {
    unsigned char const one = static_cast<unsigned char>(1);
    m_flags =  set ? (m_flags | (one<<flag_no)) : (m_flags & (~(one<<flag_no)))  ;
  }

  void setFlags(Flags flags)
  {  m_flags = static_cast<uint8_t>( flags ); }

  //inline void printFlags() const
  //{
    //for (unsigned i=0; i<sizeof(m_flags)*8; ++i)
    //{
      //std::cout << static_cast<bool>(m_flags & (1<<i));
    //}
    //std::cout << std::endl;
  //}


};


} // end namespace alelib

#endif

