#ifndef ALELIB_LIST_TYPE_HPP
#define ALELIB_LIST_TYPE_HPP

#include <vector>
#include <map>
#include <set>
#include <utility>
#include <iterator>
#include <tr1/type_traits>
#include "../mesh/enums.hpp"
#include "../util/misc.hpp"
#include "contrib/Loki/set_vector.hpp"

//#include "../mesh/labelable.hpp"  //nao precisa
#include <iostream>
#include "macros.hpp"

namespace alelib
{
// TODO: a const_iterator



//
//                      _      _  _       _
//                     | |    | |(_) _ | |_
//                     | |    | || |/ || |
//                     | |_ | || |\ \| |_
//                     |_||_||_||_/ \|
//
//
//

// fwd
template<class,class> class SeqList_iterator;


/// @brief A container with a specific use for the mesh. The value type of this container is
/// deducted from the container passed as template argument "C". This value type should have two
/// functions: "bool isDisabled() const" and "void setDisabledTo(bool)", see Labelable class.
/// These functions are used to "delete" containers elements. If these functions don't have
/// public access, make SeqList as a friend class.
///
template<class C,                      ///< A random access data container: std::vector, boost::ptr_vector, etc.
                                       ///< it should have public access to its value type.
                                       /// for store pointers, DO NOT USE std::vector<T*>. Use boost::ptr_vector<T> instead.
         class S  = SetVector<index_t> >   ///< A sorted container for internal indices. For most
                                       ///< purposes you will never have to change this default.
class SeqList
{
  template<class,class> friend class SeqList_iterator;

  typedef          SeqList<C,S>                          Self;
  typedef typename C::iterator                           DataIterator;
  typedef typename C::const_iterator                     DataConstIterator;
  typedef typename C::reverse_iterator                   DataReverseIterator;

public:

  // stl standard
  typedef          C                                      container_type;
  typedef typename container_type::value_type             value_type;
  typedef typename container_type::allocator_type         allocator_type;
  typedef          S                                      ids_container_type;
  typedef typename container_type::reference              reference;
  typedef typename container_type::const_reference        const_reference;
  typedef typename container_type::pointer                pointer;
  typedef typename container_type::size_type              size_type;
  typedef typename container_type::difference_type        difference_type;
  
  typedef SeqList_iterator<DataIterator,Self>             iterator;
  typedef SeqList_iterator<DataConstIterator,Self const>  const_iterator;

private:
  
  // auxiliary typedefs
  typedef typename std::tr1::remove_pointer<value_type>::type RP_ValueType; // remove pointer value type
  typedef          RP_ValueType &                          RP_Reference;
  typedef          RP_ValueType const&                     RP_ConstReference;
  typedef          RP_ValueType *                          RP_Pointer;
  typedef          RP_ValueType const*                     RP_ConstPointer;

public:

  template<class T> // T = value_type
  struct fi_InsertFunArgument {
    typedef T const& const_type;
    typedef T        type;
    
  };
  
  template<class T> // T* = value_type
  struct fi_InsertFunArgument<T*> {
    typedef T*  type;
    typedef T*  const_type;
  };

  explicit SeqList()
  {
    m_data.clear();
    m_disabled_idcs.clear();
    m_actived_beg = m_data.begin();
  }

  void clear()
  {
    m_data.clear();
    m_disabled_idcs.clear();
  }

  //// TODO: only to vector type ...
  //int getDataId(const_pointer v) const
  //{
  //  return static_cast<int>(std::distance(m_data.begin(), DataConstIterator(v)));
  //}

  void reserve(size_type n)
  { m_data.reserve(n); }

  size_type size() const
  {return m_data.size() - m_disabled_idcs.size();};

  size_type totalSize() const
  {return m_data.size();};

  //  SERIAL VERSION 

  iterator begin()
  { return iterator(this, m_actived_beg); }

  const_iterator begin() const
  { return const_iterator(this, m_actived_beg); }  

  iterator end()
  { return iterator(this, m_data.end()); }

  const_iterator end() const
  { return const_iterator(this, m_data.end()); }

  const_iterator constBegin() const
  { return const_iterator(this, m_actived_beg); } 


  //  PARALLEL VERSION

  iterator begin(size_type tid, int nthreads, index_t * begin_idx = NULL)
  {
    size_type const N = size();
    size_type const start = size_type(tid*(N/nthreads) + ((N%nthreads) < tid ? (N%nthreads) : tid));
    if(begin_idx)
      *begin_idx = (index_t)start;
    iterator s = begin();
    for (size_type i = 0; i < start; ++i)
      ++s;
    return s;
  }

  iterator end(size_type tid, int nthreads, index_t * end_idx = NULL)
  {
    size_type const N = size();
    size_type const start = size_type(tid*(N/nthreads) + ((N%nthreads) < tid ? (N%nthreads) : tid));
    size_type const end_  = size_type(start + N/nthreads + ((N%nthreads) > tid));
    if (end_idx)
      *end_idx = (index_t)end_;
    iterator e = begin(tid,nthreads);
    for (size_type i = 0; i < end_-start; ++i)
      ++e;
    return e;
  }

  const_iterator begin(size_type tid, int nthreads, index_t * begin_idx = NULL) const
  {
    index_t const N = size();
    index_t const start = tid*(N/nthreads) + ((N%nthreads) < tid ? (N%nthreads) : tid);
    if(begin_idx)
      *begin_idx = (index_t)start;
    const_iterator s = begin();
    for (size_type i = 0; i < start; ++i)
      ++s;
    return s;
  }

  const_iterator end(size_type tid, int nthreads, index_t * end_idx = NULL) const
  {
    size_type const N = size();
    size_type const start = tid*(N/nthreads) + ((N%nthreads) < tid ? (N%nthreads) : tid);
    size_type const end_  = start + N/nthreads + ((N%nthreads) > tid);
    if (end_idx)
      *end_idx = (index_t)end_;
    const_iterator e = begin(tid,nthreads);
    for (int i = 0; i < end_-start; ++i)
      ++e;
    return e;
  }


  // modifiers

  void disable(const_iterator it)
  { disable(it.index());};

  void disable(index_t del_id)
  {
    //DataIterator it = DataIterator(&m_data[del_id]);
    RP_Pointer it = &m_data.at(del_id);
    if (it->isDisabled())
    {
      //std::cout << "ERRROR: trying to disable a disabled element \n";
      //throw;
      return;
    }
    m_disabled_idcs.insert(del_id);
    it->setDisabledTo(true);

    if (it == &*m_actived_beg)
      fi_update_member_beg_removal();
  }

  /** @brief insert an element and return your id.
   */
  index_t insert(typename fi_InsertFunArgument<value_type>::const_type obj)
  {
    //fi_InsertFunArgument<value_type>::type 
    //    = T const& for non-pointer containers
    //    = T*   for pointer containers
    return insert_impl<value_type>(obj);
  }

  /** @brief insert an element and return your id.
   */
  index_t insert()
  {
    typedef typename fi_InsertFunArgument<value_type>::type t;
    return insert_impl<value_type>(t());
  }

  // com dor no coracao
  //void resize(size_type s)
  //{
  //  m_data.resize(s);
  //  fi_update_member_beg();
  //}

  RP_Reference operator[](size_type n)
  { return m_data[n]; }

  RP_ConstReference operator[](size_type n) const
  { return m_data[n]; }

  index_t contiguousId(index_t id) const
  { return id - static_cast<index_t>( std::distance(m_disabled_idcs.begin(), std::lower_bound(m_disabled_idcs.begin(), m_disabled_idcs.end(), id)));  }
  
  /// A shortcut to get multiples cids at same time.
  /// @param[out] cids contiguous ids.
  ///  
  void contiguousIds(index_t *ids_beg, index_t const* ids_end, index_t *cids) const
  {
    while (ids_beg != ids_end)
    { *cids++ = contiguousId(*ids_beg++); }
  }
    
protected:

  template<class Value_type>
  index_t insert_impl(const_reference obj, typename EnableIf< ! std::tr1::is_pointer<Value_type>::value >::type * = NULL)
  {
    if (m_disabled_idcs.empty())
    {
      // --- push_back ----
      m_data.push_back(obj);
      fi_update_member_beg();
      // -------------------
      return m_data.size()-1;
    }

    index_t const new_id = m_disabled_idcs.back();
    m_disabled_idcs.pop_back();
    m_data[new_id] = obj;

    fi_update_member_beg_insertion(new_id);
    return new_id;

  }

  template<class Value_type>
  index_t insert_impl(value_type obj, typename EnableIf< std::tr1::is_pointer<Value_type>::value >::type * = NULL)
  {
    if (m_disabled_idcs.empty())
    {
      // --- push_back ----
      m_data.push_back(obj);
      fi_update_member_beg();
      // -------------------
      return m_data.size()-1;
    }

    index_t const new_id = m_disabled_idcs.back();
    m_disabled_idcs.pop_back();
    m_data[new_id] = *obj;

    fi_update_member_beg();
    return new_id;
  }

  void fi_update_member_beg()
  {
    m_actived_beg = m_data.begin();
    while (m_actived_beg != m_data.end() && m_actived_beg->isDisabled())
      ++m_actived_beg;
  }

  void fi_update_member_beg_removal()
  {
    while (m_actived_beg != m_data.end() && m_actived_beg->isDisabled())
      ++m_actived_beg;
  }

  void fi_update_member_beg_insertion(index_t id)
  {
    DataIterator new_beg = m_data.begin() + id;
    if (new_beg < m_actived_beg)
      m_actived_beg = new_beg;
  }



  container_type      m_data;
  ids_container_type  m_disabled_idcs; // sorted vector
  DataIterator        m_actived_beg;   // iterator to the beginning of valid data

};




// an iterator adaptor
template<typename Iterator_T, typename Container_T>
class SeqList_iterator
{

  template<class,class> friend class SeqList;
  template<class> friend class SeqList_const_iterator;

  typedef Container_T*      SeqListPtr;
  typedef SeqList_iterator Self;
  typedef Iterator_T        DataIterator;
public:

  // stl standard
  typedef typename DataIterator::difference_type   difference_type;
  typedef typename DataIterator::value_type        value_type;
  typedef typename DataIterator::pointer           pointer;
  typedef typename DataIterator::reference         reference;
  typedef typename DataIterator::iterator_category iterator_category;

protected:

  // attributes
  DataIterator m_data_iter;
  SeqListPtr   m_seq_ptr;

public:

  SeqList_iterator(SeqListPtr sq, DataIterator x) : m_data_iter(x), m_seq_ptr(sq) {}

  SeqList_iterator() : m_data_iter(), m_seq_ptr(NULL) {}

  // Allow iterator to const_iterator conversion
  template<class Iter_T>
  SeqList_iterator(const SeqList_iterator<Iter_T,Container_T> & i)
  : m_data_iter(i.base()),  m_seq_ptr(i.m_seq_ptr) { }

  DataIterator const& base() const
  { return m_data_iter; }

  difference_type index() const
  { return base() - m_seq_ptr->m_data.begin();}

  reference
  operator*() const
  {return *m_data_iter; }

  pointer
  operator->() const
  { return &(operator*()); }
  
  Self&
  operator++()
  {
    ++m_data_iter;
    while(m_data_iter != m_seq_ptr->m_data.end() && m_data_iter->isDisabled())
      ++m_data_iter;
    return *this;
  }

  Self
  operator++(int)
  {
    Self tmp = *this;
    ++m_data_iter;
    while(m_data_iter != m_seq_ptr->m_data.end() && m_data_iter->isDisabled())
      ++m_data_iter;
    return tmp;
  }

  Self
  operator+(std::ptrdiff_t idx) const
  {
    return Self(m_seq_ptr, m_data_iter + idx);
  }

  Self&
  operator--()
  {
    --m_data_iter;
    while(m_data_iter != m_seq_ptr->m_data.begin() && m_data_iter->isDisabled())
      --m_data_iter;
    return *this;
  }

  Self
  operator--(int)
  {
    Self tmp = *this;
    --m_data_iter;
    while(m_data_iter != m_seq_ptr->m_data.begin() && m_data_iter->isDisabled())
      --m_data_iter;
    return tmp;
  }
 
};

  //
  // inspired by /usr/include/c++/4.6.3/bits/stl_iterator.h
  //

  // Note: In what follows, the left- and right-hand-side iterators are
  // allowed to vary in types (conceptually in cv-qualification) so that
  // comparison between cv-qualified and non-cv-qualified iterators be
  // valid.  However, the greedy and unfriendly operators in std::rel_ops
  // will make overload resolution ambiguous (when in scope) if we don't
  // provide overloads whose operands are of the same type.  Can someone
  // remind me what generic programming is about? -- Gaby

  // Forward iterator requirements
  template<typename IteratorL_T, typename IteratorR_T, typename Container_T>
    inline bool
    operator==(const SeqList_iterator<IteratorL_T, Container_T>& lhs,
               const SeqList_iterator<IteratorR_T, Container_T>& rhs)
    { return lhs.base() == rhs.base(); }

  template<typename Iterator_T, typename Container_T>
    inline bool
    operator==(const SeqList_iterator<Iterator_T, Container_T>& lhs,
               const SeqList_iterator<Iterator_T, Container_T>& rhs)
    { return lhs.base() == rhs.base(); }

  template<typename IteratorL_T, typename IteratorR_T, typename Container_T>
    inline bool
    operator!=(const SeqList_iterator<IteratorL_T, Container_T>& lhs,
               const SeqList_iterator<IteratorR_T, Container_T>& rhs)
    { return lhs.base() != rhs.base(); }

  template<typename Iterator_T, typename Container_T>
    inline bool
    operator!=(const SeqList_iterator<Iterator_T, Container_T>& lhs,
               const SeqList_iterator<Iterator_T, Container_T>& rhs)
    { return lhs.base() != rhs.base(); }

  // Random access iterator requirements
  template<typename IteratorL_T, typename IteratorR_T, typename Container_T>
    inline bool
    operator<(const SeqList_iterator<IteratorL_T, Container_T>& lhs,
              const SeqList_iterator<IteratorR_T, Container_T>& rhs)
    { return lhs.base() < rhs.base(); }

  template<typename Iterator_T, typename Container_T>
    inline bool
    operator<(const SeqList_iterator<Iterator_T, Container_T>& lhs,
              const SeqList_iterator<Iterator_T, Container_T>& rhs)
    { return lhs.base() < rhs.base(); }

  template<typename IteratorL_T, typename IteratorR_T, typename Container_T>
    inline bool
    operator>(const SeqList_iterator<IteratorL_T, Container_T>& lhs,
              const SeqList_iterator<IteratorR_T, Container_T>& rhs)
    { return lhs.base() > rhs.base(); }

  template<typename Iterator_T, typename Container_T>
    inline bool
    operator>(const SeqList_iterator<Iterator_T, Container_T>& lhs,
              const SeqList_iterator<Iterator_T, Container_T>& rhs)
    { return lhs.base() > rhs.base(); }

  template<typename IteratorL_T, typename IteratorR_T, typename Container_T>
    inline bool
    operator<=(const SeqList_iterator<IteratorL_T, Container_T>& lhs,
               const SeqList_iterator<IteratorR_T, Container_T>& rhs)
    { return lhs.base() <= rhs.base(); }

  template<typename Iterator_T, typename Container_T>
    inline bool
    operator<=(const SeqList_iterator<Iterator_T, Container_T>& lhs,
               const SeqList_iterator<Iterator_T, Container_T>& rhs)
    { return lhs.base() <= rhs.base(); }

  template<typename IteratorL_T, typename IteratorR_T, typename Container_T>
    inline bool
    operator>=(const SeqList_iterator<IteratorL_T, Container_T>& lhs,
               const SeqList_iterator<IteratorR_T, Container_T>& rhs)
    { return lhs.base() >= rhs.base(); }

  template<typename Iterator_T, typename Container_T>
    inline bool
    operator>=(const SeqList_iterator<Iterator_T, Container_T>& lhs,
               const SeqList_iterator<Iterator_T, Container_T>& rhs)
    { return lhs.base() >= rhs.base(); }

  // _GLIBCXX_RESOLVE_LIB_DEFECTS
  // According to the resolution of DR179 not only the various comparison
  // operators but also operator- must accept mixed iterator/const_iterator
  // parameters.
  template<typename IteratorL_T, typename IteratorR_T, typename Container_T>
#ifdef GXX_EXPERIMENTAL_CXX0X
    // DR 685.
    inline auto
    operator-(const SeqList_iterator<IteratorL_T, Container_T>& lhs,
              const SeqList_iterator<IteratorR_T, Container_T>& rhs)
    -> decltype(lhs.base() - rhs.base())
#else
    inline typename SeqList_iterator<IteratorL_T, Container_T>::difference_type
    operator-(const SeqList_iterator<IteratorL_T, Container_T>& lhs,
              const SeqList_iterator<IteratorR_T, Container_T>& rhs)
#endif
    { return lhs.base() - rhs.base(); }

  template<typename Iterator_T, typename Container_T>
    inline typename SeqList_iterator<Iterator_T, Container_T>::difference_type
    operator-(const SeqList_iterator<Iterator_T, Container_T>& lhs,
              const SeqList_iterator<Iterator_T, Container_T>& rhs)
    { return lhs.base() - rhs.base(); }



} // end alelib namespace


#endif


                          
                          
                          
