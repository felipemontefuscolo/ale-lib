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


#ifndef ALELIB_SET_VECTOR_HPP
#define ALELIB_SET_VECTOR_HPP

// inspired in Loki::SetVector
// NEVER use dynamic allocation, i.e., SetVector<Foo>* p; .
// SetVector base's doesnt have virtual destructor.


#include <algorithm>
#include <functional>
#include <vector>
#include <utility>


template<
         class K,
         class C = std::less<K>,
         class A = std::allocator< K >
        >
class SetVector : private std::vector<K, A>, private C
{
  typedef std::vector<K, A> Base;
  typedef C                 MyCompare;

public:
  typedef K key_type;
  typedef typename Base::value_type value_type;

  typedef          C                            key_compare;
  typedef          A                            allocator_type;
  typedef typename A::reference                 reference;
  typedef typename A::const_reference           const_reference;
  typedef typename Base::iterator               iterator;
  typedef typename Base::const_iterator         const_iterator;
  typedef typename Base::size_type              size_type;
  typedef typename Base::difference_type        difference_type;
  typedef typename A::pointer                   pointer;
  typedef typename A::const_pointer             const_pointer;
  typedef typename Base::reverse_iterator       reverse_iterator;
  typedef typename Base::const_reverse_iterator const_reverse_iterator;

  explicit SetVector(const key_compare& comp = key_compare(),
                     const A& alloc = A())
  : Base(alloc), MyCompare(comp)
  {}

  template <class InputIterator>
  SetVector(InputIterator first, InputIterator last,
              const key_compare& comp = key_compare(),
              const A& alloc = A())
  : Base(first, last, alloc), MyCompare(comp)
  {
      MyCompare& me = *this;
      std::sort(begin(), end(), me);
      iterator it = std::unique(begin(), end());
      Base::resize(it - begin());
  }

  SetVector& operator=(const SetVector& rhs)
  {
    SetVector(rhs).swap(*this);
    return *this;
  }

  // iterators:
  // The following are here because MWCW gets 'using' wrong
  iterator begin() { return Base::begin(); }
  const_iterator begin() const { return Base::begin(); }
  iterator end() { return Base::end(); }
  const_iterator end() const { return Base::end(); }
  reverse_iterator rbegin() { return Base::rbegin(); }
  const_reverse_iterator rbegin() const { return Base::rbegin(); }
  reverse_iterator rend() { return Base::rend(); }
  const_reverse_iterator rend() const { return Base::rend(); }

  // capacity:
  bool empty() const { return Base::empty(); }
  size_type size() const { return Base::size(); }
  size_type max_size() { return Base::max_size(); }



  void reserve(size_type a)
  {
    Base::reserve(a);
  }
  size_type capacity() const
  {
    return Base::capacity();
  }
  reference back()
  {
    return Base::back();
  }
  const_reference back ( ) const
  {
    return Base::back();
  }
  void pop_back()
  {
    Base::pop_back();
  }

  pointer data()
  {
    return Base::data();
  }
  const_pointer data() const
  {
    return Base::data();
  }

  // modifiers:
  std::pair<iterator, bool> insert(const value_type& val)
  {
    bool found(true);
    iterator i(lower_bound(val));

    if (i == end() || this->operator()(val, *i))
    {
      i = Base::insert(i, val);
      found = false;
    }
    return std::make_pair(i, !found);
  }

  iterator insert(iterator pos, const value_type& val)
  {
    if( (pos == begin() || this->operator()(*(pos-1),val)) &&
        (pos == end()   || this->operator()(val, *pos)) )
    {
      return Base::insert(pos, val);
    }
    return insert(val).first;
  }

  template <class InputIterator>
  void insert(InputIterator first, InputIterator last)
  { for (; first != last; ++first) insert(*first); }

  void erase(iterator pos)
  { Base::erase(pos); }

  size_type erase(const key_type& k)
  {
    iterator i(find(k));
    if (i == end()) return 0;
    erase(i);
    return 1;
  }

  void erase(iterator first, iterator last)
  { Base::erase(first, last); }


  void swap(SetVector& other)
  {
    Base::swap(other);
    MyCompare& me = *this;
    MyCompare& rhs = other;
    std::swap(me, rhs);
  }

  void clear()
  { Base::clear(); }

  // observers:
  key_compare key_comp() const
  { return *this; }

  // não tem
  //value_compare value_comp() const
  //{
  //  const key_compare& comp = *this;
  //  return value_compare(comp);
  //}


  // 23.3.1.3 map operations:
  iterator find(const key_type& k)
  {
    iterator i(lower_bound(k));
    if (i != end() && this->operator()(k, *i))
    {
      i = end();
    }
    return i;
  }

  const_iterator find(const key_type& k) const
  {
    const_iterator i(lower_bound(k));
    if (i != end() && this->operator()(k, *i))
    {
      i = end();
    }
    return i;
  }

  size_type count(const key_type& k) const
  { return find(k) != end(); }

  iterator lower_bound(const key_type& k)
  {
    MyCompare& me = *this;
    return std::lower_bound(begin(), end(), k, me);
  }

  const_iterator lower_bound(const key_type& k) const
  {
    const MyCompare& me = *this;
    return std::lower_bound(begin(), end(), k, me);
  }

  iterator upper_bound(const key_type& k)
  {
    MyCompare& me = *this;
    return std::upper_bound(begin(), end(), k, me);
  }

  std::pair<iterator, iterator> equal_range(const key_type& k)
  {
    MyCompare& me = *this;
    return std::equal_range(begin(), end(), k, me);
  }

  std::pair<const_iterator, const_iterator> equal_range(
                                      const key_type& k) const
  {
    const MyCompare& me = *this;
    return std::equal_range(begin(), end(), k, me);
  }

  template <class K1, class C1, class A1>
  friend bool operator==(const SetVector<K1, C1, A1>& lhs,
                  const SetVector<K1, C1, A1>& rhs);

  bool operator<(const SetVector& rhs) const
  {
      const Base& me = *this;
      const Base& yo = rhs;
      return me < yo;
  }


  template <class K1, class C1, class A1>
  friend bool operator!=(const SetVector<K1, C1, A1>& lhs,
                         const SetVector<K1, C1, A1>& rhs);

  template <class K1, class C1, class A1>
  friend bool operator>(const SetVector<K1, C1, A1>& lhs,
                        const SetVector<K1, C1, A1>& rhs);

  template <class K1, class C1, class A1>
  friend bool operator>=(const SetVector<K1, C1, A1>& lhs,
                         const SetVector<K1, C1, A1>& rhs);

  template <class K1, class C1, class A1>
  friend bool operator<=(const SetVector<K1, C1, A1>& lhs,
                         const SetVector<K1, C1, A1>& rhs);


};


template <class K, class C, class A>
inline bool operator==(const SetVector<K, C, A>& lhs,
                       const SetVector<K, C, A>& rhs)
{
  const std::vector<K, A>& me = lhs;
  return me == rhs;
}

template <class K, class C, class A>
inline bool operator!=(const SetVector<K, C, A>& lhs,
                       const SetVector<K, C, A>& rhs)
{ return !(lhs == rhs); }

template <class K, class C, class A>
inline bool operator>(const SetVector<K, C, A>& lhs,
                      const SetVector<K, C, A>& rhs)
{ return rhs < lhs; }

template <class K, class C, class A>
inline bool operator>=(const SetVector<K, C, A>& lhs,
                       const SetVector<K, C, A>& rhs)
{ return !(lhs < rhs); }

template <class K, class C, class A>
inline bool operator<=(const SetVector<K, C, A>& lhs,
                       const SetVector<K, C, A>& rhs)
{ return !(rhs < lhs); }


// specialized algorithms:
template <class K, class C, class A>
void swap(SetVector<K, C, A>& lhs, SetVector<K, C, A>& rhs)
{ lhs.swap(rhs); }



#endif
