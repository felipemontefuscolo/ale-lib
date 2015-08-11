// This file is part of generic_array, A lightweight generic
// N-dimensional array library
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.


#ifndef MULTI_ARRAY_HPP
#define MULTI_ARRAY_HPP

#include "listify.hpp"
#include <vector>
#include <stdexcept>

#include <ciso646>  // detect std::lib
#ifdef _LIBCPP_VERSION
// using libc++
#define MULTI_HAVE_TYPE_TRAITS
#else
// using libstdc++
#define MULTI_HAVE_TR1_TYPE_TRAITS
#endif

#ifdef MULTI_HAVE_TYPE_TRAITS
#include <type_traits>
namespace Tr1 = std;
#else
#include <tr1/type_traits>
namespace Tr1 = std::tr1;
#endif

namespace marray {

#ifndef MA_DEFAULT_MAJOR
#define MA_DEFAULT_MAJOR RowMajor
#endif

namespace marray_internal {
	template<bool, typename T = void>
	struct EnableIf {};

	template<typename T>
	struct EnableIf<true, T> {
	  typedef T type;
	};
}


enum Options {

  RowMajor = 1 << 1,
  ColMajor = 1 << 2

};


namespace internal
{
  template<class T> struct Traits;

  template<int> struct CompileTimeError;
  template<> struct CompileTimeError<true> {};

  #define MA_STATIC_CHECK(expr, msg) \
      { internal::CompileTimeError<((expr) != 0)> ERROR_##msg; (void)ERROR_##msg; }


// to expand `int i0, int i1, int i2, ... `

#define MA_EXPAND_ARGS1(type_)                          type_ i0
#define MA_EXPAND_ARGS2(type_)  MA_EXPAND_ARGS1(type_), type_ i1
#define MA_EXPAND_ARGS3(type_)  MA_EXPAND_ARGS2(type_), type_ i2
#define MA_EXPAND_ARGS4(type_)  MA_EXPAND_ARGS3(type_), type_ i3
#define MA_EXPAND_ARGS5(type_)  MA_EXPAND_ARGS4(type_), type_ i4
#define MA_EXPAND_ARGS6(type_)  MA_EXPAND_ARGS5(type_), type_ i5
#define MA_EXPAND_ARGS7(type_)  MA_EXPAND_ARGS6(type_), type_ i6
#define MA_EXPAND_ARGS8(type_)  MA_EXPAND_ARGS7(type_), type_ i7
#define MA_EXPAND_ARGS9(type_)  MA_EXPAND_ARGS8(type_), type_ i8
#define MA_EXPAND_ARGS10(type_) MA_EXPAND_ARGS9(type_), type_ i9

#define MA_EXPAND_ARGS_(N, type_) MA_EXPAND_ARGS##N(type_)
#define MA_EXPAND_ARGS(N, type_) MA_EXPAND_ARGS_(N, type_)


// to expand `i0, i1, i2, ...`

#define MA_EXPAND_SEQ1                  i0
#define MA_EXPAND_SEQ2  MA_EXPAND_SEQ1, i1
#define MA_EXPAND_SEQ3  MA_EXPAND_SEQ2, i2
#define MA_EXPAND_SEQ4  MA_EXPAND_SEQ3, i3
#define MA_EXPAND_SEQ5  MA_EXPAND_SEQ4, i4
#define MA_EXPAND_SEQ6  MA_EXPAND_SEQ5, i5
#define MA_EXPAND_SEQ7  MA_EXPAND_SEQ6, i6
#define MA_EXPAND_SEQ8  MA_EXPAND_SEQ7, i7
#define MA_EXPAND_SEQ9  MA_EXPAND_SEQ8, i8
#define MA_EXPAND_SEQ10 MA_EXPAND_SEQ9, i9

#define MA_EXPAND_SEQ_(N) MA_EXPAND_SEQ##N
#define MA_EXPAND_SEQ(N) MA_EXPAND_SEQ_(N)


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

  //template<typename ArrayT, typename IteratorT = typename ArrayT::UserT*>
  template<typename UserT, typename IteratorT>
  class ListInitializationSwitch {

  public:

    ListInitializationSwitch(const ListInitializationSwitch<UserT,IteratorT>& lis) : array_(lis.array_), value_(lis.value_)
    {  }

    ListInitializationSwitch(UserT* array, UserT const& value) : array_(array), value_(value)
    { }

    ListInitializer<UserT, IteratorT> operator,(UserT const& x)
    {
      IteratorT iter = array_;
      *iter = value_;
      ++iter;
      *iter = x;
      ++iter;
      return ListInitializer<UserT, IteratorT>(iter);
    }

  private:
    ListInitializationSwitch();

  protected:
    UserT* array_;
    UserT value_;
  };





  template<int Rank>
  struct RowMajIdxComputer
  {
    template<class Idx>
    static std::size_t idx(std::size_t const* sizes, Idx const* indices)
    {
      return RowMajIdxComputer<Rank-1>::idx(sizes, indices) * sizes[Rank-1]  + (std::size_t)indices[Rank-1];
    }
  };
  template<>
  struct RowMajIdxComputer<1>
  {
    template<class Idx>
    static std::size_t idx(std::size_t const*, Idx const* indices)
    { return indices[0]; }
  };




  template<int Rank>
  struct ColMajIdxComputer
  {
    template<class Idx>
    static std::size_t idx(std::size_t const* sizes, Idx const* indices)
    {
      return (std::size_t)indices[0] + sizes[0]*ColMajIdxComputer<Rank-1>::idx(sizes+1, indices+1);
    }
  };
  template<>
  struct ColMajIdxComputer<1>
  {
    template<class Idx>
    static std::size_t idx(std::size_t const*, Idx const* indices)
    { return indices[0]; }
  };


  template<int Rank, bool isRowMajor>
  struct IdxComputationTraits;

  template<int Rank>
  struct IdxComputationTraits<Rank, true>
  {
    typedef RowMajIdxComputer<Rank> type;
  };

  template<int Rank>
  struct IdxComputationTraits<Rank, false>
  {
    typedef ColMajIdxComputer<Rank> type;
  };

#ifdef DEBUG
  template<int Rank>
  struct BoundCheck
  {
    template<class Idx_t>
    static void check(std::size_t const dims[], Idx_t const args[])
    {
      if ((std::size_t)args[0] >= dims[0])
        throw std::out_of_range ("ERROR: Array<>: invalid index");
      BoundCheck<Rank-1>::check(dims+1, args +1);
    }
  };

  template<>
  struct BoundCheck<1>
  {
    template<class Idx_t>
    static void check(std::size_t const dims[], Idx_t const args[])
    {
      if ((std::size_t)args[0] >= dims[0])
        throw std::out_of_range ("ERROR: Array<>: invalid index");
    }
  };

  // assert i < j
  inline void assertLess(std::size_t i, std::size_t j, const char* msg)
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
  template<int Rank>
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



//              db                                                     88888888ba
//             d88b                                                    88      "8b
//            d8'`8b                                                   88      ,8P
//           d8'  `8b     8b,dPPYba, 8b,dPPYba, ,adPPYYba, 8b       d8 88aaaaaa8P' ,adPPYYba, ,adPPYba,  ,adPPYba,
//          d8YaaaaY8b    88P'   "Y8 88P'   "Y8 ""     `Y8 `8b     d8' 88""""""8b, ""     `Y8 I8[    "" a8P_____88
//         d8""""""""8b   88         88         ,adPPPPP88  `8b   d8'  88      `8b ,adPPPPP88  `"Y8ba,  8PP"""""""
//        d8'        `8b  88         88         88,    ,88   `8b,d8'   88      a8P 88,    ,88 aa    ]8I "8b,   ,aa
//       d8'          `8b 88         88         `"8bbdP"Y8     Y88'    88888888P"  `"8bbdP"Y8 `"YbbdP"'  `"Ybbd8"'
//                                                             d8'
//                                                            d8'

namespace internal
{
template<class T>
struct GetRef;

template<class T>
struct GetRef<T&> { typedef typename T::reference type; };

template<class T>
struct GetRef<T const&> { typedef typename T::const_reference type; };


template<int Count, int Rank, class VecT, class S> // Count = Rank - 1
struct Proxy
{
	VecT ids[Rank]; // indices
	S a;
  explicit Proxy(std::size_t i, S a_) : a(a_)
	{
		ids[0] = i;
	}

	typedef Proxy type;

	typename Proxy<Count-1, Rank, void, S>::type operator[] (int i)
	{
		ids[1] = i;
		return Proxy<Count-1, Rank, void, S>(ids,a);
	}
};

template<int Count, int Rank, class S> // Count = Rank - 1
struct Proxy<Count, Rank, void, S>
{
	std::size_t* ids; // indices
	S a;
  explicit Proxy(std::size_t* i, S a_) : ids(i), a(a_)
	{	}

	typedef Proxy type;

	typename Proxy<Count-1, Rank, void, S>::type operator[] (int i)
	{
		ids[Rank-Count] = i;
		return Proxy<Count-1, Rank, void, S>(ids,a);
	}
};

//std::remove_reference

template<int Rank, class S>
struct Proxy<0, Rank, void, S>
{
	std::size_t* ids;
  S a;
	explicit Proxy(std::size_t* v, S a_) : ids(v), a(a_)
	{ }

  typedef typename Tr1::remove_reference<S>::type S_no_ref;

  typedef typename GetRef<S>::type type;

	operator type()
  {
    typedef typename internal::IdxComputationTraits<Rank, S_no_ref::isRowMajor>::type ToGlobal;
    return a.access( ToGlobal::idx(a.rdims(), ids) );
	}
};

template<int Rank, class VecT, class S>
struct Proxy<0, Rank, VecT, S>
{
	VecT ids[Rank];
  S a;
	explicit Proxy(std::size_t i, S a_) : a(a_)
	{ ids[0] = i;	}

  typedef typename Tr1::remove_reference<S>::type S_no_ref;

  typedef typename GetRef<S>::type type;

	operator type()
  {
    typedef typename internal::IdxComputationTraits<Rank, S_no_ref::isRowMajor>::type ToGlobal;
    return a.access( ToGlobal::idx(a.rdims(), ids) );
	}

};




} // end internal




template<typename Derived, int P_rank, Options P_opts>
class ArrayBase;

#if !defined(THIS) && !defined(CONST_THIS)
  #define THIS static_cast<Derived*>(this)
  #define CONST_THIS static_cast<const Derived*>(this)
#endif

#define IMPLEMENT_BASE(P_rank)                                                                \
template<typename Derived, Options P_opts>                                                    \
class ArrayBase<Derived, P_rank, P_opts>                                                      \
{                                                                                             \
                                                                                              \
  typedef internal::Traits<Derived> Traits_Derived;                                           \
  typedef ArrayBase Self;                                                                     \
  typedef typename Traits_Derived::UserT UserT;                                               \
                                                                                              \
public:                                                                                       \
  static const int Rank = P_rank;                                                             \
  static const bool isRowMajor = P_opts & RowMajor;                                           \
                                                                                              \
  typedef typename Traits_Derived::reference        reference;                                \
  typedef typename Traits_Derived::const_reference  const_reference;                          \
  typedef typename Traits_Derived::iterator         iterator;                                 \
  typedef typename Traits_Derived::const_iterator   const_iterator;                           \
  typedef typename Traits_Derived::size_type        size_type;                                \
  typedef typename Traits_Derived::difference_type  difference_type;                          \
  typedef typename Traits_Derived::pointer          pointer;                                  \
  typedef typename Traits_Derived::const_pointer    const_pointer;                            \
                                                                                              \
                                                                                              \
  reference operator() (MA_EXPAND_ARGS(P_rank, size_type))                                    \
  {                                                                                           \
    MA_STATIC_CHECK(Rank==P_rank, INVALID_NUMBER_OF_ARGS_IN_CALL_OP);                         \
                                                                                              \
    size_type const indices[] = {MA_EXPAND_SEQ(P_rank)};                                      \
                                                                                              \
    internal::BoundCheck<Rank>::check(THIS->rdims(), indices);                                \
                                                                                              \
    typedef typename internal::IdxComputationTraits<Rank, isRowMajor>::type ToGlobal;         \
                                                                                              \
    return THIS->access( ToGlobal::idx(THIS->rdims(), indices) );                             \
  }                                                                                           \
                                                                                              \
  template<class Idx_t>                                                                       \
  reference operator() (Idx_t const indices[])                                                \
  {                                                                                           \
    internal::BoundCheck<Rank>::check(THIS->rdims(), indices);                                \
                                                                                              \
    typedef typename internal::IdxComputationTraits<Rank, isRowMajor>::type ToGlobal;         \
                                                                                              \
    return THIS->access( ToGlobal::idx(THIS->rdims(), indices) );                             \
  }                                                                                           \
                                                                                              \
  const_reference operator() (MA_EXPAND_ARGS(P_rank, size_type)) const                        \
  {                                                                                           \
    MA_STATIC_CHECK(Rank==P_rank, INVALID_NUMBER_OF_ARGS_IN_CALL_OP);                         \
                                                                                              \
    size_type const indices[] = {MA_EXPAND_SEQ(P_rank)};                                      \
                                                                                              \
    internal::BoundCheck<Rank>::check(CONST_THIS->rdims(), indices);                          \
                                                                                              \
    typedef typename internal::IdxComputationTraits<Rank, isRowMajor>::type ToGlobal;         \
                                                                                              \
    return CONST_THIS->access( ToGlobal::idx(CONST_THIS->rdims(), indices) );                 \
  }                                                                                           \
                                                                                              \
  template<class Idx_t>                                                                       \
  const_reference operator() (Idx_t const indices[]) const                                    \
  {                                                                                           \
    internal::BoundCheck<Rank>::check(CONST_THIS->rdims(), indices);                          \
                                                                                              \
    typedef typename internal::IdxComputationTraits<Rank, isRowMajor>::type ToGlobal;         \
                                                                                              \
    return CONST_THIS->access( ToGlobal::idx(CONST_THIS->rdims(), indices) );                 \
  }                                                                                           \
                                                                                              \
	typename internal::Proxy<Rank-1,Rank,size_type,Self&>::type                                 \
	operator[] (size_type i)                                                                    \
	{                                                                                           \
		return internal::Proxy<Rank-1,Rank,size_type,Self&>(i, *this);                            \
	}                                                                                           \
                                                                                              \
	typename internal::Proxy<Rank-1,Rank,size_type,Self const&>::type                           \
	operator[] (size_type i) const                                                              \
	{                                                                                           \
		return internal::Proxy<Rank-1,Rank,size_type,Self const&>(i, *this);                      \
	}                                                                                           \
                                                                                              \
                                                                                              \
  inline                                                                                      \
  reference access(size_type i)                                                               \
  { return THIS->access(i);}                                                                  \
                                                                                              \
  inline                                                                                      \
  const_reference access(size_type i) const                                                   \
  { return CONST_THIS->access(i);}                                                            \
                                                                                              \
  size_type* rdims()                                                                          \
  { return THIS->rdims(); }                                                                   \
                                                                                              \
  size_type const* rdims() const                                                              \
  { return CONST_THIS->rdims(); }                                                             \
                                                                                              \
  size_type maxDim() const                                                                    \
  {                                                                                           \
    int m = 0;                                                                                \
    for (int i = 0; i < Rank; ++i)                                                            \
      if ( CONST_THIS->dim(i) > m)                                                            \
        m = CONST_THIS->dim(i);                                                               \
    return m;                                                                                 \
  }                                                                                           \
                                                                                              \
                                                                                              \
                                                                                              \
                                                                                              \
};


IMPLEMENT_BASE(1)
IMPLEMENT_BASE(2)
IMPLEMENT_BASE(3)
IMPLEMENT_BASE(4)
IMPLEMENT_BASE(5)
IMPLEMENT_BASE(6)
IMPLEMENT_BASE(7)
IMPLEMENT_BASE(8)
IMPLEMENT_BASE(9)
IMPLEMENT_BASE(10)

#undef IMPLEMENT_BASE


#undef THIS
#undef CONST_THIS

//              db
//             d88b
//            d8'`8b
//           d8'  `8b     8b,dPPYba, 8b,dPPYba, ,adPPYYba, 8b       d8
//          d8YaaaaY8b    88P'   "Y8 88P'   "Y8 ""     `Y8 `8b     d8'
//         d8""""""""8b   88         88         ,adPPPPP88  `8b   d8'
//        d8'        `8b  88         88         88,    ,88   `8b,d8'
//       d8'          `8b 88         88         `"8bbdP"Y8     Y88'
//                                                             d8'
//                                                            d8'

template<typename P_type, int P_rank, Options P_opts = MA_DEFAULT_MAJOR, typename P_MemBlock = std::vector<P_type> >
class GenericN : public P_MemBlock, public ArrayBase<GenericN<P_type, P_rank, P_opts, P_MemBlock>, P_rank, P_opts>
{
protected:
  typedef P_MemBlock                        Base1;
  typedef ArrayBase<GenericN,P_rank,P_opts> Base2;

  typedef typename Base2::reference reference;
  typedef typename Base2::const_reference const_reference;
  typedef typename Base2::size_type size_type;

  friend class ArrayBase<GenericN,P_rank,P_opts>;

//  using P_MemBlock::m_data;

public:

  typedef P_type UserT;
  static const int Rank = Base2::Rank;
  static const bool isRowMajor = Base2::isRowMajor;

protected:
  size_type m_rdims[Rank];   // size of each rank

  // user can't use this
  using Base1::resize;

public:
  using Base2::operator[];

  GenericN() : Base1(), Base2(), m_rdims() {};
  //Array(Array const& ) = default;
  //Array& operator<< (Array const&) = default;

  int rank() const
  { return Rank; }

  size_type dim(size_type r) const
  {
    internal::assertTrue(r < (size_type)Rank, "**ERROR**: Array<>: invalid index in function `size()`");
    return m_rdims[r];
  }
  size_type size() const
  { return Base1::size(); }



  inline
  reference access(size_type i)
  { return P_MemBlock::operator[] (i);}

  inline
  const_reference access(size_type i) const
  { return P_MemBlock::operator[] (i);}

  inline
  reference access_check(size_type i)
  { return P_MemBlock::at(i);}

  inline
  const_reference access_check(size_type i) const
  { return P_MemBlock::at(i);}


protected:
  // vecotr with rank sizes
  size_type* rdims()
  { return m_rdims; }

  size_type const* rdims() const
  { return m_rdims; }

};


namespace internal
{
  template<class T>
  struct IsStaticArray { static const bool value = 0;};

  template<class T, std::size_t N>
  struct IsStaticArray<T[N]> { static const bool value = 1;};

}

template<typename P_type, int P_rank, Options P_opts = MA_DEFAULT_MAJOR, typename P_MemBlock = std::vector<P_type>, bool hasSizeLimit = internal::IsStaticArray<P_MemBlock>::value >
class Array : public GenericN<P_type,P_rank,P_opts,P_MemBlock>
{
  typedef GenericN<P_type,P_rank,P_opts,P_MemBlock> Base0;

  typedef typename Base0::Base1  Base1;
  typedef typename Base0::Base2  Base2;

  template<typename T, int R, Options O, typename M, bool L>
  friend class Array;

public:

  typedef typename Base2::reference        reference;
  typedef typename Base2::const_reference  const_reference;
  typedef typename Base2::iterator         iterator;
  typedef typename Base2::const_iterator   const_iterator;
  typedef typename Base2::size_type        size_type;
  typedef typename Base2::difference_type  difference_type;
  typedef typename Base2::pointer          pointer;
  typedef typename Base2::const_pointer    const_pointer;

  typedef P_type UserT;
  static const int Rank = P_rank;
  static const bool isRowMajor = P_opts & RowMajor;
  static const Options Opts = P_opts;

  Array() {};

  internal::ListInitializationSwitch<UserT, UserT*> operator<<(const_reference x)
  {
    return internal::ListInitializationSwitch<UserT, UserT*>(this->data(), x);
  }

  template<typename Q_MemBlock, bool Q_hasSizeLimit>
  Array(Array<UserT,Rank,Opts,Q_MemBlock,Q_hasSizeLimit> const& x)
  {
    this->resize(x.size());
    std::copy(x.begin(), x.end(), this->begin());
    std::copy(x.rdims(), x.rdims()+Rank, Base0::rdims());
  }

  template<class T>
  Array(T const new_dims[], UserT val)
  { reshape(new_dims, val); }

  template<class T>
  Array(T const new_dims[])
  { reshape(new_dims); }

  template<class T>
  void reshape(T const new_dims[], UserT val)
  {
    size_type new_size = 1;
    for (int i = 0; i < Rank; ++i)
    {
      internal::assertTrue(new_dims[i] > 0, "**ERROR**: Array<>: dimension must be greater than 0");
      Base0::m_rdims[i] = new_dims[i];
      new_size *= new_dims[i];
    }

    this->resize(new_size, val);
  }

  template<class T>
  void reshape(T const new_dims[])
  {
    size_type new_size = 1;
    for (int i = 0; i < Rank; ++i)
    {
      internal::assertTrue(new_dims[i] > 0, "**ERROR**: Array<>: dimension must be greater than 0");
      Base0::m_rdims[i] = new_dims[i];
      new_size *= new_dims[i];
    }

    this->resize(new_size);
  }

  void clear()
  {
    Base0::clear();
    for (int i=0; i<Rank; ++i)
    {
      Base0::m_rdims[i] = 0;
    }
  }

#define MA_IMPLEMENT_FUN(n_args)                                                                       \
  Array(MA_EXPAND_ARGS(n_args, size_type))                                                             \
  {                                                                                                    \
    MA_STATIC_CHECK(n_args == Rank, TOO_FEW_ARGUMENTS_IN_CONSTRUCTOR);                                 \
    reshape(MA_EXPAND_SEQ(n_args));                                                                    \
  }                                                                                                    \
                                                                                                       \
  void reshape(MA_EXPAND_ARGS(n_args, size_type))                                                      \
  {                                                                                                    \
    MA_STATIC_CHECK(n_args == Rank, TOO_FEW_ARGUMENTS_IN_RESHAPE);                                     \
    size_type const new_dims[] = { MA_EXPAND_SEQ(n_args) };                                            \
                                                                                                       \
    size_type new_size = 1;                                                                            \
    for (int i = 0; i < Rank; ++i)                                                                     \
    {                                                                                                  \
      internal::assertTrue(new_dims[i] > 0, "**ERROR**: Array<>: dimension must be greater than 0");   \
      Base0::m_rdims[i] = new_dims[i];                                                                 \
      new_size *= new_dims[i];                                                                         \
    }                                                                                                  \
                                                                                                       \
    this->resize(new_size);                                                                            \
  }

  MA_IMPLEMENT_FUN( 1)
  MA_IMPLEMENT_FUN( 2)
  MA_IMPLEMENT_FUN( 3)
  MA_IMPLEMENT_FUN( 4)
  MA_IMPLEMENT_FUN( 5)
  MA_IMPLEMENT_FUN( 6)
  MA_IMPLEMENT_FUN( 7)
  MA_IMPLEMENT_FUN( 8)
  MA_IMPLEMENT_FUN( 9)
  MA_IMPLEMENT_FUN(10)
#undef MA_IMPLEMENT_FUN

};


//
// Size-Limited version
//
template<typename P_type, int P_rank, Options P_opts, typename P_MemBlock >
class Array<P_type, P_rank, P_opts, P_MemBlock, true> : public ArrayBase< Array<P_type, P_rank, P_opts, P_MemBlock, true>, P_rank, P_opts>
{

  typedef ArrayBase<Array,P_rank,P_opts> Base;


  friend class ArrayBase<Array,P_rank,P_opts>;
  template<typename T, int R, Options O, typename M, bool L>
  friend class Array;

  // static checking
  template<class T, bool Err>
  struct ERROR_INCOMPATIBLE_TYPE_AND_STORAGE_TYPE;

  template<class T>
  struct ERROR_INCOMPATIBLE_TYPE_AND_STORAGE_TYPE<T,true> {};

  enum { Dummy1 = sizeof(ERROR_INCOMPATIBLE_TYPE_AND_STORAGE_TYPE<Array, Tr1::is_same<P_type,typename Tr1::remove_extent<P_MemBlock>::type>::value>) };


public:

  typedef typename Base::reference        reference;
  typedef typename Base::const_reference  const_reference;
  typedef typename Base::iterator         iterator;
  typedef typename Base::const_iterator   const_iterator;
  typedef typename Base::size_type        size_type;
  typedef typename Base::difference_type  difference_type;
  typedef typename Base::pointer          pointer;
  typedef typename Base::const_pointer    const_pointer;

  static const std::size_t MaxSize = sizeof(P_MemBlock)/sizeof(P_type);
  typedef P_type UserT;
  static const int Rank = Base::Rank;
  static const bool isRowMajor = Base::isRowMajor;
  static const Options Opts = P_opts;

private:
  UserT        m_data[MaxSize];
  size_type    m_size;
  size_type    m_rdims[Rank];


public:



  Array() : m_data(), m_size(), m_rdims() {};

  //Array(Array const& ) = default;
  //Array& operator<< (Array const&) = default;

  internal::ListInitializationSwitch<UserT, UserT*> operator<<(const_reference x)
  {
    return internal::ListInitializationSwitch<UserT, UserT*>(this->data(), x);
  }

  template<typename Q_MemBlock, bool Q_hasSizeLimit>
  Array(Array<UserT,Rank,Opts,Q_MemBlock,Q_hasSizeLimit> const& x) : m_size(x.size())
  {
    std::copy(x.begin(), x.end(), m_data);
    std::copy(x.m_rdims, x.m_rdims+Rank, m_rdims);
  }

  template<class T>
  Array(T const new_dims[], UserT val) : m_size()
  { reshape(new_dims, val); }

  template<class T>
  Array(T const new_dims[]) : m_size()
  { reshape(new_dims); }

  template<class T>
  void reshape(T const new_dims[], UserT const& val)
  {
    size_type new_size = 1;
    for (int i = 0; i < Rank; ++i)
    {
      internal::assertTrue(new_dims[i] > 0, "**ERROR**: Array<>: dimension must be greater than 0");
      m_rdims[i] = new_dims[i];
      new_size *= new_dims[i];
    }

    this->resize(new_size, val);
  }

  template<class T>
  void reshape(T const new_dims[])
  {
    size_type new_size = 1;
    for (int i = 0; i < Rank; ++i)
    {
      internal::assertTrue(new_dims[i] > 0, "**ERROR**: Array<>: dimension must be greater than 0");
      m_rdims[i] = new_dims[i];
      new_size *= new_dims[i];
    }

    this->resize(new_size);
  }
  
  void clear()
  {
    m_size = 0;
    for(int i=0; i<Rank; ++i)
    {
      m_rdims[i] = 0;
    }
  }


#define MA_IMPLEMENT_FUN(n_args)                                                                       \
  Array(MA_EXPAND_ARGS(n_args, size_type)) : m_size()                                                  \
  {                                                                                                    \
    MA_STATIC_CHECK(n_args == Rank, TOO_FEW_ARGUMENTS_IN_CONSTRUCTOR);                                 \
    reshape(MA_EXPAND_SEQ(n_args));                                                                    \
  }                                                                                                    \
                                                                                                       \
  void reshape(MA_EXPAND_ARGS(n_args, size_type))                                                      \
  {                                                                                                    \
    MA_STATIC_CHECK(n_args == Rank, TOO_FEW_ARGUMENTS_IN_RESHAPE);                                     \
    size_type const new_dims[] = { MA_EXPAND_SEQ(n_args) };                                            \
                                                                                                       \
    size_type new_size = 1;                                                                            \
    for (int i = 0; i < Rank; ++i)                                                                     \
    {                                                                                                  \
      internal::assertTrue(new_dims[i] > 0, "**ERROR**: Array<>: dimension must be greater than 0");   \
      m_rdims[i] = new_dims[i];                                                                        \
      new_size *= new_dims[i];                                                                         \
    }                                                                                                  \
    this->resize(new_size);                                                                            \
                                                                                                       \
  }

  MA_IMPLEMENT_FUN( 1)
  MA_IMPLEMENT_FUN( 2)
  MA_IMPLEMENT_FUN( 3)
  MA_IMPLEMENT_FUN( 4)
  MA_IMPLEMENT_FUN( 5)
  MA_IMPLEMENT_FUN( 6)
  MA_IMPLEMENT_FUN( 7)
  MA_IMPLEMENT_FUN( 8)
  MA_IMPLEMENT_FUN( 9)
  MA_IMPLEMENT_FUN(10)
#undef MA_IMPLEMENT_FUN



  //internal::ListInitializationSwitch<Array, UserT*> operator<<(UserT const& x)
  //{
  //  return internal::ListInitializationSwitch<Array, UserT*>(*this, x);
  //}

  int rank() const
  { return Rank; }

  size_type dim(size_type r) const
  {
    internal::assertTrue(r < (size_type)Rank, "**ERROR**: Array<>: invalid index in function `size()`");
    return m_rdims[r];
  }

  size_type size() const
  { return m_size; }

  pointer data()
  {return m_data; }

  const_pointer data() const
  {return m_data; }


  inline
  reference access(size_type i)
  { return m_data[i];}

  inline
  const_reference access(size_type i) const
  { return m_data[i];}

  inline
  iterator begin()
  { return m_data; }

  inline
  const_iterator begin() const
  { return m_data; }

  inline
  iterator end()
  { return m_data+m_size; }

  inline
  const_iterator end() const
  { return m_data+m_size; }

protected:

  void resize(size_type n)
  {
    internal::assertTrue(n <= MaxSize, "**ERROR**: Size limit exceeded");
    for (size_type i = m_size; i < n; ++i)
      m_data[i] = UserT();
    m_size = n;
  }

  void resize(size_type n, UserT const& val)
  {
    internal::assertTrue(n <= MaxSize, "**ERROR**: Size limit exceeded");
    for (size_type i = m_size; i < n; ++i)
      m_data[i] = val;
    m_size = n;
  }


  // vecotr with rank sizes
  size_type* rdims()
  { return m_rdims; }

  size_type const* rdims() const
  { return m_rdims; }
};


//               db
//              d88b
//             d8'`8b
//            d8'  `8b     88,dPYba,,adPYba,  ,adPPYYba, 8b,dPPYba,  ,adPPYba,
//           d8YaaaaY8b    88P'   "88"    "8a ""     `Y8 88P'    "8a I8[    ""
//          d8""""""""8b   88      88      88 ,adPPPPP88 88       d8  `"Y8ba,
//         d8'        `8b  88      88      88 88,    ,88 88b,   ,a8" aa    ]8I
//        d8'          `8b 88      88      88 `"8bbdP"Y8 88`YbbdP"'  `"YbbdP"'
//                                                       88
//                                                       88

template<typename P_type, int P_rank, Options P_opts = MA_DEFAULT_MAJOR >
class Amaps : public ArrayBase<Amaps<P_type,P_rank,P_opts>,P_rank,P_opts>
{

  typedef ArrayBase<Amaps,P_rank,P_opts> Base;

  typedef typename Base::reference        reference;
  typedef typename Base::const_reference  const_reference;
  typedef typename Base::iterator         iterator;
  typedef typename Base::const_iterator   const_iterator;
  typedef typename Base::size_type        size_type;
  typedef typename Base::difference_type  difference_type;
  typedef typename Base::pointer          pointer;
  typedef typename Base::const_pointer    const_pointer;

  friend class ArrayBase<Amaps,P_rank,P_opts>;

public:

  typedef P_type UserT;
  static const int Rank = P_rank;
  static const bool isRowMajor = P_opts & RowMajor;

private:
  UserT*       m_data;
  size_type    m_size;
  size_type    m_rdims[Rank];

  Amaps();
public:
  //Amaps(Amaps const& ) = default;
  //Amaps& operator<< (Amaps const&) = default;

  internal::ListInitializationSwitch<UserT, UserT*> operator<<(const_reference x)
  {
    return internal::ListInitializationSwitch<UserT, UserT*>(this->data(), x);
  }

#define MA_AMAPS_CONSTRUCTOR(n_args)                                                                 \
  Amaps(UserT* mapped, MA_EXPAND_ARGS(n_args, size_type))                                            \
  {                                                                                                  \
    MA_STATIC_CHECK(n_args == Rank, TOO_FEW_ARGUMENTS_IN_AMAPS_CONSTRUCTOR);                         \
                                                                                                     \
    size_type const new_dims[] = { MA_EXPAND_SEQ(n_args) };                                          \
    m_size = 1;                                                                                      \
    for (int i = 0; i < Rank; ++i)                                                                   \
    {                                                                                                \
      internal::assertTrue(new_dims[i] > 0, "**ERROR**: Amaps<>: dimension must be greater than 0"); \
      m_rdims[i] = new_dims[i];                                                                      \
      m_size *= new_dims[i];                                                                         \
    }                                                                                                \
                                                                                                     \
    if (mapped == NULL)                                                                              \
      throw std::runtime_error("**ERROR**: Amaps<>: null pointer");                                  \
    m_data = mapped;                                                                                 \
  }

  MA_AMAPS_CONSTRUCTOR( 1)
  MA_AMAPS_CONSTRUCTOR( 2)
  MA_AMAPS_CONSTRUCTOR( 3)
  MA_AMAPS_CONSTRUCTOR( 4)
  MA_AMAPS_CONSTRUCTOR( 5)
  MA_AMAPS_CONSTRUCTOR( 6)
  MA_AMAPS_CONSTRUCTOR( 7)
  MA_AMAPS_CONSTRUCTOR( 8)
  MA_AMAPS_CONSTRUCTOR( 9)
  MA_AMAPS_CONSTRUCTOR(10)
#undef MA_AMAPS_CONSTRUCTOR


  //internal::ListInitializationSwitch<Amaps, UserT*> operator<<(UserT const& x)
  //{
  //  return internal::ListInitializationSwitch<Amaps, UserT*>(*this, x);
  //}

  int rank() const
  { return Rank; }

  size_type dim(size_type r) const
  {
    internal::assertTrue(r < (size_type)Rank, "**ERROR**: Amaps<>: invalid index in function `size()`");
    return m_rdims[r];
  }

  size_type size() const
  { return m_size; }

  pointer data()
  {return m_data; }

  const_pointer data() const
  {return m_data; }


  inline
  reference access(size_type i)
  { return m_data[i];}

  inline
  const_reference access(size_type i) const
  { return m_data[i];}

  inline
  reference access_check(size_type i)
  { return m_data[i];}

  inline
  const_reference access_check(size_type i) const
  { return m_data[i];}


protected:

  // vecotr with rank sizes
  size_type* rdims()
  { return m_rdims; }

  size_type const* rdims() const
  { return m_rdims; }
};

namespace internal
{

template<class T, int A, Options O, class M>
struct Traits<GenericN<T,A,O,M> > {
  typedef T UserT;
  typedef M MemBlockT;

  typedef typename MemBlockT::reference        reference;
  typedef typename MemBlockT::const_reference  const_reference;
  typedef typename MemBlockT::iterator         iterator;
  typedef typename MemBlockT::const_iterator   const_iterator;
  typedef typename MemBlockT::size_type        size_type;
  typedef typename MemBlockT::difference_type  difference_type;
  typedef typename MemBlockT::pointer          pointer;
  typedef typename MemBlockT::const_pointer    const_pointer;

};

template<typename T, int R, Options O, typename M>
struct Traits<Array<T,R,O,M,true> > {

  typedef T UserT;

  typedef  UserT&          reference;
  typedef  UserT const&    const_reference;
  typedef  UserT*          iterator;
  typedef  UserT const*    const_iterator;
  typedef  std::size_t     size_type;
  typedef  std::ptrdiff_t  difference_type;
  typedef  UserT*          pointer;
  typedef  UserT const*    const_pointer;

};


template<class T, int A, Options O>
struct Traits<Amaps<T,A,O> > {
  typedef T UserT;

  typedef  UserT&          reference;
  typedef  UserT const&    const_reference;
  typedef  UserT*          iterator;
  typedef  UserT const*    const_iterator;
  typedef  std::size_t     size_type;
  typedef  std::ptrdiff_t  difference_type;
  typedef  UserT*          pointer;
  typedef  UserT const*    const_pointer;

};




}





} // endnamespace









#endif
