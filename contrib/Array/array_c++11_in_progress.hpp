// This file is part of generic_array, A lightweight generic
// N-dimensional array library
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.


#ifndef MULTI_ARRAY_HPP
#define MULTI_ARRAY_HPP

#include <vector>
#include <stdexcept>

namespace marray {

#ifndef MA_DEFAULT_MAJOR
#define MA_DEFAULT_MAJOR RowMajor
#endif


enum Options {

  RowMajor = 1 << 1,
  ColMajor = 1 << 2

};


namespace internal
{
  template<class T> class Traits;

  template<int> struct CompileTimeError;
  template<> struct CompileTimeError<true> {};

  #define MA_STATIC_CHECK(expr, msg) \
      { internal::CompileTimeError<((expr) != 0)> ERROR_##msg; (void)ERROR_##msg; }



  // for syntax sugar initialization
  template<typename UserT, typename IteratorT>
  class ListInitializer {

  public:
    ListInitializer(IteratorT iter)
        : iter_(iter)
    { }
    
    ListInitializer<UserT, IteratorT> operator,(UserT const& x)
    {
      *iter_ = x;
      ++iter_;
      return ListInitializer<UserT, IteratorT>(iter_);
    }
    
  private:
    ListInitializer();

  protected:
    IteratorT iter_;
  };

  template<typename ArrayT, typename IteratorT = typename ArrayT::UserT*>
  class ListInitializationSwitch {

  public:
    typedef typename ArrayT::UserT UserT;
  
    ListInitializationSwitch(const ListInitializationSwitch<ArrayT>& lis) : array_(lis.array_), value_(lis.value_)
    {  }
  
    ListInitializationSwitch(ArrayT& array, UserT const& value) : array_(array), value_(value)
    { }
  
    ListInitializer<UserT, IteratorT> operator,(UserT const& x)
    {
      IteratorT iter = array_.data();
      *iter = value_;
      ++iter;
      *iter = x;
      ++iter;
      return ListInitializer<UserT, IteratorT>(iter);
    }

  private:
    ListInitializationSwitch();

  protected:
    ArrayT& array_;
    UserT value_;
  };





  template<int Rank>
  struct RowMajIdxComputer
  {
    static int idx(int const*const sizes, int const*const indices)
    {
      return RowMajIdxComputer<Rank-1>::idx(sizes, indices) * sizes[Rank-1]  + indices[Rank-1];
    }
  };
  template<>
  struct RowMajIdxComputer<1>
  {
    static int idx(int const*, int const* indices)
    { return indices[0]; }
  };




  template<int Rank>
  struct ColMajIdxComputer
  {
    static int idx(int const* sizes, int const* indices)
    {
      return indices[0] + sizes[0]*ColMajIdxComputer<Rank-1>::idx(sizes+1, indices+1);
    }
  };
  template<>
  struct ColMajIdxComputer<1>
  {
    static int idx(int const*const, int const*const indices)
    { return indices[0]; }
  };


  template<int Rank, bool isRowMajor>
  struct IdxCompTraits;

  template<int Rank>
  struct IdxCompTraits<Rank, true>
  {
    typedef RowMajIdxComputer<Rank> type;
  };

  template<int Rank>
  struct IdxCompTraits<Rank, false>
  {
    typedef ColMajIdxComputer<Rank> type;
  };

#ifdef DEBUG
  template<int Rank>
  struct BoundCheck
  {
    static void check(int const dims[], int const args[])
    {
      if (args[0] >= dims[0])
        throw std::out_of_range ("ERROR: Array<>: invalid index");
      BoundCheck<Rank-1>::check(dims+1, args +1);
    }
  };

  template<>
  struct BoundCheck<1>
  {
    static void check(int const dims[], int const args[])
    {
      if (args[0] >= dims[0])
        throw std::out_of_range ("ERROR: Array<>: invalid index");
    }
  };

  // assert i < j
  inline void assertLess(int i, int j, const char* msg)
  {
    if (i >= j)
      throw std::out_of_range (msg);
  }


  inline void assertTrue(bool b, const char* msg)
  {
    if (!b)
      throw std::out_of_range (msg);
  }

#else
  template<int Rank, typename ...Args>
  struct BoundCheck
  {
    static void check(...) { }
  };

  // assert i < j
  inline void assertLess(... )
  { }

  inline void assertTrue(...)
  { }

#endif


  template<bool isGreaterThan0>
  struct CheckRank;

  template<>
  struct CheckRank<true> {};


} // end `internal` namespace





template<typename Derived>
class ArrayBase
{
#if !defined(THIS) && !defined(CONST_THIS)
  #define THIS static_cast<Derived*>(this)
  #define CONST_THIS static_cast<const Derived*>(this)
#endif

  typedef ArrayBase Self;
  typedef typename internal::Traits<Derived>::UserT UserT;
  static const int Rank = internal::Traits<Derived>::Rank;
  static const bool isRowMajor = internal::Traits<Derived>::isRowMajor;

public:

  typedef          UserT&                       reference;
  typedef          UserT const&                 const_reference;
  typedef          UserT*                       iterator;
  typedef          UserT const*                 const_iterator;
  typedef          int                          size_type;
  typedef          std::ptrdiff_t               difference_type;
  typedef          UserT*                       pointer;
  typedef          UserT const*                 const_pointer;


  iterator begin()
  { return THIS->data(); }

  const_iterator begin() const
  { return THIS->data(); }

  iterator end()
  { return THIS->data() + THIS->size(); }

  const_iterator end() const
  { return THIS->data() + THIS->size(); }

  reference back()
  { return *(--end()); }

  const_reference back() const
  { return *(--end()); }

  reference front()
  { return *begin(); }

  const_reference front() const
  { return *begin(); }


  template<class ...Args>
  reference operator() (Args ...args)
  {
    MA_STATIC_CHECK(sizeof...(Args) == Rank, ERROR_NUMBER_OF_ARGUMENTS_DOES_NOT_MATCH_ARRAY_RANK);

    typedef typename internal::IdxCompTraits<Rank, isRowMajor>::type ToRelative;

    int const indices[] = {args...};

    internal::BoundCheck<Rank>::check(THIS->rdims(), indices);

    return THIS->operator[] ( ToRelative::idx(THIS->rdims(), indices) );
  }

  template<class ...Args>
  const_reference operator() (Args ...args) const
  {
    MA_STATIC_CHECK(sizeof...(Args) == Rank, ERROR_NUMBER_OF_ARGUMENTS_DOES_NOT_MATCH_ARRAY_RANK);

    typedef typename internal::IdxCompTraits<Rank, isRowMajor>::type ToRelative;

    int const indices[] = {args...};

    internal::BoundCheck<Rank>::check(THIS->rdims(), indices);

    return THIS->operator[] ( ToRelative::idx(THIS->rdims(), indices) );
  }

  int maxDim() const
  {
    int m = 0;
    for (int i = 0; i < Rank; ++i)
      if ( CONST_THIS->dim(i) > m)
        m = CONST_THIS->dim(i);
    return m;
  }


  // Const Version -----------



#undef THIS
#undef CONST_THIS
};

template<typename P_type, int P_rank, Options P_opts = MA_DEFAULT_MAJOR, typename P_MemBlock = std::vector<P_type> >
class Array : public P_MemBlock, public ArrayBase<Array<P_type,P_rank,P_opts,P_MemBlock> >
{
  
  typedef P_MemBlock       Base1;
  typedef ArrayBase<Array> Base2;

  typedef typename Base2::reference reference;
  typedef typename Base2::const_reference const_reference;

  friend class ArrayBase<Array>;

//  using P_MemBlock::m_data;

public:

  typedef P_type UserT;
  static const int Rank = P_rank;
  static const bool isRowMajor = P_opts & RowMajor;

  using Base1::resize;

private:
    int m_rdims[Rank];   // size of each rank


public:

  Array() : Base1(), Base2(), m_rdims() {};
  Array(Array const& ) = default;
  Array& operator= (Array const&) = default;

  internal::ListInitializationSwitch<Array, UserT*> operator=(UserT const& x)
  {
    return internal::ListInitializationSwitch<Array, UserT*>(*this, x);
  }

  template<class ...Args>
  Array(int i0, Args...args)
  {
    reshape(i0, args...);
  }

  int rank() const
  { return Rank; }

  int dim(int r) const
  {
    internal::assertTrue(r < Rank, "**ERROR**: Array<>: invalid index in function `size()`");
    return m_rdims[r];
  }
  int size() const
  { return Base1::size(); }

  template<class ...Args>
  void reshape(int i0, Args...args)
  {
    MA_STATIC_CHECK(sizeof...(Args)+1 == Rank, TOO_FEW_ARGUMENTS_IN_RESHAPE);
    int const new_dims[] = { i0, args... };

    int new_size = 1;
    for (int i = 0; i < Rank; ++i)
    {
      internal::assertTrue(new_dims[i] > 0, "**ERROR**: Array<>: dimension must be greater than 0");
      m_rdims[i] = new_dims[i];
      new_size *= new_dims[i];
    }

    resize(new_size);
  }

protected:

  // vecotr with rank sizes
  int* rdims()
  { return m_rdims; }

  int const* rdims() const
  { return m_rdims; }

};


template<typename P_type, int P_rank, Options P_opts = MA_DEFAULT_MAJOR >
class Amaps : public ArrayBase<Amaps<P_type,P_rank,P_opts> >
{

  typedef ArrayBase<Amaps> Base;

  typedef typename Base::reference reference;
  typedef typename Base::const_reference const_reference;

  friend class ArrayBase<Amaps>;

public:

  typedef P_type UserT;
  static const int Rank = P_rank;
  static const bool isRowMajor = P_opts & RowMajor;

private:
  UserT* m_data;
  int    m_size;
  int    m_rdims[Rank];

public:

  Amaps() = delete;
  Amaps(Amaps const& ) = default;
  Amaps& operator= (Amaps const&) = default;

  template<class ...Args>
  Amaps(UserT* mapped, Args...args)
  {
    MA_STATIC_CHECK(sizeof...(Args) == Rank, TOO_FEW_ARGUMENTS_IN_AMAPS_CONSTRUCTOR);
    
    int const new_dims[] = { args... };
    m_size = 1;
    for (int i = 0; i < Rank; ++i)
    {
      internal::assertTrue(new_dims[i] > 0, "**ERROR**: Amaps<>: dimension must be greater than 0");
      m_rdims[i] = new_dims[i];
      m_size *= new_dims[i];
    }
    
    if (mapped == NULL)
      throw "**ERROR**: Amaps<>: null pointer";
    m_data = mapped;
  }

  internal::ListInitializationSwitch<Amaps, UserT*> operator=(UserT const& x)
  {
    return internal::ListInitializationSwitch<Amaps, UserT*>(*this, x);
  }

  int rank() const
  { return Rank; }

  int dim(int r) const
  {
    internal::assertTrue(r < Rank, "**ERROR**: Amaps<>: invalid index in function `size()`");
    return m_rdims[r];
  }
  
  int size() const
  { return m_size; }

  UserT* data()
  {return m_data; }

  UserT const* data() const
  {return m_data; }

  UserT& operator[] (int i)
  { return m_data[i]; }
  
  UserT const& operator[] (int i) const
  { return m_data[i]; }

protected: // used by base class

  // vecotr with rank sizes
  int* rdims()
  { return m_rdims; }

  int const* rdims() const
  { return m_rdims; }
};

namespace internal
{

template<class T, int A, Options O, class M>
struct Traits<Array<T,A,O,M> > {
  typedef T UserT;
  static const int Rank = A;
  static const bool isRowMajor = O & RowMajor;
  typedef M MemBlockT;
};

template<class T, int A, Options O>
struct Traits<Amaps<T,A,O> > {
  typedef T UserT;
  static const int Rank = A;
  static const bool isRowMajor = O & RowMajor;
};


}





} // endnamespace

#endif
