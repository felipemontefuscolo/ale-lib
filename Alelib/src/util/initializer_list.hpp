#ifndef ALELIB_INITIALIZER_LIST
#define ALELIB_INITIALIZER_LIST

namespace alelib
{

namespace details {

template<class T, int N> struct ListAux{};

// expand to `type a0, type a1, type a2, ... `
#define ALE_EXPAND_ARGS1(type_)                           type_ a0
#define ALE_EXPAND_ARGS2(type_)  ALE_EXPAND_ARGS1(type_), type_ a1
#define ALE_EXPAND_ARGS3(type_)  ALE_EXPAND_ARGS2(type_), type_ a2
#define ALE_EXPAND_ARGS4(type_)  ALE_EXPAND_ARGS3(type_), type_ a3
#define ALE_EXPAND_ARGS5(type_)  ALE_EXPAND_ARGS4(type_), type_ a4
#define ALE_EXPAND_ARGS6(type_)  ALE_EXPAND_ARGS5(type_), type_ a5
#define ALE_EXPAND_ARGS7(type_)  ALE_EXPAND_ARGS6(type_), type_ a6
#define ALE_EXPAND_ARGS8(type_)  ALE_EXPAND_ARGS7(type_), type_ a7
#define ALE_EXPAND_ARGS9(type_)  ALE_EXPAND_ARGS8(type_), type_ a8
#define ALE_EXPAND_ARGS10(type_) ALE_EXPAND_ARGS9(type_), type_ a9

#define ALE_EXPAND_ARGS_(N, type_) ALE_EXPAND_ARGS##N(type_)
#define ALE_EXPAND_ARGS(N, type_) ALE_EXPAND_ARGS_(N, type_)

// expand to `v[0]=a0; v[1]=a1; v[2]=a2; ...`
#define ALE_EXPAND_SEQ1                  v[0]=a0;
#define ALE_EXPAND_SEQ2  ALE_EXPAND_SEQ1 v[1]=a1;
#define ALE_EXPAND_SEQ3  ALE_EXPAND_SEQ2 v[2]=a2;
#define ALE_EXPAND_SEQ4  ALE_EXPAND_SEQ3 v[3]=a3;
#define ALE_EXPAND_SEQ5  ALE_EXPAND_SEQ4 v[4]=a4;
#define ALE_EXPAND_SEQ6  ALE_EXPAND_SEQ5 v[5]=a5;
#define ALE_EXPAND_SEQ7  ALE_EXPAND_SEQ6 v[6]=a6;
#define ALE_EXPAND_SEQ8  ALE_EXPAND_SEQ7 v[7]=a7;
#define ALE_EXPAND_SEQ9  ALE_EXPAND_SEQ8 v[8]=a8;
#define ALE_EXPAND_SEQ10 ALE_EXPAND_SEQ9 v[9]=a9;

#define ALE_EXPAND_SEQ_(N) ALE_EXPAND_SEQ##N
#define ALE_EXPAND_SEQ(N) ALE_EXPAND_SEQ_(N)

// N = num of args
#define ALE_IMPL_LISTAUX(N)                   \
    template<class T>                       \
    struct ListAux<T,N>                     \
    {                                       \
      T v[N];                               \
      inline ListAux(ALE_EXPAND_ARGS(N, T)) \
      {                                     \
        ALE_EXPAND_SEQ(N)                   \
      };                                    \
      inline operator T* () {return v;}     \
    };

ALE_IMPL_LISTAUX(1)
ALE_IMPL_LISTAUX(2)
ALE_IMPL_LISTAUX(3)
ALE_IMPL_LISTAUX(4)
ALE_IMPL_LISTAUX(5)
ALE_IMPL_LISTAUX(6)
ALE_IMPL_LISTAUX(7)
ALE_IMPL_LISTAUX(8)
ALE_IMPL_LISTAUX(9)


} // end namespace details

#define ALE_IMPL_LISTOF(N)                                \
                                                          \
template<class T>                                         \
details::ListAux<T,N> listOf(ALE_EXPAND_ARGS(N, T))       \
{                                                         \
  return details::ListAux<T,N>(ALE_EXPAND_ARGS(N, (T) )); \
}

ALE_IMPL_LISTOF(1)
ALE_IMPL_LISTOF(2)
ALE_IMPL_LISTOF(3)
ALE_IMPL_LISTOF(4)
ALE_IMPL_LISTOF(5)
ALE_IMPL_LISTOF(6)
ALE_IMPL_LISTOF(7)
ALE_IMPL_LISTOF(8)
ALE_IMPL_LISTOF(9)





}// end namespace alelib

#endif









































