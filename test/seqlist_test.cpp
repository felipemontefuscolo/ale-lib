// Copyright 2005, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.



#include <gtest/gtest.h>

#include "Alelib/src/mesh/labelable.hpp"
#include "Alelib/src/util/list_type.hpp"
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <vector>
#include <deque>

using namespace std;
using namespace alelib;

template<class V>
unsigned size(V const& v)
{
  typename V::const_iterator i = v.begin();
  int k=0;

  for (; i!= v.end(); ++i)
    ++k;
  return k;
}

template<class V>
std::string print_vector(V const& v, int size)
{
  std::stringstream ss;
  for (int i = 0; i < size; ++i)
    ss << v[i] << " ";
  ss << "\n";
  return ss.str();
}

class Dummy : public Labelable
{public:
  Dummy(int tag = 0, bool d = false) : Labelable(tag, Flags( DISABLED*d)), hist(0) {}
  int hist;
};




TEST(SeqListTest, TestStep0)
{
  int a[] = {0,1,2,3,  0,1,2,3,  0,1,2,3,  0,1,2,3,  0,1,2,3,  0,1,2,3}; // 6 x 4 = 24
  int a_size = sizeof(a)/sizeof(int);
  alelib::SeqList<std::vector<Dummy> > v0;
  alelib::SeqList<std::vector<Dummy> >::iterator it;
  alelib::SeqList<std::vector<Dummy> >::const_iterator cit;


  for (int i=0; i<a_size; ++i)
    v0.insert(Dummy(i)); // v0 = a


  // ================== disable ===================

  // disabling ? 3, except the last one
  for (int i = 3; i <a_size-1; i+=4)
    v0.disable(i);

  // v0 = {0,1,2,x,  0,1,2,x,  0,1,2,x,  0,1,2,x,  0,1,2,x,  0,1,2,3};
  
  EXPECT_EQ(19u, v0.size());

  v0.disable(a_size-1); // v0 = {0,1,2,x,  0,1,2,x,  0,1,2,x,  0,1,2,x,  0,1,2,x,  0,1,2,x};

  EXPECT_EQ(18u, v0.size());

  
 

  // ================== disable ===================


  // nothing
  v0.disable(a_size-1); // v0 = {0,1,2,x,  0,1,2,x,  0,1,2,x,  0,1,2,x,  0,1,2,x,  0,1,2,x};

  v0.disable(1); // v0 = {0,x,2,x,  0,1,2,x,  0,1,2,x,  0,1,2,x,  0,1,2,x,  0,1,2,x};

  EXPECT_EQ(17u, v0.size());

  v0.disable(21); // v0 = {0,x,2,x,  0,1,2,x,  0,1,2,x,  0,1,2,x,  0,1,2,x,  0,x,2,x};

  v0.disable(9); // v0 = {0,x,2,x,  0,1,2,x,  0,x,2,x,  0,1,2,x,  0,1,2,x,  0,x,2,x};

  // =====================================================

  // v0 = {0,x,2,x,  0,1,2,x,  0,x,2,x,  0,1,2,x,  0,1,2,x,  0,x,2,x};
  v0.disable(5);
  v0.disable(13);
  v0.disable(17);

  // v0 = {0,x,2,x,  0,x,2,x,  0,x,2,x,  0,x,2,x,  0,x,2,x,  0,x,2,x};

  // ================== insert =======================

  // v0 = {0,x,2,x,  0,x,2,x,  0,x,2,x,  0,x,2,x,  0,x,2,x,  0,x,2,x};
  int id;
  id = v0.insert(Dummy(3)); // v0 = {0,x,2,x,  0,x,2,x,  0,x,2,x,  0,x,2,x,  0,x,2,x,  0,x,2,3};

  EXPECT_EQ(23, id);
  EXPECT_EQ(13u, v0.size());
  EXPECT_EQ(24u, v0.totalSize());

  id = v0.insert(Dummy(3)); EXPECT_EQ( 21, id);
  id = v0.insert(Dummy(3)); EXPECT_EQ( 19, id);
  id = v0.insert(Dummy(3)); EXPECT_EQ( 17, id);
  id = v0.insert(Dummy(3)); EXPECT_EQ( 15, id);
  id = v0.insert(Dummy(3)); EXPECT_EQ( 13, id);
  id = v0.insert(Dummy(3)); EXPECT_EQ( 11, id);
  id = v0.insert(Dummy(3)); EXPECT_EQ(  9, id);
  id = v0.insert(Dummy(3)); EXPECT_EQ(  7, id);
  id = v0.insert(Dummy(3)); EXPECT_EQ(  5, id);
  id = v0.insert(Dummy(3)); EXPECT_EQ(  3, id);
  id = v0.insert(Dummy(3)); EXPECT_EQ(  1, id);

  // v0 = {0,3,2,3,  0,3,2,3,  0,3,2,3,  0,3,2,3,  0,3,2,3,  0,3,2,3};

  EXPECT_EQ(24u, v0.size());
  EXPECT_EQ(24u, v0.totalSize());

  id = v0.insert(Dummy(5)); EXPECT_EQ( 24, id);
  id = v0.insert(Dummy(5)); EXPECT_EQ( 25, id);
  id = v0.insert(Dummy(5)); EXPECT_EQ( 26, id);

  // v0 = {0,3,2,3,  0,3,2,3,  0,3,2,3,  0,3,2,3,  0,3,2,3,  0,3,2,3, 5,5,5};
  EXPECT_EQ(27u, v0.size());
  EXPECT_EQ(27u, v0.totalSize());

};


TEST(SeqListTest, TestStep2)
{
  int a[] = {1,3,5,7,  1,3,5,7,  1,3,5,7,  1,3,5,7,  1,3,5,7,  1,3,5,7}; // 6 x 4 = 24
  //int a[] = {0,1,2,3,  0,1,2,3,  0,1,2,3,  0,1,2,3,  0,1,2,3,  0,1,2,3}; // 6 x 4 = 24
  int a_size = sizeof(a)/sizeof(int);
  SeqList<std::vector<Dummy> > v;
  SeqList<std::vector<Dummy> >::iterator it;
  SeqList<std::vector<Dummy> >::const_iterator cit;
  
  //

  for (int i=0; i<a_size; ++i)
    v.insert(Dummy(i)); // v = a

  EXPECT_EQ(index_t(a_size), (index_t)size(v));

  EXPECT_EQ(24u, v.size());
  EXPECT_EQ(24u, v.totalSize());


  v.disable(0);
  // {x,3,5,7,  1,3,5,7,  1,3,5,7,  1,3,5,7,  1,3,5,7,  1,3,5,7};
  EXPECT_EQ(1, v.begin()->getTag());

  v.disable(0);
  // {x,3,5,7,  1,3,5,7,  1,3,5,7,  1,3,5,7,  1,3,5,7,  1,3,5,7};
  EXPECT_EQ(1, v.begin()->getTag());

  v.disable(1);
  // {x,x,5,7,  1,3,5,7,  1,3,5,7,  1,3,5,7,  1,3,5,7,  1,3,5,7};
  EXPECT_EQ(2, v.begin()->getTag());

  v.disable(2);
  // {x,x,x,7,  1,3,5,7,  1,3,5,7,  1,3,5,7,  1,3,5,7,  1,3,5,7};
  EXPECT_EQ(3, v.begin()->getTag());

  v.disable(3);
  // {x,x,x,x,  1,3,5,7,  1,3,5,7,  1,3,5,7,  1,3,5,7,  1,3,5,7};
  EXPECT_EQ(4, v.begin()->getTag());

  EXPECT_EQ(20u, v.size());
  EXPECT_EQ(24u, v.totalSize());

  int id;
  id = v.insert(Dummy(3)); EXPECT_EQ(3, id);
  id = v.insert(Dummy(2)); EXPECT_EQ(2, id);
  id = v.insert(Dummy(1)); EXPECT_EQ(1, id);
  id = v.insert(Dummy(0)); EXPECT_EQ(0, id);
  // {1,3,5,7,  1,3,5,7,  1,3,5,7,  1,3,5,7,  1,3,5,7,  1,3,5,7};
  
  // ================ iterators ===================

  v.disable(0); v.disable(20);
  v.disable(1); v.disable(21);
  v.disable(2); v.disable(22);
  v.disable(3); v.disable(23);
  // {x,x,x,x,  1,3,5,7,  1,3,5,7,  1,3,5,7,  1,3,5,7,  x,x,x,x};
  
  
  EXPECT_EQ(index_t(0), v.contiguousId(4));

  v.disable(5);
  v.disable(11);
  v.disable(18);
  v.disable(15);
  // {x,x,x,x,  1,x,5,7,  1,3,5,x,  1,3,5,x,  1,3,x,7,  x,x,x,x};
  
  EXPECT_EQ(index_t(12), (index_t)v.size());
  EXPECT_EQ(index_t(12), (index_t)size(v));
  EXPECT_EQ(index_t(24), (index_t)v.totalSize());  
  EXPECT_EQ(index_t(0 ), v.contiguousId(4));  EXPECT_EQ(index_t(6 ), v.contiguousId(12));
  EXPECT_EQ(index_t(1 ), v.contiguousId(6));  EXPECT_EQ(index_t(7 ), v.contiguousId(13));
  EXPECT_EQ(index_t(2 ), v.contiguousId(7));  EXPECT_EQ(index_t(8 ), v.contiguousId(14));
  EXPECT_EQ(index_t(3 ), v.contiguousId(8));  EXPECT_EQ(index_t(9 ), v.contiguousId(16));
  EXPECT_EQ(index_t(4 ), v.contiguousId(9));  EXPECT_EQ(index_t(10), v.contiguousId(17));
  EXPECT_EQ(index_t(5 ), v.contiguousId(10)); EXPECT_EQ(index_t(11), v.contiguousId(19));

  index_t u[] = {4,6,7,8,9,10,12,13,14,16,17,19};
  index_t w[] = {0,1,2,3,4,5,6,7,8,9,10,11};

  v.contiguousIds(u, u+12, u);
  
  EXPECT_TRUE(sameElements(u, u+12, w, w+12));
  
  // test iterators (valgrind test)
  {
    SeqList<std::vector<Dummy> >::iterator i = v.begin();
    int k=0;
    for (; i!= v.end();   ++i) ++k;
    for (; i!= v.begin(); --i) --k;
    EXPECT_EQ(0,k);
    for (; i!= v.end();   i++) k++;
    for (; i!= v.begin(); i--) k--;
    EXPECT_EQ(0,k);
  }  
  
  
};


TEST(SeqListTest, ParallelItersTest0)
{

  int a[] = {0,1,2,3,  0,1,2,3,  0,1,2,3,  0,1,2,3,  0,1,2,3,  0,1,2,3,
             0,1,2,3,  0,1,2,3,  0,1,2,3,  0,1,2,3,  0,1,2,3,  0,1,2,3,
             0,1,2,3,  0,1,2,3,  0,1,2,3,  0,1,2,3,  0,1,2,3,  0,1,2,3}; // 18 x 4 = 72
  int a_size = sizeof(a)/sizeof(int);
  alelib::SeqList<std::vector<Dummy> > v;
  alelib::SeqList<std::vector<Dummy> >::iterator it;
  alelib::SeqList<std::vector<Dummy> >::const_iterator cit;
  
  

  for (int i=0; i<a_size; ++i)
    v.insert(Dummy(i)); // v = a

  // ============================ iterators ==============================
  
  int nthreads = 3;
  int tid;
  //    simulating 3 threads
  for (tid = 0; tid<nthreads; ++tid)
    for (it = v.begin(tid, nthreads); it != v.end(tid, nthreads); ++it)
      (*it).hist += 1;
      
  // checking
  for (it = v.begin(); it != v.end(); ++it)
  {
    EXPECT_EQ(1, (*it).hist);
    (*it).hist = 0;
  }
  
    
  // =======================================================================
  
  v.disable(0); v.disable(68);
  v.disable(1); v.disable(69);
  v.disable(2); v.disable(70);
  v.disable(3); v.disable(71);
  
  EXPECT_EQ(64u, size(v));
  EXPECT_EQ(64u, v.size());
  
  //    simulating 3 threads
  for (tid = 0; tid<nthreads; ++tid)
    for (it = v.begin(tid, nthreads); it != v.end(tid, nthreads); ++it)
      (*it).hist += 1;
      
  // checking
  for (it = v.begin(); it != v.end(); ++it)
  {
    EXPECT_EQ(1, (*it).hist);
    (*it).hist = 0; // reseting
  }
  
 
  
};


TEST(SeqListTest, ParallelItersTest1)
{

  int a[] = {0,1,2,3,  0,1,2,3}; // 2 x 4 = 8
  int a_size = sizeof(a)/sizeof(int);
  alelib::SeqList<std::vector<Dummy> > v;
  alelib::SeqList<std::vector<Dummy> >::iterator it;
  alelib::SeqList<std::vector<Dummy> >::const_iterator cit;
  
  

  for (int i=0; i<a_size; ++i)
    v.insert(Dummy(i)); // v = a

  
  // ============================ iterators ==============================
  
  int nthreads = 10;
  int tid;
  //    simulating 10 threads
  for (tid = 0; tid<nthreads; ++tid)
    for (it = v.begin(tid, nthreads); it != v.end(tid, nthreads); ++it)
      (*it).hist += 1;
      
  // checking
  for (it = v.begin(); it != v.end(); ++it)
  {
    EXPECT_EQ(1, (*it).hist);
    (*it).hist = 0;
  }

  
  // =======================================================================
  

  
};
  


TEST(SeqListTest, TestStepWithDeque0)
{
  int a[] = {0,1,2,3,  0,1,2,3,  0,1,2,3,  0,1,2,3,  0,1,2,3,  0,1,2,3}; // 6 x 4 = 24
  int a_size = sizeof(a)/sizeof(int);
  
  typedef alelib::SeqList<std::deque<Dummy> > container_type;
  
  container_type v0;
  container_type::iterator it;
  container_type::const_iterator cit;


  for (int i=0; i<a_size; ++i)
    v0.insert(Dummy(i)); // v0 = a


  // ================== disable ===================

  // disabling ? 3, except the last one
  for (int i = 3; i <a_size-1; i+=4)
    v0.disable(i);

  // v0 = {0,1,2,x,  0,1,2,x,  0,1,2,x,  0,1,2,x,  0,1,2,x,  0,1,2,3};
  
  EXPECT_EQ(19u, v0.size());

  v0.disable(a_size-1); // v0 = {0,1,2,x,  0,1,2,x,  0,1,2,x,  0,1,2,x,  0,1,2,x,  0,1,2,x};

  EXPECT_EQ(18u, v0.size());

  
 

  // ================== disable ===================


  // nothing
  v0.disable(a_size-1); // v0 = {0,1,2,x,  0,1,2,x,  0,1,2,x,  0,1,2,x,  0,1,2,x,  0,1,2,x};

  v0.disable(1); // v0 = {0,x,2,x,  0,1,2,x,  0,1,2,x,  0,1,2,x,  0,1,2,x,  0,1,2,x};

  EXPECT_EQ(17u, v0.size());

  v0.disable(21); // v0 = {0,x,2,x,  0,1,2,x,  0,1,2,x,  0,1,2,x,  0,1,2,x,  0,x,2,x};

  v0.disable(9); // v0 = {0,x,2,x,  0,1,2,x,  0,x,2,x,  0,1,2,x,  0,1,2,x,  0,x,2,x};

  // =====================================================

  // v0 = {0,x,2,x,  0,1,2,x,  0,x,2,x,  0,1,2,x,  0,1,2,x,  0,x,2,x};
  v0.disable(5);
  v0.disable(13);
  v0.disable(17);

  // v0 = {0,x,2,x,  0,x,2,x,  0,x,2,x,  0,x,2,x,  0,x,2,x,  0,x,2,x};

  // ================== insert =======================

  // v0 = {0,x,2,x,  0,x,2,x,  0,x,2,x,  0,x,2,x,  0,x,2,x,  0,x,2,x};
  int id;
  id = v0.insert(Dummy(3)); // v0 = {0,x,2,x,  0,x,2,x,  0,x,2,x,  0,x,2,x,  0,x,2,x,  0,x,2,3};

  EXPECT_EQ(23, id);
  EXPECT_EQ(13u, v0.size());
  EXPECT_EQ(24u, v0.totalSize());

  id = v0.insert(Dummy(3)); EXPECT_EQ( 21, id);
  id = v0.insert(Dummy(3)); EXPECT_EQ( 19, id);
  id = v0.insert(Dummy(3)); EXPECT_EQ( 17, id);
  id = v0.insert(Dummy(3)); EXPECT_EQ( 15, id);
  id = v0.insert(Dummy(3)); EXPECT_EQ( 13, id);
  id = v0.insert(Dummy(3)); EXPECT_EQ( 11, id);
  id = v0.insert(Dummy(3)); EXPECT_EQ(  9, id);
  id = v0.insert(Dummy(3)); EXPECT_EQ(  7, id);
  id = v0.insert(Dummy(3)); EXPECT_EQ(  5, id);
  id = v0.insert(Dummy(3)); EXPECT_EQ(  3, id);
  id = v0.insert(Dummy(3)); EXPECT_EQ(  1, id);

  // v0 = {0,3,2,3,  0,3,2,3,  0,3,2,3,  0,3,2,3,  0,3,2,3,  0,3,2,3};

  EXPECT_EQ(24u, v0.size());
  EXPECT_EQ(24u, v0.totalSize());

  id = v0.insert(Dummy(5)); EXPECT_EQ( 24, id);
  id = v0.insert(Dummy(5)); EXPECT_EQ( 25, id);
  id = v0.insert(Dummy(5)); EXPECT_EQ( 26, id);

  // v0 = {0,3,2,3,  0,3,2,3,  0,3,2,3,  0,3,2,3,  0,3,2,3,  0,3,2,3, 5,5,5};
  EXPECT_EQ(27u, v0.size());
  EXPECT_EQ(27u, v0.totalSize());

};


