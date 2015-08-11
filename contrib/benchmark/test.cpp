#include <cstdio>
#include <ctime>
#define _SCL_SECURE_NO_WARNINGS
#define BOOST_DISABLE_ASSERTS
#include <boost/multi_array.hpp>
#include "Array/array.hpp"


void bench_10x10()
{
  const int X_SIZE = 10;
  const int Y_SIZE = 10;
  const int ITERATIONS = 10000000;
  clock_t startTime;
  clock_t endTime;

  printf("\naccess operator 10x10 matrices, %d iterations\n\n", ITERATIONS);

  // Create Array
  marray::Array<double, 2> MyArray(X_SIZE,Y_SIZE);

  // Create the boost array
  typedef boost::multi_array<double, 2> ImageArrayType;
  ImageArrayType boostMatrix(boost::extents[X_SIZE][Y_SIZE]);

  // Create the native array
  volatile double *nativeMatrix = new double [X_SIZE * Y_SIZE];

  
  
  //------------------Measure boost----------------------------------------------
  startTime = clock();
  for (int i = 0; i < ITERATIONS; ++i)
  {
      for (int x = 0; x < X_SIZE; ++x)
      {
          for (int y = 0; y < Y_SIZE; ++y)
          {
              boostMatrix[x][y] = 2.345;
          }
      }
  }
  endTime = clock();
  printf("[Boost]              Elapsed time: %6.3f seconds\n", double(endTime - startTime) / CLOCKS_PER_SEC);

  //------------------Measure Array subscript Fortran ----------------------------------------------
  startTime = clock();
  for (int i = 0; i < ITERATIONS; ++i)
  {
      for (int x = 0; x < X_SIZE; ++x)
      {
          for (int y = 0; y < Y_SIZE; ++y)
          {
              MyArray(x,y) = 2.345;
          }
      }
  }
  endTime = clock();
  printf("[Array (Fort style)] Elapsed time: %6.3f seconds\n", double(endTime - startTime) / CLOCKS_PER_SEC);    

  //------------------Measure Array subscript C----------------------------------------------
  startTime = clock();
  for (int i = 0; i < ITERATIONS; ++i)
  {
      for (int x = 0; x < X_SIZE; ++x)
      {
          for (int y = 0; y < Y_SIZE; ++y)
          {
              MyArray[x][y] = 2.345;
          }
      }
  }
  endTime = clock();
  printf("[Array (C style)]    Elapsed time: %6.3f seconds\n", double(endTime - startTime) / CLOCKS_PER_SEC);    

  //------------------Measure native-----------------------------------------------
  startTime = clock();
  for (int i = 0; i < ITERATIONS; ++i)
  {
      for (int x = 0; x < X_SIZE; ++x)
      {
          for (int y = 0; y < Y_SIZE; ++y)
          {
              nativeMatrix[x*Y_SIZE + y] = 2.345;
          }
      }
  }
  endTime = clock();
  printf("[Native]             Elapsed time: %6.3f seconds\n", double(endTime - startTime) / CLOCKS_PER_SEC);



  if (nativeMatrix)
    delete nativeMatrix;  
}

void bench_100x100()
{
  const int X_SIZE = 100;
  const int Y_SIZE = 100;
  const int ITERATIONS = 50000;
  clock_t startTime;
  clock_t endTime;

  printf("\naccess operator 100x100 matrices, %d iterations\n\n", ITERATIONS);

  // Create Array
  marray::Array<double, 2> MyArray(X_SIZE,Y_SIZE);

  // Create the boost array
  typedef boost::multi_array<double, 2> ImageArrayType;
  ImageArrayType boostMatrix(boost::extents[X_SIZE][Y_SIZE]);

  // Create the native array
  double *nativeMatrix = new double [X_SIZE * Y_SIZE];

  
  
  //------------------Measure boost----------------------------------------------
  startTime = clock();
  for (int i = 0; i < ITERATIONS; ++i)
  {
      for (int x = 0; x < X_SIZE; ++x)
      {
          for (int y = 0; y < Y_SIZE; ++y)
          {
              boostMatrix[x][y] = 2.345;
          }
      }
  }
  endTime = clock();
  printf("[Boost]              Elapsed time: %6.3f seconds\n", double(endTime - startTime) / CLOCKS_PER_SEC);

  //------------------Measure Array subscript Fortran ----------------------------------------------
  startTime = clock();
  for (int i = 0; i < ITERATIONS; ++i)
  {
      for (int x = 0; x < X_SIZE; ++x)
      {
          for (int y = 0; y < Y_SIZE; ++y)
          {
              MyArray(x,y) = 2.345;
          }
      }
  }
  endTime = clock();
  printf("[Array (Fort style)] Elapsed time: %6.3f seconds\n", double(endTime - startTime) / CLOCKS_PER_SEC);    

  //------------------Measure Array subscript C----------------------------------------------
  startTime = clock();
  for (int i = 0; i < ITERATIONS; ++i)
  {
      for (int x = 0; x < X_SIZE; ++x)
      {
          for (int y = 0; y < Y_SIZE; ++y)
          {
              MyArray[x][y] = 2.345;
          }
      }
  }
  endTime = clock();
  printf("[Array (C style)]    Elapsed time: %6.3f seconds\n", double(endTime - startTime) / CLOCKS_PER_SEC);    

  //------------------Measure native-----------------------------------------------
  startTime = clock();
  for (int i = 0; i < ITERATIONS; ++i)
  {
      for (int x = 0; x < X_SIZE; ++x)
      {
          for (int y = 0; y < Y_SIZE; ++y)
          {
              nativeMatrix[x*Y_SIZE + y] = 2.345;
          }
      }
  }
  endTime = clock();
  printf("[Native]             Elapsed time: %6.3f seconds\n", double(endTime - startTime) / CLOCKS_PER_SEC);



  if (nativeMatrix)
    delete nativeMatrix;  
}

void bench_3x3x3x3x3x3()
{
  const int N = 3;
  
  const int n0=N,n1=N,n2=N,n3=N,n4=N,n5=N;
  const int ITERATIONS = 5000000;
  clock_t startTime;
  clock_t endTime;

  printf("\naccess operator 3x3x3x3x3x3 matrices, %d iterations\n\n", ITERATIONS);

  // Create Array
  marray::Array<double, 6> MyArray(n0,n1,n2,n3,n4,n5);

  // Create the boost array
  typedef boost::multi_array<double, 6> ImageArrayType;
  ImageArrayType boostMatrix(boost::extents[n0][n1][n2][n3][n4][n5]);

  // Create the native array
  double *nativeMatrix = new double [N*N*N*N*N*N];

  
  
  //------------------Measure boost----------------------------------------------
  startTime = clock();
  for (int i = 0; i < ITERATIONS; ++i)
  {
    for (int i0 = 0; i0 < n0; ++i0)
      for (int i1 = 0; i1 < n1; ++i1)
        for (int i2 = 0; i2 < n2; ++i2)
          for (int i3 = 0; i3 < n3; ++i3)
            for (int i4 = 0; i4 < n4; ++i4)
              for (int i5 = 0; i5 < n5; ++i5)
                boostMatrix[i0][i1][i2][i3][i4][i5] = 2.345;
  }
  endTime = clock();
  printf("[Boost]              Elapsed time: %6.3f seconds\n", double(endTime - startTime) / CLOCKS_PER_SEC);

  //------------------Measure Array subscript Fortran ----------------------------------------------
  startTime = clock();
  for (int i = 0; i < ITERATIONS; ++i)
  {
    for (int i0 = 0; i0 < n0; ++i0)
      for (int i1 = 0; i1 < n1; ++i1)
        for (int i2 = 0; i2 < n2; ++i2)
          for (int i3 = 0; i3 < n3; ++i3)
            for (int i4 = 0; i4 < n4; ++i4)
              for (int i5 = 0; i5 < n5; ++i5)
                MyArray(i0,i1,i2,i3,i4,i5) = 2.345;
  }
  endTime = clock();
  printf("[Array (Fort style)] Elapsed time: %6.3f seconds\n", double(endTime - startTime) / CLOCKS_PER_SEC);    

  //------------------Measure Array subscript C----------------------------------------------
  startTime = clock();
  for (int i = 0; i < ITERATIONS; ++i)
  {
    for (int i0 = 0; i0 < n0; ++i0)
      for (int i1 = 0; i1 < n1; ++i1)
        for (int i2 = 0; i2 < n2; ++i2)
          for (int i3 = 0; i3 < n3; ++i3)
            for (int i4 = 0; i4 < n4; ++i4)
              for (int i5 = 0; i5 < n5; ++i5)    
                MyArray[i0][i1][i2][i3][i4][i5] = 2.345;
  }
  endTime = clock();
  printf("[Array (C style)]    Elapsed time: %6.3f seconds\n", double(endTime - startTime) / CLOCKS_PER_SEC);    

  //------------------Measure native-----------------------------------------------
  startTime = clock();
  for (int i = 0; i < ITERATIONS; ++i)
  {
    for (int i0 = 0; i0 < n0; ++i0)
      for (int i1 = 0; i1 < n1; ++i1)
        for (int i2 = 0; i2 < n2; ++i2)
          for (int i3 = 0; i3 < n3; ++i3)
            for (int i4 = 0; i4 < n4; ++i4)
              for (int i5 = 0; i5 < n5; ++i5)        
                nativeMatrix[i5 + n5*(i4 + n4*(i3 + n3*(i2 + n2*(i1 + n1*(i0)))))] = 2.345;
  }
  endTime = clock();
  printf("[Native]             Elapsed time: %6.3f seconds\n", double(endTime - startTime) / CLOCKS_PER_SEC);



  if (nativeMatrix)
    delete nativeMatrix;  
}


int main(int, char* [])
{
  bench_10x10();
  bench_100x100();
  bench_3x3x3x3x3x3();
  
  return 0;
}
