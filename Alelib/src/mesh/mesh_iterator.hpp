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


#ifndef ALELIB_MESH_ITERATORS_HPP
#define ALELIB_MESH_ITERATORS_HPP

#include <iterator>

namespace alelib
{

template<ECellType T>
class Mesh;

// EntityType = Cell, Facet, Corner or Point
template<class Iterator_T, class Container_T, class Mesh_T>
class MeshIterator
{
protected:  

  template<ECellType Z>
  friend class Mesh;

  typedef Iterator_T      SeqListIter;
  typedef MeshIterator    Self;
  typedef Mesh_T*         MeshPtr;
  typedef Container_T     ContainerType;
  
public:
  
  //typedef typename long                  difference_type;
  typedef typename SeqListIter::difference_type   difference_type;
  typedef typename SeqListIter::iterator_category iterator_category;  
  typedef typename SeqListIter::value_type        value_type;
  typedef typename SeqListIter::pointer           pointer;
  typedef typename SeqListIter::reference         reference;

protected:
  MeshPtr     m_mesh_ptr;
  SeqListIter m_seq_iter;

public:
  
  template<class Iter_T>
  MeshIterator(MeshPtr, Iter_T it) : m_seq_iter(it) {}
  
  MeshIterator() {}

  // Allow iterator to const_iterator conversion
  template<class Iter_T>
  MeshIterator(const MeshIterator<Iter_T,Container_T, Mesh_T> & i)
              : m_seq_iter(i.base()) { }  

  SeqListIter const& base() const
  { return m_seq_iter; }

public:

  difference_type index() const
  { return m_seq_iter.index(); }

  reference
  operator*() const
  { return *m_seq_iter; }

  pointer
  operator->() const
  { return &operator*(); }

  Self&
  operator++()
  {
    ++m_seq_iter;
    return *this;
  }

  Self
  operator++(int)
  { return Self(m_seq_iter++); }

  // Bidirectional iterator requirements
  Self&
  operator--()
  {
    --m_seq_iter;
    return *this;
  }

  Self
  operator--(int)
  { return Self(m_seq_iter--); }

};

// Forward iterator requirements
template<class IteratorL_T, class IteratorR_T, class Container_T, class Mesh_T>
  inline bool
  operator==(const MeshIterator<IteratorL_T, Container_T, Mesh_T>& lhs,
             const MeshIterator<IteratorR_T, Container_T, Mesh_T>& rhs)
  { return lhs.base() == rhs.base(); }

template<class Iterator_T, class Container_T, class Mesh_T>
  inline bool
  operator==(const MeshIterator<Iterator_T, Container_T, Mesh_T>& lhs,
             const MeshIterator<Iterator_T, Container_T, Mesh_T>& rhs)
  { return lhs.base() == rhs.base(); }

template<class IteratorL_T, class IteratorR_T, class Container_T, class Mesh_T>
  inline bool
  operator!=(const MeshIterator<IteratorL_T, Container_T, Mesh_T>& lhs,
             const MeshIterator<IteratorR_T, Container_T, Mesh_T>& rhs)
  { return lhs.base() != rhs.base(); }

template<class Iterator_T, class Container_T, class Mesh_T>
  inline bool
  operator!=(const MeshIterator<Iterator_T, Container_T, Mesh_T>& lhs,
             const MeshIterator<Iterator_T, Container_T, Mesh_T>& rhs)
  { return lhs.base() != rhs.base(); }

// Random access iterator requirements
template<class IteratorL_T, class IteratorR_T, class Container_T, class Mesh_T>
  inline bool
  operator<(const MeshIterator<IteratorL_T, Container_T, Mesh_T>& lhs,
            const MeshIterator<IteratorR_T, Container_T, Mesh_T>& rhs)
  { return lhs.base() < rhs.base(); }

template<class Iterator_T, class Container_T, class Mesh_T>
  inline bool
  operator<(const MeshIterator<Iterator_T, Container_T, Mesh_T>& lhs,
            const MeshIterator<Iterator_T, Container_T, Mesh_T>& rhs)
  { return lhs.base() < rhs.base(); }

template<class IteratorL_T, class IteratorR_T, class Container_T, class Mesh_T>
  inline bool
  operator>(const MeshIterator<IteratorL_T, Container_T, Mesh_T>& lhs,
            const MeshIterator<IteratorR_T, Container_T, Mesh_T>& rhs)
  { return lhs.base() > rhs.base(); }

template<class Iterator_T, class Container_T, class Mesh_T>
  inline bool
  operator>(const MeshIterator<Iterator_T, Container_T, Mesh_T>& lhs,
            const MeshIterator<Iterator_T, Container_T, Mesh_T>& rhs)
  { return lhs.base() > rhs.base(); }

template<class IteratorL_T, class IteratorR_T, class Container_T, class Mesh_T>
  inline bool
  operator<=(const MeshIterator<IteratorL_T, Container_T, Mesh_T>& lhs,
             const MeshIterator<IteratorR_T, Container_T, Mesh_T>& rhs)
  { return lhs.base() <= rhs.base(); }

template<class Iterator_T, class Container_T, class Mesh_T>
  inline bool
  operator<=(const MeshIterator<Iterator_T, Container_T, Mesh_T>& lhs,
             const MeshIterator<Iterator_T, Container_T, Mesh_T>& rhs)
  { return lhs.base() <= rhs.base(); }

template<class IteratorL_T, class IteratorR_T, class Container_T, class Mesh_T>
  inline bool
  operator>=(const MeshIterator<IteratorL_T, Container_T, Mesh_T>& lhs,
             const MeshIterator<IteratorR_T, Container_T, Mesh_T>& rhs)
  { return lhs.base() >= rhs.base(); }

template<class Iterator_T, class Container_T, class Mesh_T>
  inline bool
  operator>=(const MeshIterator<Iterator_T, Container_T, Mesh_T>& lhs,
             const MeshIterator<Iterator_T, Container_T, Mesh_T>& rhs)
  { return lhs.base() >= rhs.base(); }

// _GLIBCXX_RESOLVE_LIB_DEFECTS
// According to the resolution of DR179 not only the various comparison
// operators but also operator- must accept mixed iterator/const_iterator
// parameters.
template<class IteratorL_T, class IteratorR_T, class Container_T, class Mesh_T>
#ifdef GXX_EXPERIMENTAL_CXX0X
  // DR 685.
  inline auto
  operator-(const MeshIterator<IteratorL_T, Container_T, Mesh_T>& lhs,
            const MeshIterator<IteratorR_T, Container_T, Mesh_T>& rhs)
  -> decltype(lhs.base() - rhs.base())
#else
  inline class MeshIterator<IteratorL_T, Container_T, Mesh_T>::difference_type
  operator-(const MeshIterator<IteratorL_T, Container_T, Mesh_T>& lhs,
            const MeshIterator<IteratorR_T, Container_T, Mesh_T>& rhs)
#endif
  { return lhs.base() - rhs.base(); }

template<class Iterator_T, class Container_T, class Mesh_T>
  inline class MeshIterator<Iterator_T, Container_T, Mesh_T>::difference_type
  operator-(const MeshIterator<Iterator_T, Container_T, Mesh_T>& lhs,
            const MeshIterator<Iterator_T, Container_T, Mesh_T>& rhs)
  { return lhs.base() - rhs.base(); }

} // end namespace

#endif













