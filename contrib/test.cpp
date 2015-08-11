#include <iostream>
#include <cstdio>
#include <cassert>

#include <deque>

#include <Array/array.hpp>

using namespace std;
using namespace marray;

typedef std::size_t Index;

#define KRONECKER2(i,j)   = ( i==j?1:0 )
#define KRONECKER3(i,j,k) = (i==j?(j==k?1:0):0)

template<class T, Options Mj, class S>
void test_constructors()
{
  printf("test_constructors() ... ");

  Array<T, 3, Mj, S>  A;
  Array<T, 1, Mj, S>  B(5);
  Array<T, 2, Mj, S>  C(2,3);
  Array<T, 3, Mj, S>  D(2,3,4);
  Array<T, 4, Mj, S>  E(2,3,4,3);
  Array<T, 5, Mj, S>  F(2,3,4,3,2);
  Array<T, 6, Mj, S>  G(2,3,4,3,2,1);
  Array<T, 7, Mj, S>  H(2,3,4,3,2,1,2);
  Array<T, 8, Mj, S>  I(2,3,4,3,2,1,2,3);
  Array<T, 9, Mj, S>  J(2,3,4,3,2,1,2,3,4);

  assert( A.size() == 0    );
  assert( B.size() == 5    );
  assert( C.size() == 6    );
  assert( D.size() == 24   );
  assert( E.size() == 72   );
  assert( F.size() == 144  );
  assert( G.size() == 144  );
  assert( H.size() == 288  );
  assert( I.size() == 864  );
  assert( J.size() == 3456 );


#ifdef DEBUG
  // must throw
  try {
    A(1,2,3);
    printf("error ...\n");
    throw;
  }
  catch (...)
  { }

  // must not throw
  try {
    D(1,2,3);
  }
  catch (...)
  { printf("error ...\n"); throw; }
#endif


#define LOOP_TEST(M) \
  for (int i = 0; (Index)i < M.size(); ++i) \
  { \
    assert(M.access(i) == 0); \
    M.access(i) = i; \
    assert(M.access(i) == i); \
  }

  LOOP_TEST(A)
  LOOP_TEST(B)
  LOOP_TEST(C)
  LOOP_TEST(D)
  LOOP_TEST(E)
  LOOP_TEST(F)
  LOOP_TEST(G)
  LOOP_TEST(H)
  LOOP_TEST(I)
  LOOP_TEST(J)

  assert(A.dim(0) == 0);
  assert(A.dim(1) == 0);
  assert(A.dim(2) == 0);

  assert(J.dim(0) == 2);
  assert(J.dim(1) == 3);
  assert(J.dim(2) == 4);
  assert(J.dim(3) == 3);
  assert(J.dim(4) == 2);
  assert(J.dim(5) == 1);
  assert(J.dim(6) == 2);
  assert(J.dim(7) == 3);
  assert(J.dim(8) == 4);

#undef LOOP_TEST

}

template<class S>
void test_RowMajor()
{
  printf("test_RowMajor() ... ");

  // 3x3 test
  {
    Array<double, 3, RowMajor, S>  A(2,3,4);  // RowMajor by default

    assert(A.size() == 24);

    for (Index i = 0; i < A.size(); ++i)
      A.access(i) = i;

    int accum = 0;
    for (Index i = 0; i < A.dim(0); ++i)
      for (Index j = 0; j < A.dim(1); ++j)
        for (Index k = 0; k < A.dim(2); ++k)
        {
          assert( A(i,j,k)          ==  accum);
          assert( A[i][j][k]        ==  accum);
          assert( A(listify(i,j,k).v) ==  accum++);
        }

    Array<double, 3, RowMajor, S> const B(A);  // RowMajor by default

    accum = 0;
    for (Index i = 0; i < B.dim(0); ++i)
      for (Index j = 0; j < B.dim(1); ++j)
        for (Index k = 0; k < B.dim(2); ++k)
        {
          assert( B(i,j,k)   ==  accum);
          assert( B[i][j][k] ==  accum++);
        }

    accum = 0;
    for (Index i = 0; i < A.dim(0); ++i)
      for (Index j = 0; j < A.dim(1); ++j)
        for (Index k = 0; k < A.dim(2); ++k)
        {
          A[i][j][k] = accum--;
          assert( A[i][j][k] ==  A(i,j,k));
        }

    for (Index i = 0; i < A.size(); ++i)
    {
      //printf("AQUIIIIIIII %f %d\n", A.access(i), -int(i));
      assert( A.access(i) == -int(i) );
    }

  }

  // 2x2 test
  {
    Array<double, 2, RowMajor, S>  A(2,3);  // RowMajor by default

    assert(A.size() == 6);

    for (Index i = 0; i < A.size(); ++i)
      A.access(i) = i;

    int accum = 0;
    for (Index i = 0; i < A.dim(0); ++i)
      for (Index j = 0; j < A.dim(1); ++j)
        {
          assert( A(i,j)   ==  accum);
          assert( A[i][j]  ==  accum++);
        }

    Array<double, 2, RowMajor, S> const B(A);  // RowMajor by default

    accum = 0;
    for (Index i = 0; i < B.dim(0); ++i)
      for (Index j = 0; j < B.dim(1); ++j)
        {
          assert( B(i,j)   ==  accum);
          assert( B[i][j]  ==  accum++);
        }

    accum = 0;
    for (Index i = 0; i < A.dim(0); ++i)
      for (Index j = 0; j < A.dim(1); ++j)
        {
          A[i][j] = accum--;
          assert( A[i][j] ==  A(i,j));
        }

    for (Index i = 0; i < A.size(); ++i)
      assert( A.access(i) == -int(i) );

  }

  // 1x1 test
  {
    Array<double, 1, RowMajor, S>  A(2);  // RowMajor by default

    assert(A.size() == 2);

    for (Index i = 0; i < A.size(); ++i)
      A.access(i) = i;

    int accum = 0;
    for (Index i = 0; i < A.dim(0); ++i)
        {
          assert( A(i)   ==  accum);
          assert( A[i]  ==  accum++);
        }

    Array<double, 1, RowMajor, S> const B(A);  // RowMajor by default

    accum = 0;
    for (Index i = 0; i < B.dim(0); ++i)
        {
          assert( B(i)   ==  accum);
          assert( B[i]  ==  accum++);
        }

    accum = 0;
    for (Index i = 0; i < A.dim(0); ++i)
        {
          A[i] = accum--;
          assert( A[i] ==  A(i));
        }

    for (Index i = 0; i < A.size(); ++i)
      assert( A.access(i) == -int(i) );

  }


}

template<class S>
void test_ColMajor()
{
  printf("test_ColMajor() ... ");

  // 3x3 test
  {
    Array<double, 3, ColMajor, S>  A(2,3,4);  // RowMajor by default

    assert(A.size() == 24);

    for (Index i = 0; i < A.size(); ++i)
      A.access(i) = i;

    int accum = 0;
    for (Index k = 0; k < A.dim(2); ++k)
      for (Index j = 0; j < A.dim(1); ++j)
        for (Index i = 0; i < A.dim(0); ++i)
        {
          assert( A(i,j,k)   ==  accum);
          assert( A[i][j][k] ==  accum++);
        }

    Array<double, 3, ColMajor, S> const B(A);  // RowMajor by default

    accum = 0;
    for (Index k = 0; k < B.dim(2); ++k)
      for (Index j = 0; j < B.dim(1); ++j)
        for (Index i = 0; i < B.dim(0); ++i)
        {
          assert( B(i,j,k)   ==  accum);
          assert( B[i][j][k] ==  accum++);
        }

    accum = 0;
    for (Index k = 0; k < A.dim(2); ++k)
      for (Index j = 0; j < A.dim(1); ++j)
        for (Index i = 0; i < A.dim(0); ++i)
        {
          A[i][j][k] = accum--;
          assert( A[i][j][k] ==  A(i,j,k));
        }

    for (Index i = 0; i < A.size(); ++i)
      assert( A.access(i) == -int(i) );

  }

  // 2x2 test
  {
    Array<double, 2, ColMajor, S>  A(2,3);  // RowMajor by default

    assert(A.size() == 6);

    for (Index i = 0; i < A.size(); ++i)
      A.access(i) = i;

    int accum = 0;
    for (Index j = 0; j < A.dim(1); ++j)
      for (Index i = 0; i < A.dim(0); ++i)
        {
          assert( A(i,j)   ==  accum);
          assert( A[i][j]  ==  accum++);
        }

    Array<double, 2, ColMajor, S> const B(A);  // RowMajor by default

    accum = 0;
    for (Index j = 0; j < B.dim(1); ++j)
      for (Index i = 0; i < B.dim(0); ++i)
        {
          assert( B(i,j)   ==  accum);
          assert( B[i][j]  ==  accum++);
        }

    accum = 0;
    for (Index j = 0; j < A.dim(1); ++j)
      for (Index i = 0; i < A.dim(0); ++i)
        {
          A[i][j] = accum--;
          assert( A[i][j] ==  A(i,j));
        }

    for (Index i = 0; i < A.size(); ++i)
      assert( A.access(i) == -int(i) );

  }

  // 1x1 test
  {
    Array<double, 1, ColMajor, S>  A(2);  // RowMajor by default

    assert(A.size() == 2);

    for (Index i = 0; i < A.size(); ++i)
      A.access(i) = i;

    int accum = 0;
    for (Index i = 0; i < A.dim(0); ++i)
        {
          assert( A(i)  ==  accum);
          assert( A[i]  ==  accum++);
        }

    Array<double, 1, ColMajor, S> const B(A);  // RowMajor by default

    accum = 0;
    for (Index i = 0; i < B.dim(0); ++i)
        {
          assert( B(i)  ==  accum);
          assert( B[i]  ==  accum++);
        }

    accum = 0;
    for (Index i = 0; i < A.dim(0); ++i)
        {
          A[i] = accum--;
          assert( A[i] ==  A(i));
        }

    for (Index i = 0; i < A.size(); ++i)
      assert( A.access(i) == -int(i) );

  }

}

template<class S>
void test_InitializerRowM()
{
  printf("test_InitializerRowM() ... ");

  Array<double, 3, RowMajor, S> A(2,3,4);

  A << 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23;

  int accum = 0;
  for (Index i = 0; i < A.dim(0); ++i)
    for (Index j = 0; j < A.dim(1); ++j)
      for (Index k = 0; k < A.dim(2); ++k)
        assert( A(i,j,k) ==  accum++);


}

template<class S>
void test_InitializerColM()
{
  printf("test_InitializerColM() ... ");

  Array<double, 3, ColMajor, S> B(2,3,4);

  B << 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23;

  int accum = 0;
  for (Index k = 0; k < B.dim(2); ++k)
    for (Index j = 0; j < B.dim(1); ++j)
      for (Index i = 0; i < B.dim(0); ++i)
        assert( B(i,j,k) ==  accum++);


}

void test_CopyConstructor()
{
  printf("test_CopyConstructor() ... ");

  {
    Array<double, 3> B(2,3,4);

    B << 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23;

    Array<double, 3> A(B);

    int accum = 0;
    for (Index i = 0; i < A.dim(0); ++i)
      for (Index j = 0; j < A.dim(1); ++j)
        for (Index k = 0; k < A.dim(2); ++k)
          assert( A(i,j,k) ==  accum++);

    assert(A.dim(0)*A.dim(1)*A.dim(2) == 24);
    assert(A.size() == 24);

  }

  {
    Array<double, 3, RowMajor, double[24]> B(2,3,4);

    B << 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23;

    Array<double, 3, RowMajor, double[24]> A(B);

    assert(A.dim(0)*A.dim(1)*A.dim(2) == 24);
    assert(A.size() == 24);

    int accum = 0;
    for (Index i = 0; i < A.dim(0); ++i)
      for (Index j = 0; j < A.dim(1); ++j)
        for (Index k = 0; k < A.dim(2); ++k)
          assert( A(i,j,k) ==  accum++);

  }

  {
    Array<double, 3> B(2,3,4);

    B << 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23;

    Array<double, 3, RowMajor, double[24]> A(B);

    assert(A.dim(0)*A.dim(1)*A.dim(2) == 24);
    assert(A.size() == 24);
    
    int accum = 0;
    for (Index i = 0; i < A.dim(0); ++i)
      for (Index j = 0; j < A.dim(1); ++j)
        for (Index k = 0; k < A.dim(2); ++k)
          assert( A(i,j,k) ==  accum++);
          
  }

  {
    Array<double, 3, RowMajor, double[24]> B(2,3,4);

    B << 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23;

    Array<double, 3> A(B);

    assert(A.dim(0)*A.dim(1)*A.dim(2) == 24);
    assert(A.size() == 24);

    int accum = 0;
    for (Index i = 0; i < A.dim(0); ++i)
      for (Index j = 0; j < A.dim(1); ++j)
        for (Index k = 0; k < A.dim(2); ++k)
          assert( A(i,j,k) ==  accum++);

  }

  // Col Major
  // Col Major
  // Col Major

  {
    Array<double, 3, ColMajor> B(2,3,4);

    B << 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23;
         
    Array<double, 3, ColMajor> A(B);

    for (Index i = 0; i < A.dim(0); ++i)
      for (Index j = 0; j < A.dim(1); ++j)
        for (Index k = 0; k < A.dim(2); ++k)
          assert( A(i,j,k) ==  B(i,j,k));

    assert(A.dim(0)*A.dim(1)*A.dim(2) == 24);
    assert(A.size() == 24);

  }

  {
    Array<double, 3, ColMajor, double[24]> B(2,3,4);

    B << 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23;

    Array<double, 3, ColMajor, double[24]> A(B);

    assert(A.dim(0)*A.dim(1)*A.dim(2) == 24);
    assert(A.size() == 24);

    //int accum = 0;
    for (Index i = 0; i < A.dim(0); ++i)
      for (Index j = 0; j < A.dim(1); ++j)
        for (Index k = 0; k < A.dim(2); ++k)
          assert( A(i,j,k) ==  B(i,j,k));

  }

  {
    Array<double, 3, ColMajor> B(2,3,4);

    B << 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23;

    Array<double, 3, ColMajor, double[24]> A(B);

    assert(A.dim(0)*A.dim(1)*A.dim(2) == 24);
    assert(A.size() == 24);
    
    //int accum = 0;
    for (Index i = 0; i < A.dim(0); ++i)
      for (Index j = 0; j < A.dim(1); ++j)
        for (Index k = 0; k < A.dim(2); ++k)
          assert( A(i,j,k) ==  B(i,j,k));
          
  }

  {
    Array<double, 3, ColMajor, double[24]> B(2,3,4);

    B << 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23;

    Array<double, 3, ColMajor> A(B);

    assert(A.dim(0)*A.dim(1)*A.dim(2) == 24);
    assert(A.size() == 24);

    //int accum = 0;
    for (Index i = 0; i < A.dim(0); ++i)
      for (Index j = 0; j < A.dim(1); ++j)
        for (Index k = 0; k < A.dim(2); ++k)
          assert( A(i,j,k) ==  B(i,j,k));

  }



}

void test_Assign()
{
  printf("test_Assign() ... ");

  {
    Array<double, 3> B(2,3,4);

    B << 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23;

    Array<double, 3> A;

    A = B;

    int accum = 0;
    for (Index i = 0; i < A.dim(0); ++i)
      for (Index j = 0; j < A.dim(1); ++j)
        for (Index k = 0; k < A.dim(2); ++k)
        {
          assert( A(i,j,k) ==  accum++);
          A(i,j,k) = -1;
        }

    for (Index i = 0; i < B.dim(0); ++i)
      for (Index j = 0; j < B.dim(1); ++j)
        for (Index k = 0; k < B.dim(2); ++k)
          assert( B(i,j,k) >= 0  );
  }

  {
    Array<double, 3, RowMajor, double[24]> B(2,3,4);

    B << 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23;

    Array<double, 3, RowMajor, double[24]> A;

    A = B;

    int accum = 0;
    for (Index i = 0; i < A.dim(0); ++i)
      for (Index j = 0; j < A.dim(1); ++j)
        for (Index k = 0; k < A.dim(2); ++k)
        {
          assert( A(i,j,k) ==  accum++);
          A(i,j,k) = -1;
        }

    for (Index i = 0; i < B.dim(0); ++i)
      for (Index j = 0; j < B.dim(1); ++j)
        for (Index k = 0; k < B.dim(2); ++k)
          assert( B(i,j,k) >= 0  );
          
    assert(A.dim(0)*A.dim(1)*A.dim(2) == 24);
    assert(A.size() == 24);

  }

  {
    Array<double, 3, RowMajor, double[24]> B(2,3,4);

    B << 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23;

    Array<double, 3> A;

    A = B;

    int accum = 0;
    for (Index i = 0; i < A.dim(0); ++i)
      for (Index j = 0; j < A.dim(1); ++j)
        for (Index k = 0; k < A.dim(2); ++k)
        {
          assert( A(i,j,k) ==  accum++);
          A(i,j,k) = -1;
        }

    for (Index i = 0; i < B.dim(0); ++i)
      for (Index j = 0; j < B.dim(1); ++j)
        for (Index k = 0; k < B.dim(2); ++k)
          assert( B(i,j,k) >= 0  );
  }

  {
    Array<double, 3, RowMajor, std::vector<double> > B(2,3,4);

    B << 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23;

    Array<double, 3, RowMajor, double[24]> A;

    A = B;

    int accum = 0;
    for (Index i = 0; i < A.dim(0); ++i)
      for (Index j = 0; j < A.dim(1); ++j)
        for (Index k = 0; k < A.dim(2); ++k)
        {
          assert( A(i,j,k) ==  accum++);
          A(i,j,k) = -1;
        }

    for (Index i = 0; i < B.dim(0); ++i)
      for (Index j = 0; j < B.dim(1); ++j)
        for (Index k = 0; k < B.dim(2); ++k)
          assert( B(i,j,k) >= 0  );
  }

}

template<class S>
void test_Reshape()
{
  printf("test_Reshape() ... ");

  // Array Dynamic
  {

    Array<double, 3, RowMajor, S> B(2,3,4);

    B << 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23;

    B.reshape(4,3,1);

    assert(B.dim(0) == 4);
    assert(B.dim(1) == 3);
    assert(B.dim(2) == 1);
    assert(B.rank() == 3);
    assert(B.size() == 12);

    int accum = 0;
    for (Index i = 0; i < B.dim(0); ++i)
      for (Index j = 0; j < B.dim(1); ++j)
        for (Index k = 0; k < B.dim(2); ++k)
          assert( B(i,j,k) ==  accum++);

    B.reshape(4,4,4);

    assert(B.dim(0) == 4);
    assert(B.dim(1) == 4);
    assert(B.dim(2) == 4);

    assert(B.size() == 64);

    accum = 0;
    for (Index i = 0; i < B.dim(0); ++i)
      for (Index j = 0; j < B.dim(1); ++j)
        for (Index k = 0; k < B.dim(2); ++k)
          B(i,j,k) =  accum++;

    for (Index i = 0; i < B.size(); ++i)
      assert(B.access(i) == i);


    double const some_val = 1.25;
    B.reshape(&listify(8,4,4)[0], some_val);
    // or
    // B.reshape(listify(8,4,4).v, some_val);
    for (int i = 0; (Index)i < B.dim(0); ++i)
      for (int j = 0; (Index)j < B.dim(1); ++j)
        for (int k = 0; (Index)k < B.dim(2); ++k)
        {
          if (i<4)
            // WARNING: it is not true for other dimensions because the elements
            // is stored in a contiguous memory block. It is not true
            // for ColMajor too.
            if (i%2 == 0)
              assert(B(i,j,k) == i*B.dim(1)*B.dim(2) + j*B.dim(2) + k); // testing both interfaces
            else
              assert(B(listify(i,j,k).v) == i*B.dim(1)*B.dim(2) + j*B.dim(2) + k);
          else
            assert(B(i,j,k) == some_val);
        }

    B.clear();
    for (unsigned i=0; i<3; ++i)
      assert(B.dim(i) == 0);

    B.reshape(listify(2,3,4).v, -1);
    assert(B.size() == 24);
    for (int i=0; (Index)i < B.dim(0); ++i)
      for (int j = 0; (Index)j < B.dim(1); ++j)
        for (int k = 0; (Index)k < B.dim(2); ++k)
          assert(B(i,j,k) == -1);
    
  }

}

void test_Amaps()
{
  printf("test_Amaps() ... ");

  Array<double, 3> B(2,3,4);

  B << 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23;

  Amaps<double, 2> A(B.data(), 6, 4);

  assert(A.size() == 24);
  assert(B.size() == 24);

  assert(A.dim(0) == 6);
  assert(A.dim(1) == 4);

  int acc = 0;
  for (Index i = 0; i < A.dim(0); ++i)
    for (Index j = 0; j < A.dim(1); ++j)
    {
      assert(A(i,j) == acc);
      assert(A[i][j] == acc);
      A[i][j] = 23 - acc;
      ++acc;
    }

  // check if reflects on B
  for (Index i = 0; i < B.size(); ++i)
    assert( B.access(i) == 23-i );

  Amaps<double, 2> C(A);

  assert(C.size() == 24);

  assert(C.dim(0) == 6);
  assert(C.dim(1) == 4);

  acc = 23;
  for (Index i = 0; i < C.dim(0); ++i)
    for (Index j = 0; j < C.dim(1); ++j)
    {
      assert(C(i,j) == acc);
      C(i,j) = 23 - acc;
      --acc;
    }

  // check if reflects on B
  for (Index i = 0; i < B.size(); ++i)
    assert( B.access(i) == i );

  new(&A) Amaps<double, 2>(B.data(), 12, 2);

  assert(A.size() == 24);
  assert(A.rank() == 2);
  assert(A.dim(0) == 12);
  assert(A.dim(1) == 2);

  A << -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1;

  for (Index i = 0; i < B.size(); ++i)
    assert( B.access(i) == -1 );


}

void test_deque()
{
  printf("test_deque() ... ");

  Array<Index, 3, RowMajor, std::deque<Index> > B(600,3,10);

  for(Index i=0; i<B.size(); ++i)
    B.access(i) = i;

  for(Index i=0; i<B.dim(0); ++i)
    for (Index j=0; j<B.dim(1); ++j)
      for (Index k=0; k<B.dim(2); ++k)
        assert(B(i,j,k) == i*B.dim(2)*B.dim(1) + j*B.dim(2) + k);


}

template<class S>
void test_iterators()
{
  typedef Array<Index, 3, RowMajor, S > Array_t;

  Array_t B(600,3,10);
  typename Array_t::iterator it;

  int c = 0;
  for(it = B.begin(); it < B.end(); ++it)
    *it = c++;

  for(Index i=0; i<B.dim(0); ++i)
    for (Index j=0; j<B.dim(1); ++j)
      for (Index k=0; k<B.dim(2); ++k)
        assert(B(i,j,k) == i*B.dim(2)*B.dim(1) + j*B.dim(2) + k);


  //Array<Index, 3, RowMajor, Index[3]> A;
}

#define TEST(fun_name) printf( #fun_name "() ... "); \
                       fun_name ();                  \
                       printf("OK\n");


int main()
{
  #define COMMA ,
  #define com ,
  TEST(test_constructors<double com RowMajor com std::vector<double> >);
  TEST(test_constructors<double com ColMajor com std::vector<double> >);
  TEST(test_constructors<int com RowMajor com std::vector<int> >      );
  TEST(test_constructors<int com ColMajor com std::vector<int> >      );
  TEST(test_constructors<double com RowMajor com double[3456] >       );
  TEST(test_constructors<double com ColMajor com double[3456] >       );
  TEST(test_constructors<int com RowMajor com int[3456] >             );
  TEST(test_constructors<int com ColMajor com int[3456] >             );
  TEST(test_RowMajor<double[24]>                                      );
  TEST(test_ColMajor<double[24]>                                      );
  TEST(test_RowMajor<std::vector<double> >                            );
  TEST(test_ColMajor<std::vector<double> >                            );
  TEST(test_InitializerRowM<double[24]>                               );
  TEST(test_InitializerColM<double[24]>                               );
  TEST(test_InitializerRowM<std::vector<double> >                     );
  TEST(test_InitializerColM<std::vector<double> >                     );
  TEST(test_CopyConstructor                                           );
  TEST(test_Assign                                                    );
  TEST(test_Reshape<std::vector<double> >                             );
  TEST(test_Reshape<double[128]>                                      );
  TEST(test_Amaps                                                     );
  TEST(test_deque                                                     );
  TEST(test_iterators<std::vector<Index> >                            );
  TEST(test_iterators<std::deque<Index> >                             );
  TEST(test_iterators<Index[18000] >                                  );

  printf("Everything seems OK \n");
}



