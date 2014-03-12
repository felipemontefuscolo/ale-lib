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


  //iterator begin()
  //{ return THIS->data(); }
  //
  //const_iterator begin() const
  //{ return CONST_THIS->data(); }
  //
  //iterator end()
  //{ return THIS->data() + THIS->size(); }
  //
  //const_iterator end() const
  //{ return CONST_THIS->data() + THIS->size(); }
  //
  //reference back()
  //{ return *(--end()); }
  //
  //const_reference back() const
  //{ return *(--end()); }
  //
  //reference front()
  //{ return *begin(); }
  //
  //const_reference front() const
  //{ return *begin(); }


// define call operator
#define MA_DEF_CALL_OP(n_args_)                                                       \
  reference operator() (MA_EXPAND_ARGS(n_args_, int))                                 \
  {                                                                                   \
    MA_STATIC_CHECK(Rank==n_args_, INVALID_NUMBER_OF_ARGS_IN_CALL_OP);                \
                                                                                      \
    int const indices[] = {MA_EXPAND_SEQ(n_args_)};                                   \
                                                                                      \
    internal::BoundCheck<Rank>::check(THIS->rdims(), indices);                        \
                                                                                      \
    typedef typename internal::IdxComputationTraits<Rank, isRowMajor>::type ToGlobal; \
                                                                                      \
    return THIS->operator[] ( ToGlobal::idx(THIS->rdims(), indices) );                \
  }

  // define call operators
  MA_DEF_CALL_OP(1)
  MA_DEF_CALL_OP(2)
  MA_DEF_CALL_OP(3)
  MA_DEF_CALL_OP(4)
  MA_DEF_CALL_OP(5)
  MA_DEF_CALL_OP(6)
  MA_DEF_CALL_OP(7)
  MA_DEF_CALL_OP(8)
  MA_DEF_CALL_OP(9)
  MA_DEF_CALL_OP(10)
#undef MA_DEF_CALL_OP


// define const call operator
#define MA_DEF_CONST_CALL_OP(n_args_)                                                 \
  const_reference operator() (MA_EXPAND_ARGS(n_args_, int)) const                     \
  {                                                                                   \
    MA_STATIC_CHECK(Rank==n_args_, INVALID_NUMBER_OF_ARGS_IN_CALL_OP);                \
                                                                                      \
    int const indices[] = {MA_EXPAND_SEQ(n_args_)};                                   \
                                                                                      \
    internal::BoundCheck<Rank>::check(CONST_THIS->rdims(), indices);                        \
                                                                                      \
    typedef typename internal::IdxComputationTraits<Rank, isRowMajor>::type ToGlobal; \
                                                                                      \
    return CONST_THIS->operator[] ( ToGlobal::idx(CONST_THIS->rdims(), indices) );                \
  };

  // define call operators
  MA_DEF_CONST_CALL_OP(1)
  MA_DEF_CONST_CALL_OP(2)
  MA_DEF_CONST_CALL_OP(3)
  MA_DEF_CONST_CALL_OP(4)
  MA_DEF_CONST_CALL_OP(5)
  MA_DEF_CONST_CALL_OP(6)
  MA_DEF_CONST_CALL_OP(7)
  MA_DEF_CONST_CALL_OP(8)
  MA_DEF_CONST_CALL_OP(9)
  MA_DEF_CONST_CALL_OP(10)
#undef MA_DEF_CONST_CALL_OP






// check bound

// define call operator
#define MA_DEF_CALL_OP(n_args_)                                                       \
  reference get(MA_EXPAND_ARGS(n_args_, int))                                          \
  {                                                                                   \
    MA_STATIC_CHECK(Rank==n_args_, INVALID_NUMBER_OF_ARGS_IN_CALL_OP);                \
                                                                                      \
    int const indices[] = {MA_EXPAND_SEQ(n_args_)};                                   \
                                                                                      \
    internal::BoundCheck<Rank>::check(THIS->rdims(), indices);                        \
                                                                                      \
    typedef typename internal::IdxComputationTraits<Rank, isRowMajor>::type ToGlobal; \
                                                                                      \
    return THIS->at( ToGlobal::idx(THIS->rdims(), indices) );                         \
  }

  // define call operators
  MA_DEF_CALL_OP(1)
  MA_DEF_CALL_OP(2)
  MA_DEF_CALL_OP(3)
  MA_DEF_CALL_OP(4)
  MA_DEF_CALL_OP(5)
  MA_DEF_CALL_OP(6)
  MA_DEF_CALL_OP(7)
  MA_DEF_CALL_OP(8)
  MA_DEF_CALL_OP(9)
  MA_DEF_CALL_OP(10)
#undef MA_DEF_CALL_OP


// define const call operator
#define MA_DEF_CONST_CALL_OP(n_args_)                                                 \
  const_reference get(MA_EXPAND_ARGS(n_args_, int)) const                              \
  {                                                                                   \
    MA_STATIC_CHECK(Rank==n_args_, INVALID_NUMBER_OF_ARGS_IN_CALL_OP);                \
                                                                                      \
    int const indices[] = {MA_EXPAND_SEQ(n_args_)};                                   \
                                                                                      \
    internal::BoundCheck<Rank>::check(CONST_THIS->rdims(), indices);                  \
                                                                                      \
    typedef typename internal::IdxComputationTraits<Rank, isRowMajor>::type ToGlobal; \
                                                                                      \
    return CONST_THIS->at( ToGlobal::idx(CONST_THIS->rdims(), indices) );    \
  };

  // define call operators
  MA_DEF_CONST_CALL_OP(1)
  MA_DEF_CONST_CALL_OP(2)
  MA_DEF_CONST_CALL_OP(3)
  MA_DEF_CONST_CALL_OP(4)
  MA_DEF_CONST_CALL_OP(5)
  MA_DEF_CONST_CALL_OP(6)
  MA_DEF_CONST_CALL_OP(7)
  MA_DEF_CONST_CALL_OP(8)
  MA_DEF_CONST_CALL_OP(9)
  MA_DEF_CONST_CALL_OP(10)
#undef MA_DEF_CONST_CALL_OP













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
  //Array(Array const& ) = default;
  //Array& operator= (Array const&) = default;

  internal::ListInitializationSwitch<Array, UserT*> operator=(UserT const& x)
  {
    return internal::ListInitializationSwitch<Array, UserT*>(*this, x);
  }

#define MA_ARRAY_CONSTRUCTOR(n_args) \
  Array(MA_EXPAND_ARGS(n_args, int)) \
  {                                  \
    reshape(MA_EXPAND_SEQ(n_args));  \
  }

  MA_ARRAY_CONSTRUCTOR(1)
  MA_ARRAY_CONSTRUCTOR(2)
  MA_ARRAY_CONSTRUCTOR(3)
  MA_ARRAY_CONSTRUCTOR(4)
  MA_ARRAY_CONSTRUCTOR(5)
  MA_ARRAY_CONSTRUCTOR(6)
  MA_ARRAY_CONSTRUCTOR(7)
  MA_ARRAY_CONSTRUCTOR(8)
  MA_ARRAY_CONSTRUCTOR(9)
  MA_ARRAY_CONSTRUCTOR(10)
#undef MA_ARRAY_CONSTRUCTOR


  int rank() const
  { return Rank; }

  int dim(int r) const
  {
    internal::assertTrue(r < Rank, "**ERROR**: Array<>: invalid index in function `size()`");
    return m_rdims[r];
  }
  int size() const
  { return Base1::size(); }

#define MA_RESHAPE(n_args)                                                                            \
  void reshape(MA_EXPAND_ARGS(n_args, int))                                                           \
  {                                                                                                   \
    MA_STATIC_CHECK(n_args == Rank, TOO_FEW_ARGUMENTS_IN_RESHAPE);                                    \
    int const new_dims[] = { MA_EXPAND_SEQ(n_args) };                                                 \
                                                                                                      \
    int new_size = 1;                                                                                 \
    for (int i = 0; i < Rank; ++i)                                                                    \
    {                                                                                                 \
      internal::assertTrue(new_dims[i] > 0, "**ERROR**: Array<>: dimension must be greater than 0");  \
      m_rdims[i] = new_dims[i];                                                                       \
      new_size *= new_dims[i];                                                                        \
    }                                                                                                 \
                                                                                                      \
    resize(new_size);                                                                                 \
  }

  // define `reshape`
  MA_RESHAPE(1)
  MA_RESHAPE(2)
  MA_RESHAPE(3)
  MA_RESHAPE(4)
  MA_RESHAPE(5)
  MA_RESHAPE(6)
  MA_RESHAPE(7)
  MA_RESHAPE(8)
  MA_RESHAPE(9)
  MA_RESHAPE(10)
#undef MA_RESHAPE


protected:

  // vecotr with rank sizes
  int* rdims()
  { return m_rdims; }

  int const* rdims() const
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

  Amaps();
public:
  //Amaps(Amaps const& ) = default;
  //Amaps& operator= (Amaps const&) = default;

#define MA_AMAPS_CONSTRUCTOR(n_args)                                                                 \
  Amaps(UserT* mapped, MA_EXPAND_ARGS(n_args, int))                                                  \
  {                                                                                                  \
    MA_STATIC_CHECK(n_args == Rank, TOO_FEW_ARGUMENTS_IN_AMAPS_CONSTRUCTOR);                         \
                                                                                                     \
    int const new_dims[] = { MA_EXPAND_SEQ(n_args) };                                                \
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
