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

// --gtest_catch_exceptions=0


#include "common.hpp"
#include <gtest/gtest.h>
#include <Alelib/DofMapper>
#include <Alelib/Mesh>
#include <Alelib/IO>
#include <tr1/array>
#include <tr1/tuple>
#include <algorithm>
#include <tr1/memory>
#include <cmath>
#include <typeinfo>

namespace MESH_TEST_CPP {




class TriMesh1Tests : public testing::Test
{
 protected:  // You should make the members protected s.t. they can be
             // accessed from sub-classes.

  typedef MeshTri MeshT;
  typedef MeshT::VertexH VertexH;
  typedef MeshT::CellH CellH;
  typedef MeshT::FacetH FacetH;
  typedef MeshT::RidgeH RidgeH;
  typedef MeshIoVtk<MeshT> MeshWriter;

  TriMesh1Tests() : m() {}

  MeshT m;

  static void addTriMesh1(MeshTri &m, bool check = true)
  {
    typedef MeshTri MeshT;
    typedef MeshT::VertexH VertexH;
    typedef MeshT::CellH   CellH;

    int const N = 14;

    VertexH vts[N];
                                    // tag
    vts[ 0] = m.addVertex(listOf<Real>(0,0,0), 0);
    vts[ 1] = m.addVertex(listOf<Real>(1,0,0), 1);
    vts[ 2] = m.addVertex(listOf<Real>(0,1,0), 2);
    vts[ 3] = m.addVertex(listOf<Real>(1,1,0), 3);
    vts[ 4] = m.addVertex(listOf<Real>(2,1,0), 4);
    vts[ 5] = m.addVertex(listOf<Real>(0,2,0), 5);
    vts[ 6] = m.addVertex(listOf<Real>(1,2,0), 6);
    vts[ 7] = m.addVertex(listOf<Real>(2,2,0), 7);
    vts[ 8] = m.addVertex(listOf<Real>(3,2,0), 8);
    vts[ 9] = m.addVertex(listOf<Real>(0,3,0), 9);
    vts[10] = m.addVertex(listOf<Real>(1,3,0),10);
    vts[11] = m.addVertex(listOf<Real>(2,3,0),11);
    vts[12] = m.addVertex(listOf<Real>(3,3,0),12);
    vts[13] = m.addVertex(listOf<Real>(1,2,1),13);

    int const E = 11;

    CellH cells[E];

    cells[ 0] = m.addCell(listOf(vts[ 0], vts[ 1], vts[ 2])); //  0
    cells[ 1] = m.addCell(listOf(vts[ 1], vts[ 3], vts[ 4])); //  1
    cells[ 2] = m.addCell(listOf(vts[ 2], vts[ 6], vts[ 5])); //  2
    cells[ 3] = m.addCell(listOf(vts[ 3], vts[ 4], vts[ 7])); //  3
    cells[ 4] = m.addCell(listOf(vts[ 5], vts[10], vts[ 9])); //  4
    cells[ 5] = m.addCell(listOf(vts[ 5], vts[ 6], vts[10])); //  5
    cells[ 6] = m.addCell(listOf(vts[ 6], vts[11], vts[10])); //  6
    cells[ 7] = m.addCell(listOf(vts[ 6], vts[ 7], vts[11])); //  7
    cells[ 8] = m.addCell(listOf(vts[ 7], vts[ 8], vts[12])); //  8
    cells[ 9] = m.addCell(listOf(vts[13], vts[ 6], vts[10])); //  9
    cells[10] = m.addCell(listOf(vts[ 2], vts[ 7], vts[ 6])); // 10

    // attention: this vertex is boundary because of the third triangle added to its edge
    EXPECT_TRUE(vts[ 6].isBoundary(&m));

    // if we add the triangle:
    m.addCell(listOf(vts[ 6], vts[ 7], vts[13]));

    // now the vertex should not be boundary:
    EXPECT_FALSE(vts[ 6].isBoundary(&m));

    EXPECT_TRUE(cells[ 3].adjCell(&m, 0) == cells[ 1]);
    EXPECT_TRUE(cells[ 1].adjCell(&m, 1) == cells[ 3]);


    // specific checking
    std::vector<VertexH> cverts(3);


    cverts[0] = vts[ 0]; cverts[1] = vts[ 1]; cverts[2] = vts[ 2]; EXPECT_TRUE(cverts == cells[0].vertices(&m));
    cverts[0] = vts[ 1]; cverts[1] = vts[ 3]; cverts[2] = vts[ 4]; EXPECT_TRUE(cverts == cells[1].vertices(&m));
    cverts[0] = vts[ 2]; cverts[1] = vts[ 6]; cverts[2] = vts[ 5]; EXPECT_TRUE(cverts == cells[2].vertices(&m));
    cverts[0] = vts[ 3]; cverts[1] = vts[ 4]; cverts[2] = vts[ 7]; EXPECT_TRUE(cverts == cells[3].vertices(&m));
    cverts[0] = vts[ 5]; cverts[1] = vts[10]; cverts[2] = vts[ 9]; EXPECT_TRUE(cverts == cells[4].vertices(&m));
    cverts[0] = vts[ 5]; cverts[1] = vts[ 6]; cverts[2] = vts[10]; EXPECT_TRUE(cverts == cells[5].vertices(&m));
    cverts[0] = vts[ 6]; cverts[1] = vts[11]; cverts[2] = vts[10]; EXPECT_TRUE(cverts == cells[6].vertices(&m));
    cverts[0] = vts[ 6]; cverts[1] = vts[ 7]; cverts[2] = vts[11]; EXPECT_TRUE(cverts == cells[7].vertices(&m));
    cverts[0] = vts[ 7]; cverts[1] = vts[ 8]; cverts[2] = vts[12]; EXPECT_TRUE(cverts == cells[8].vertices(&m));
    cverts[0] = vts[13]; cverts[1] = vts[ 6]; cverts[2] = vts[10]; EXPECT_TRUE(cverts == cells[9].vertices(&m));

    EXPECT_EQ(14u, m.numVertices());
    EXPECT_EQ(12u, m.numCells());
    EXPECT_EQ(26u, m.numFacets());

    if (!check)
      return;

    checkMesh(m);


  }

  // virtual void SetUp() will be called before each test is run.  You
  // should define it if you need to initialize the varaibles.
  // Otherwise, this can be skipped.
  virtual void SetUp() {
    //std::cout << "HHHHHHHHHHEEEEEEEEEEEERRRRRRRRRRRRREEEEEEEEEEEEEEEEEEEE: " << typeid(MeshT::CellT).name() << std::endl;
    //std::cout << "HHHHHHHHHHEEEEEEEEEEEERRRRRRRRRRRRREEEEEEEEEEEEEEEEEEEE: " << typeid(Cell<TRIANGLE>).name() << std::endl;
    addTriMesh1(m, true);

  }

  // virtual void TearDown() will be called after each test is run.
  // You should define it if there is cleanup work to do.  Otherwise,
  // you don't have to provide it.
  //
  virtual void TearDown() {
  }




};

class TriMesh2Tests : public testing::Test
{
 protected:  // You should make the members protected s.t. they can be
             // accessed from sub-classes.

  typedef MeshTri MeshT;
  typedef MeshT::VertexH VertexH;
  typedef MeshT::CellH CellH;
  typedef MeshT::FacetH FacetH;
  typedef MeshT::RidgeH RidgeH;
  typedef MeshIoVtk<MeshT> MeshWriter;

  TriMesh2Tests() : m() {}

  MeshT m;

  static void addTriMesh(MeshTri &m)
  {
    typedef MeshTri MeshT;
    typedef MeshT::VertexH VertexH;
    typedef MeshT::CellH   CellH;

    int const N = 4;

    VertexH vts[N];
                                               // tag
    vts[ 0] = m.addVertex(listOf<Real>(0,0,0), 0);
    vts[ 1] = m.addVertex(listOf<Real>(1,0,0), 1);
    vts[ 2] = m.addVertex(listOf<Real>(0,1,0), 2);
    vts[ 3] = m.addVertex(listOf<Real>(1,1,0), 3);

    int const E = 2;

    CellH cells[E];

    cells[ 0] = m.addCell(listOf(vts[ 0], vts[ 1], vts[ 2])); //  0
    cells[ 1] = m.addCell(listOf(vts[ 2], vts[ 1], vts[ 3])); //  1

    checkMesh(m);
  }

  // virtual void SetUp() will be called before each test is run.  You
  // should define it if you need to initialize the varaibles.
  // Otherwise, this can be skipped.
  virtual void SetUp() {
    addTriMesh(m);
  }

  // virtual void TearDown() will be called after each test is run.
  // You should define it if there is cleanup work to do.  Otherwise,
  // you don't have to provide it.
  //
  virtual void TearDown() {
  }


};


class TetMesh1Tests : public testing::Test
{
 protected:  // You should make the members protected s.t. they can be
             // accessed from sub-classes.

  typedef MeshTet MeshT;
  typedef MeshT::VertexH VertexH;
  typedef MeshT::CellH CellH;
  typedef MeshT::FacetH FacetH;
  typedef MeshT::RidgeH RidgeH;
  typedef MeshIoVtk<MeshT> MeshWriter;

  TetMesh1Tests() : m() {}

  MeshT m;

  static void addTetMesh1(MeshTet &m, bool check = true)
  {
    typedef MeshTet MeshT;
    typedef MeshT::VertexH VertexH;
    typedef MeshT::CellH   CellH;

    int const N = 35;

    VertexH vts[N];

    vts[ 0] = m.addVertex(listOf<Real>(0.  ,  0   ,  0   ));
    vts[ 1] = m.addVertex(listOf<Real>(1.  ,  0   ,  0   ));
    vts[ 2] = m.addVertex(listOf<Real>(0.  ,  1   ,  0   ));
    vts[ 3] = m.addVertex(listOf<Real>(0.  ,  0   ,  1   ));
    vts[ 4] = m.addVertex(listOf<Real>(0.25,  0   ,  0   ));
    vts[ 5] = m.addVertex(listOf<Real>(0.5 ,  0   ,  0   ));
    vts[ 6] = m.addVertex(listOf<Real>(0.75,  0   ,  0   ));
    vts[ 7] = m.addVertex(listOf<Real>(0.75,  0.25,  0   ));
    vts[ 8] = m.addVertex(listOf<Real>(0.5 ,  0.5 ,  0   ));
    vts[ 9] = m.addVertex(listOf<Real>(0.25,  0.75,  0   ));
    vts[10] = m.addVertex(listOf<Real>(0   ,  0.75,  0   ));
    vts[11] = m.addVertex(listOf<Real>(0   ,  0.5 ,  0   ));
    vts[12] = m.addVertex(listOf<Real>(0   ,  0.25,  0   ));
    vts[13] = m.addVertex(listOf<Real>(0   ,  0   ,  0.75));
    vts[14] = m.addVertex(listOf<Real>(0   ,  0   ,  0.5 ));
    vts[15] = m.addVertex(listOf<Real>(0   ,  0   ,  0.25));
    vts[16] = m.addVertex(listOf<Real>(0   ,  0.25,  0.75));
    vts[17] = m.addVertex(listOf<Real>(0   ,  0.5 ,  0.5 ));
    vts[18] = m.addVertex(listOf<Real>(0   ,  0.75,  0.25));
    vts[19] = m.addVertex(listOf<Real>(0.25,  0   ,  0.75));
    vts[20] = m.addVertex(listOf<Real>(0.5 ,  0   ,  0.5 ));
    vts[21] = m.addVertex(listOf<Real>(0.75,  0   ,  0.25));
    vts[22] = m.addVertex(listOf<Real>(0.25,  0.5 ,  0   ));
    vts[23] = m.addVertex(listOf<Real>(0.5 ,  0.25,  0   ));
    vts[24] = m.addVertex(listOf<Real>(0.25,  0.25,  0   ));
    vts[25] = m.addVertex(listOf<Real>(0.25,  0   ,  0.5 ));
    vts[26] = m.addVertex(listOf<Real>(0.25,  0   ,  0.25));
    vts[27] = m.addVertex(listOf<Real>(0.5 ,  0   ,  0.25));
    vts[28] = m.addVertex(listOf<Real>(0   ,  0.5 ,  0.25));
    vts[29] = m.addVertex(listOf<Real>(0   ,  0.25,  0.5 ));
    vts[30] = m.addVertex(listOf<Real>(0   ,  0.25,  0.25));
    vts[31] = m.addVertex(listOf<Real>(0.5 ,  0.25,  0.25));
    vts[32] = m.addVertex(listOf<Real>(0.25,  0.5 ,  0.25));
    vts[33] = m.addVertex(listOf<Real>(0.25,  0.25,  0.5 ));
    vts[34] = m.addVertex(listOf<Real>(0.25,  0.25,  0.25));

    int const E = 64;

    CellH cells[E];

    cells[ 0] = m.addCell(listOf(vts[0 ], vts[4 ], vts[12], vts[15]));
    cells[ 1] = m.addCell(listOf(vts[24], vts[30], vts[15], vts[26]));
    cells[ 2] = m.addCell(listOf(vts[24], vts[15], vts[4 ], vts[26]));
    cells[ 3] = m.addCell(listOf(vts[15], vts[30], vts[24], vts[12]));
    cells[ 4] = m.addCell(listOf(vts[4 ], vts[15], vts[24], vts[12]));
    cells[ 5] = m.addCell(listOf(vts[26], vts[24], vts[30], vts[34]));
    cells[ 6] = m.addCell(listOf(vts[15], vts[26], vts[30], vts[14]));
    cells[ 7] = m.addCell(listOf(vts[34], vts[29], vts[14], vts[25]));
    cells[ 8] = m.addCell(listOf(vts[34], vts[14], vts[26], vts[25]));
    cells[ 9] = m.addCell(listOf(vts[14], vts[29], vts[34], vts[30]));
    cells[10] = m.addCell(listOf(vts[26], vts[14], vts[34], vts[30]));
    cells[11] = m.addCell(listOf(vts[25], vts[34], vts[29], vts[33]));
    cells[12] = m.addCell(listOf(vts[14], vts[25], vts[29], vts[13]));
    cells[13] = m.addCell(listOf(vts[33], vts[16], vts[13], vts[19]));
    cells[14] = m.addCell(listOf(vts[33], vts[13], vts[25], vts[19]));
    cells[15] = m.addCell(listOf(vts[13], vts[16], vts[33], vts[29]));
    cells[16] = m.addCell(listOf(vts[25], vts[13], vts[33], vts[29]));
    cells[17] = m.addCell(listOf(vts[13], vts[19], vts[16], vts[3 ]));
    cells[18] = m.addCell(listOf(vts[12], vts[24], vts[11], vts[30]));
    cells[19] = m.addCell(listOf(vts[22], vts[28], vts[30], vts[34]));
    cells[20] = m.addCell(listOf(vts[22], vts[30], vts[24], vts[34]));
    cells[21] = m.addCell(listOf(vts[30], vts[28], vts[22], vts[11]));
    cells[22] = m.addCell(listOf(vts[24], vts[30], vts[22], vts[11]));
    cells[23] = m.addCell(listOf(vts[34], vts[22], vts[28], vts[32]));
    cells[24] = m.addCell(listOf(vts[30], vts[34], vts[28], vts[29]));
    cells[25] = m.addCell(listOf(vts[32], vts[17], vts[29], vts[33]));
    cells[26] = m.addCell(listOf(vts[32], vts[29], vts[34], vts[33]));
    cells[27] = m.addCell(listOf(vts[29], vts[17], vts[32], vts[28]));
    cells[28] = m.addCell(listOf(vts[34], vts[29], vts[32], vts[28]));
    cells[29] = m.addCell(listOf(vts[29], vts[33], vts[17], vts[16]));
    cells[30] = m.addCell(listOf(vts[11], vts[22], vts[10], vts[28]));
    cells[31] = m.addCell(listOf(vts[9 ], vts[18], vts[28], vts[32]));
    cells[32] = m.addCell(listOf(vts[9 ], vts[28], vts[22], vts[32]));
    cells[33] = m.addCell(listOf(vts[28], vts[18], vts[9 ], vts[10]));
    cells[34] = m.addCell(listOf(vts[22], vts[28], vts[9 ], vts[10]));
    cells[35] = m.addCell(listOf(vts[28], vts[32], vts[18], vts[17]));
    cells[36] = m.addCell(listOf(vts[10], vts[9 ], vts[2 ], vts[18]));
    cells[37] = m.addCell(listOf(vts[4 ], vts[5 ], vts[24], vts[26]));
    cells[38] = m.addCell(listOf(vts[23], vts[34], vts[26], vts[27]));
    cells[39] = m.addCell(listOf(vts[23], vts[26], vts[5 ], vts[27]));
    cells[40] = m.addCell(listOf(vts[26], vts[34], vts[23], vts[24]));
    cells[41] = m.addCell(listOf(vts[5 ], vts[26], vts[23], vts[24]));
    cells[42] = m.addCell(listOf(vts[27], vts[23], vts[34], vts[31]));
    cells[43] = m.addCell(listOf(vts[26], vts[27], vts[34], vts[25]));
    cells[44] = m.addCell(listOf(vts[31], vts[33], vts[25], vts[20]));
    cells[45] = m.addCell(listOf(vts[31], vts[25], vts[27], vts[20]));
    cells[46] = m.addCell(listOf(vts[25], vts[33], vts[31], vts[34]));
    cells[47] = m.addCell(listOf(vts[27], vts[25], vts[31], vts[34]));
    cells[48] = m.addCell(listOf(vts[25], vts[20], vts[33], vts[19]));
    cells[49] = m.addCell(listOf(vts[24], vts[23], vts[22], vts[34]));
    cells[50] = m.addCell(listOf(vts[8 ], vts[32], vts[34], vts[31]));
    cells[51] = m.addCell(listOf(vts[8 ], vts[34], vts[23], vts[31]));
    cells[52] = m.addCell(listOf(vts[34], vts[32], vts[8 ], vts[22]));
    cells[53] = m.addCell(listOf(vts[23], vts[34], vts[8 ], vts[22]));
    cells[54] = m.addCell(listOf(vts[34], vts[31], vts[32], vts[33]));
    cells[55] = m.addCell(listOf(vts[22], vts[8 ], vts[9 ], vts[32]));
    cells[56] = m.addCell(listOf(vts[5 ], vts[6 ], vts[23], vts[27]));
    cells[57] = m.addCell(listOf(vts[7 ], vts[31], vts[27], vts[21]));
    cells[58] = m.addCell(listOf(vts[7 ], vts[27], vts[6 ], vts[21]));
    cells[59] = m.addCell(listOf(vts[27], vts[31], vts[7 ], vts[23]));
    cells[60] = m.addCell(listOf(vts[6 ], vts[27], vts[7 ], vts[23]));
    cells[61] = m.addCell(listOf(vts[27], vts[21], vts[31], vts[20]));
    cells[62] = m.addCell(listOf(vts[23], vts[7 ], vts[8 ], vts[31]));
    cells[63] = m.addCell(listOf(vts[6 ], vts[1 ], vts[7 ], vts[21]));



    //// attention: this vertex is boundary because of the third triangle added to its edge
    //EXPECT_TRUE(vts[ 6].isBoundary(&m));
    //
    //// if we add the triangle:
    //m.addCell(listOf(vts[ 6], vts[ 7], vts[13]));
    //
    //// now the vertex should not be boundary:
    //EXPECT_FALSE(vts[ 6].isBoundary(&m));
    //
    //EXPECT_TRUE(cells[ 3].adjCell(&m, 0) == cells[ 1]);
    //EXPECT_TRUE(cells[ 1].adjCell(&m, 1) == cells[ 3]);
    //
    //
    //// specific checking
    //std::vector<VertexH> cverts(3);
    //
    //
    //cverts[0] = vts[ 0]; cverts[1] = vts[ 1]; cverts[2] = vts[ 2]; EXPECT_TRUE(cverts == cells[0].vertices(&m));
    //cverts[0] = vts[ 1]; cverts[1] = vts[ 3]; cverts[2] = vts[ 4]; EXPECT_TRUE(cverts == cells[1].vertices(&m));
    //cverts[0] = vts[ 2]; cverts[1] = vts[ 6]; cverts[2] = vts[ 5]; EXPECT_TRUE(cverts == cells[2].vertices(&m));
    //cverts[0] = vts[ 3]; cverts[1] = vts[ 4]; cverts[2] = vts[ 7]; EXPECT_TRUE(cverts == cells[3].vertices(&m));
    //cverts[0] = vts[ 5]; cverts[1] = vts[10]; cverts[2] = vts[ 9]; EXPECT_TRUE(cverts == cells[4].vertices(&m));
    //cverts[0] = vts[ 5]; cverts[1] = vts[ 6]; cverts[2] = vts[10]; EXPECT_TRUE(cverts == cells[5].vertices(&m));
    //cverts[0] = vts[ 6]; cverts[1] = vts[11]; cverts[2] = vts[10]; EXPECT_TRUE(cverts == cells[6].vertices(&m));
    //cverts[0] = vts[ 6]; cverts[1] = vts[ 7]; cverts[2] = vts[11]; EXPECT_TRUE(cverts == cells[7].vertices(&m));
    //cverts[0] = vts[ 7]; cverts[1] = vts[ 8]; cverts[2] = vts[12]; EXPECT_TRUE(cverts == cells[8].vertices(&m));
    //cverts[0] = vts[13]; cverts[1] = vts[ 6]; cverts[2] = vts[10]; EXPECT_TRUE(cverts == cells[9].vertices(&m));
    //
    //EXPECT_EQ(14u, m.numVertices());
    //EXPECT_EQ(12u, m.numCells());
    //EXPECT_EQ(26u, m.numFacets());

    if (!check)
      return;

    checkMesh(m);


  }

  // virtual void SetUp() will be called before each test is run.  You
  // should define it if you need to initialize the varaibles.
  // Otherwise, this can be skipped.
  virtual void SetUp() {

    addTetMesh1(m, true);

  }

  // virtual void TearDown() will be called after each test is run.
  // You should define it if there is cleanup work to do.  Otherwise,
  // you don't have to provide it.
  //
  virtual void TearDown() {
  }




};

class TetMesh2Tests : public testing::Test
{
 protected:  // You should make the members protected s.t. they can be
             // accessed from sub-classes.

  typedef MeshTet MeshT;
  typedef MeshT::VertexH VertexH;
  typedef MeshT::CellH CellH;
  typedef MeshT::FacetH FacetH;
  typedef MeshT::RidgeH RidgeH;
  typedef MeshIoVtk<MeshT> MeshWriter;

  TetMesh2Tests() : m() {}

  MeshT m;

  static void addTetMesh(MeshTet &m, bool check = true)
  {
    typedef MeshTet MeshT;
    typedef MeshT::VertexH VertexH;
    typedef MeshT::CellH   CellH;

    int const N = 5;

    VertexH vts[N];
                                               // tag
    vts[ 0] = m.addVertex(listOf<Real>(0,0,0), 0);
    vts[ 1] = m.addVertex(listOf<Real>(1,0,0), 1);
    vts[ 2] = m.addVertex(listOf<Real>(0,1,0), 2);
    vts[ 3] = m.addVertex(listOf<Real>(0,0,1), 3);
    vts[ 4] = m.addVertex(listOf<Real>(0,-1,0), 4);

    int const E = 2;

    CellH cells[E];

    //cells[ 0] = m.addCell(listOf(vts[ 0], vts[ 1], vts[ 2], vts[ 3])); //  0
    //cells[ 0] = m.addCell(listOf(vts[ 1], vts[ 2], vts[ 0], vts[ 3])); //  0
    cells[ 0] = m.addCell(listOf(vts[ 2], vts[ 3], vts[ 0], vts[ 1])); //  0
    //cells[ 0] = m.addCell(listOf(vts[ 1], vts[ 3], vts[ 2], vts[ 0])); //  0
    //cells[ 0] = m.addCell(listOf(vts[ 1], vts[ 3], vts[ 2], vts[ 0])); //  0
    //cells[ 0] = m.addCell(listOf(vts[ 1], vts[ 3], vts[ 2], vts[ 0])); //  0

    cells[ 1] = m.addCell(listOf(vts[ 0], vts[ 4], vts[ 1], vts[ 3])); //  1

    if (!check)
      return;

    checkMesh(m);
  }

  // virtual void SetUp() will be called before each test is run.  You
  // should define it if you need to initialize the varaibles.
  // Otherwise, this can be skipped.
  virtual void SetUp() {

    addTetMesh(m, true);

  }

  // virtual void TearDown() will be called after each test is run.
  // You should define it if there is cleanup work to do.  Otherwise,
  // you don't have to provide it.
  //
  virtual void TearDown() {
  }




};



TEST(MeshTest, Initialize2d)
{
  MeshEdg m0;
  MeshTri m1;
  MeshQua m2;
  MeshTet m3;
  MeshHex m4;

  //MeshEdg::printElementsSize();
  //MeshTri::printElementsSize();
  //MeshQua::printElementsSize();
  MeshTet::printElementsSize();
  //MeshHex::printElementsSize();
}

TEST(MeshTest, AddVertex)
{
  typedef MeshTri MeshT;
  typedef MeshT::VertexH VertexH;
  MeshT m;

  EXPECT_EQ(0u, m.numVertices());

  VertexH vts[5];

  for (unsigned i = 0; i < 5; ++i)
  {
    vts[i] = m.addVertex(listOf<Real>(i,i,0));
    EXPECT_EQ(i+1, m.numVertices());
    EXPECT_EQ(0u, vts[i].valency(&m));
  }

  for (int j = 0; j < 2; ++j)
    for (unsigned i = 0; i < 5; ++i)
      EXPECT_NEAR(Real(i), vts[i].coord(&m, j), 1e-15);

}

TEST(MeshTest, RemoveUnrefVertex)
{
  typedef MeshTri MeshT;
  typedef MeshT::VertexH VertexH;
  MeshT m;

  VertexH vts[5];

  for (unsigned i = 0; i < 5; ++i)
  { vts[i] = m.addVertex(listOf<Real>(i,i,0)); }

  m.removeUnrefVertex(vts[2]);
  EXPECT_EQ(index_t(4), (index_t)m.numVertices());
  m.removeUnrefVertex(vts[0]);
  EXPECT_EQ(index_t(3), (index_t)m.numVertices());
  m.removeUnrefVertex(vts[4]);
  EXPECT_EQ(index_t(2), (index_t)m.numVertices());

  EXPECT_EQ(index_t(5), (index_t)m.numVerticesTotal());

  EXPECT_TRUE( vts[2].isDisabled(&m));
  EXPECT_TRUE( vts[0].isDisabled(&m));
  EXPECT_TRUE( vts[4].isDisabled(&m));
  EXPECT_TRUE(!vts[1].isDisabled(&m));
  EXPECT_TRUE(!vts[3].isDisabled(&m));

  for (int c = 0; c < 2; ++c)
  {
    EXPECT_NEAR(Real(1), vts[1].coord(&m, c), 1e-15);
    EXPECT_NEAR(Real(3), vts[3].coord(&m, c), 1e-15);
  }

  m.removeUnrefVertices();

  for (unsigned i = 0; i < 5; ++i)
  { EXPECT_TRUE( vts[i].isDisabled(&m)); }

}

TEST_F(TriMesh1Tests, AddCell)
{
  checkMesh(m);
}

TEST_F(TriMesh1Tests, RemoveSingularCell)
{
  m.removeCell(CellH(9), false);
  // if we remove the cell 9, vtx 6 must be boundary
  EXPECT_TRUE(VertexH(6).isBoundary(&m));
  EXPECT_EQ(14u, m.numVertices());
  EXPECT_EQ(11u, m.numCells());
  EXPECT_EQ(25u, m.numFacets());
  checkMesh(m);

  // now, if we remove the cell 11, vtx 6 must be non-boundary
  m.removeCell(CellH(11), false);
  EXPECT_FALSE(VertexH(6).isBoundary(&m));
  EXPECT_EQ(14u, m.numVertices());
  EXPECT_EQ(10u, m.numCells());
  EXPECT_EQ(23u, m.numFacets());
  checkMesh(m);

  EXPECT_TRUE( m.removeUnrefVertex(13) );
  EXPECT_EQ(13u, m.numVertices());

  m.removeCell(CellH(5), true);
  EXPECT_TRUE(VertexH(6).isBoundary(&m));
  EXPECT_EQ(13u, m.numVertices());
  EXPECT_EQ(9u, m.numCells());
  EXPECT_EQ(23u, m.numFacets());
  checkMesh(m);
}

TEST_F(TriMesh1Tests, AddAndRemoveCell)
{
  unsigned k = 0;
  for (CellH ch = m.cellBegin(); ch != m.cellEnd(); ++ch)
  {
    if (ch.isDisabled(&m))
      continue;

    m.removeCell(ch, true);
    ++k;
    EXPECT_EQ(12u-k, m.numCells());
    checkMesh(m);
  }

  EXPECT_EQ(0u, m.numVertices());
  EXPECT_EQ(0u, m.numCells());
  EXPECT_EQ(0u, m.numFacets());


  // add again:
  addTriMesh1(m, false);
  checkMesh(m);
}

TEST_F(TriMesh1Tests, PrintVtkAscii)
{
  MeshWriter writer(&m);

  writer.setOutputFileName("output/trimesh1.vtk");

  double time = 1.2345;
  writer.writeMesh(&time);

}

TEST_F(TriMesh1Tests, PrintVtkBinSplitEdge)
{
  MeshWriter writer(&m);

  writer.setOutputFileName("output/trimesh1.vtk");
  writer.setBinaryOutput(true);
  writer.setFamily(true);

  writer.setNamePadding(2);

  double time = 1.1;
  writer.splitMeshEdges(2);
  writer.writeMesh(&time);

  m.removeCell(CellH(9), true);

  time = 1.2;
  writer.splitMeshEdges(3);
  writer.writeMesh(&time);

  m.removeCell(CellH(5), true);

  time = 1.3;
  writer.splitMeshEdges(4);
  writer.writeMesh(&time);

  m.removeCell(CellH(1), true);

  time = 1.4;
  writer.splitMeshEdges(5);
  writer.writeMesh(&time);

  checkMesh(m);
}

TEST_F(TetMesh1Tests, AddCell)
{
  // constructors already do that
}

TEST_F(TetMesh1Tests, RemoveCell)
{
  vector<CellH> star34 = VertexH(34).star(&m);

  for (int i = 0; i < (int)star34.size(); ++i)
    m.removeCell(star34[i], true);

  checkMesh(m);

  unsigned kk = 0;
  for(CellH c = m.cellBegin(); c != m.cellEnd(); ++c)
  {
    if (c.isDisabled(&m))
      continue;

    m.removeCell(c, true);
    if ((++kk)%2==0)
      checkMesh(m);
  }

  // add again to be sure
  addTetMesh1(m, true);

}

TEST_F(TetMesh1Tests, PrintVtkAscii)
{
  MeshWriter writer(&m);

  writer.setOutputFileName("output/TetMesh1.PrintVtkAscii.vtk");

  double time = 1.2345;
  writer.writeMesh(&time);
}

TEST_F(TetMesh1Tests, PrintVtkBinSplitEdge)
{
  MeshWriter writer(&m);

  writer.setOutputFileName("output/TetMesh1.PrintVtkBinSplitEdge.vtk");
  writer.setBinaryOutput(true);
  writer.setFamily(true);

  writer.setNamePadding(2);

  // remove some cells for better visibility
  for (int i = 0; i < 50; ++i)
    m.removeCell(CellH(i), true);


  double time = 1.1;
  writer.splitMeshEdges(2);
  writer.writeMesh(&time);

  //m.removeCell(CellH(9), true);
  //
  time = 1.2;
  writer.splitMeshEdges(3);
  writer.writeMesh(&time);
  //
  //m.removeCell(CellH(5), true);
  //
  time = 1.3;
  writer.splitMeshEdges(4);
  writer.writeMesh(&time);
  //
  //m.removeCell(CellH(1), true);
  //
  time = 1.4;
  writer.splitMeshEdges(5);
  writer.writeMesh(&time);
  //
  //checkMesh(m);
}

struct MyPrinterTet2 : public DefaultGetDataVtk
{
  typedef MeshTet::VertexH VertexH;
  typedef MeshTet::CellH CellH;
  MeshTet const& m;
  MyPrinterTet2(MeshTet const& m_) : DefaultGetDataVtk(), m(m_) {}

  virtual void getData(index_t id, Real *values) const
  {
    VertexH(id).coord(&m, values);
  }

  virtual void getData(Real const *x_local, index_t cell_id, int /*ith*/, Real *values) const
  {
    Real x[4][3];
    CellH c(cell_id);
    VertexH vts[4];
    c.vertices(&m, vts);
    for (int i = 0; i < 4; ++i)
      vts[i].coord(&m, x[i]);

    Real L[4];
    L[0] = 1;
    for (int i = 1; i <= 3; ++i) {
      L[i] = x_local[i-1];
      L[0] -= L[i];
    }

    Real x_real[3] = {0,0,0};
    for (int i = 0; i < 4; ++i)
      for (int j = 0; j < 3; ++j)
        x_real[j] += L[i]*x[i][j];

    values[0] = x_real[0];
    values[1] = x_real[1];
    values[2] = x_real[2];

  }

  virtual int numComps() const
  { return 3; }
};

TEST_F(TetMesh2Tests, PrintNodalFieldVtk)
{
  MeshWriter writer;
  writer.attachMesh(&m);
  writer.setBinaryOutput(false);
  writer.setFamily(true);

  writer.setNamePadding(2);
  writer.setOutputFileName("output/TetMesh2.PrintNodalFieldVtk.vtk");

  writer.splitMeshEdges(1);
  writer.writeMesh();
  writer.addNodalScalarField("radial", MyPrinterTet2(m));

  writer.splitMeshEdges(2);
  writer.writeMesh();
  writer.addNodalScalarField("radial", MyPrinterTet2(m));

  writer.splitMeshEdges(3);
  writer.writeMesh();
  writer.addNodalScalarField("radial", MyPrinterTet2(m));

  writer.splitMeshEdges(4);
  writer.writeMesh();
  writer.addNodalScalarField("radial", MyPrinterTet2(m));

  writer.splitMeshEdges(5);
  writer.writeMesh();
  writer.addNodalScalarField("radial", MyPrinterTet2(m));

}

TEST_F(TetMesh2Tests, PrintCustomCoordsFieldVtk)
{
  MeshWriter writer;
  writer.attachMesh(&m);
  writer.setBinaryOutput(false);
  writer.setFamily(true);

  writer.setNamePadding(2);
  writer.setOutputFileName("output/TetMesh2.PrintCustomCoordsFieldVtk.vtk");

  writer.splitMeshEdges(1);
  writer.writeMesh();
  writer.addNodalScalarField("radial", MyPrinterTet2(m));

  writer.splitMeshEdges(2);
  writer.writeMesh();
  writer.addNodalScalarField("radial", MyPrinterTet2(m));

  writer.splitMeshEdges(3);
  writer.writeMesh();
  writer.addNodalScalarField("radial", MyPrinterTet2(m));

  writer.splitMeshEdges(4);
  writer.writeMesh();
  writer.addNodalScalarField("radial", MyPrinterTet2(m));

  writer.splitMeshEdges(5);
  writer.writeMesh();
  writer.addNodalScalarField("radial", MyPrinterTet2(m));

}


struct MyPrinterTri2 : public DefaultGetDataVtk
{
  typedef MeshTri::VertexH VertexH;
  typedef MeshTri::CellH CellH;
  MeshTri const& m;
  MyPrinterTri2(MeshTri const& m_) : DefaultGetDataVtk(), m(m_) {}

  virtual void getData(index_t id, Real *values) const
  {
    VertexH(id).coord(&m, values);
  }

  virtual void getData(Real const *x_local, index_t cell_id, int /*ith*/, Real * values) const
  {
    //Real * const values = values_; // UNBELIEVABLE: it is necessary in clang while in gcc it is not. God?
    Real x[3][3];
    CellH c(cell_id);
    VertexH vts[3];
    c.vertices(&m, vts);
    for (int i = 0; i < 3; ++i)
      vts[i].coord(&m, x[i]);

    Real L[3];
    L[0] = 1;
    for (int i = 1; i <= 2; ++i) {
      L[i] = x_local[i-1];
      L[0] -= L[i];
    }

    Real x_real[3] = {0,0,0};
    for (int i = 0; i < 3; ++i)
      for (int j = 0; j < 2; ++j)
        x_real[j] += L[i]*x[i][j];

    values[0] = x_real[0];
    values[1] = x_real[1];

  }

  virtual int numComps() const
  { return 2; }
};

TEST_F(TriMesh2Tests, PrintNodalFieldVtk)
{
  MeshWriter writer;
  writer.attachMesh(&m);
  writer.setBinaryOutput(false);
  writer.setFamily(true);

  writer.setNamePadding(2);
  writer.setOutputFileName("output/TriMesh2.PrintNodalFieldVtk.vtk");

  writer.splitMeshEdges(1);
  writer.writeMesh();
  writer.addNodalScalarField("radial", MyPrinterTri2(m));

  writer.splitMeshEdges(2);
  writer.writeMesh();
  writer.addNodalScalarField("radial", MyPrinterTri2(m));

  writer.splitMeshEdges(3);
  writer.writeMesh();
  writer.addNodalScalarField("radial", MyPrinterTri2(m));

  writer.splitMeshEdges(4);
  writer.writeMesh();
  writer.addNodalScalarField("radial", MyPrinterTri2(m));

  writer.splitMeshEdges(5);
  writer.writeMesh();
  writer.addNodalScalarField("radial", MyPrinterTri2(m));

}

struct MyPrinterTri3 : public DefaultGetDataVtk
{
  typedef MeshTri::VertexH VertexH;
  typedef MeshTri::CellH CellH;
  MeshTri const& m;
  MyPrinterTri3(MeshTri const& m_) : DefaultGetDataVtk(), m(m_) {}

  virtual void getData(index_t id, Real *values) const
  {
    Real x_real[2];
    VertexH(id).coord(&m, x_real);

    // finding angles
    {
      Real x = x_real[0], y = x_real[1];
      Real z = atan(y/x);
      Real t = -fabs(z-pi/4.)+pi/4.;

      if (x != 0)
      {
        x_real[0] = x*cos(t);
        x_real[1] = y*cos(t);
      }
    }

    values[0] = x_real[0];
    values[1] = x_real[1];
  }

  virtual void getData(Real const *x_local, index_t cell_id, int /*ith*/, Real *values) const
  {

    Real x[3][3];
    CellH c(cell_id);
    VertexH vts[3];
    c.vertices(&m, vts);
    for (int i = 0; i < 3; ++i)
      vts[i].coord(&m, x[i]);

    Real L[3];
    L[0] = 1;
    for (int i = 1; i <= 2; ++i) {
      L[i] = x_local[i-1];
      L[0] -= L[i];
    }

    Real x_real[3] = {0,0,0};
    for (int i = 0; i < 3; ++i)
      for (int j = 0; j < 2; ++j)
        x_real[j] += L[i]*x[i][j];

    // finding angles
    {
      Real x = x_real[0], y = x_real[1];
      Real z = atan(y/x);
      Real t = -fabs(z-pi/4.)+pi/4.;

      if (x != 0)
      {
        x_real[0] = x*cos(t);
        x_real[1] = y*cos(t);
      }
    }

    values[0] = x_real[0];
    values[1] = x_real[1];

  }

  virtual int numComps() const
  { return 2; }
};


TEST_F(TriMesh2Tests, PrintCustomCoordsFieldVtk)
{
  MeshWriter writer;
  writer.attachMesh(&m);
  writer.setBinaryOutput(false);
  writer.setFamily(true);

  writer.setNamePadding(2);
  writer.setOutputFileName("output/TriMesh2.PrintCustomCoordsFieldVtk.vtk");

  writer.splitMeshEdges(1);
  writer.writeMesh(MyPrinterTri3(m));
  writer.addNodalScalarField("radial", MyPrinterTri3(m));

  writer.splitMeshEdges(2);
  writer.writeMesh(MyPrinterTri3(m));
  writer.addNodalScalarField("radial", MyPrinterTri3(m));

  writer.splitMeshEdges(3);
  writer.writeMesh(MyPrinterTri3(m));
  writer.addNodalScalarField("radial", MyPrinterTri3(m));

  writer.splitMeshEdges(4);
  writer.writeMesh(MyPrinterTri3(m));
  writer.addNodalScalarField("radial", MyPrinterTri3(m));

  writer.splitMeshEdges(5);
  writer.writeMesh(MyPrinterTri3(m));
  writer.addNodalScalarField("radial", MyPrinterTri3(m));

}


TEST(MshIoTests, IdentifiesMeshTypeTest)
{
  //MeshIoMsh<EDGE>        R1;
  MeshIoMsh<MeshTri> R2;
  //MeshIoMsh<QUADRANGLE>  R3;
  MeshIoMsh<MeshTet> R4;
  //MeshIoMsh<HEXAHEDRON>  R5;

  int sdim;
  ECellType ct;

  //ct = R1.identifiesMeshType("meshes/simple_edge0.msh", &sdim);
  //EXPECT_EQ(EDGE, ct);

  ct = R2.identifiesMeshType("meshes/simple_tri0.msh", &sdim);
  EXPECT_EQ(TRIANGLE, ct);

  //ct = R3.identifiesMeshType("meshes/simple_quad0.msh", &sdim);
  //EXPECT_EQ(QUADRANGLE, ct);

  ct = R4.identifiesMeshType("meshes/simple_tet0.msh", &sdim);
  EXPECT_EQ(TETRAHEDRON, ct);

  //ct = R5.identifiesMeshType("meshes/simple_hex0.msh", &sdim);
  //EXPECT_EQ(HEXAHEDRON, ct);

}


TEST(MshIoTests, ReadFileTest)
{
  //MeshIoMsh<EDGE>        R1;
  MeshIoMsh<MeshTri>    R2;
  //MeshIoMsh<QUADRANGLE>  R3;
  MeshIoMsh<MeshTet> R4;
  //MeshIoMsh<HEXAHEDRON>  R5;

  MeshTri m2;
  MeshTet m4;

  R2.readFile("meshes/simple_tri0.msh", &m2);
  R4.readFile("meshes/simple_tet0.msh", &m4);

  EXPECT_EQ(13u, m2.numVertices());
  EXPECT_EQ(16u, m2.numCells());
  EXPECT_EQ(28u, m2.numFacets());

  EXPECT_EQ(27u,  m4.numVertices());
  EXPECT_EQ(48u,  m4.numCells());
  EXPECT_EQ(120u, m4.numFacets());
  EXPECT_EQ(98u,  m4.numRidges());


  for (index_t i = 0; i < 13; ++i)
    EXPECT_TRUE(MeshTri::VertexH(i).tag(&m2) > 0);

  for (index_t i = 0; i < 16; ++i)
    EXPECT_TRUE(MeshTri::CellH(i).tag(&m2) > 0);

  for (index_t i = 0; i < 28; ++i)
    EXPECT_TRUE(MeshTri::FacetH(i).tag(&m2) > 0);



  EXPECT_EQ(1 , MeshTet::VertexH(0).tag(&m4));
  EXPECT_EQ(2 , MeshTet::VertexH(1).tag(&m4));
  EXPECT_EQ(4 , MeshTet::VertexH(2).tag(&m4));
  EXPECT_EQ(3 , MeshTet::VertexH(3).tag(&m4));
  EXPECT_EQ(13, MeshTet::VertexH(4).tag(&m4));
  EXPECT_EQ(10, MeshTet::VertexH(5).tag(&m4));
  EXPECT_EQ(11, MeshTet::VertexH(6).tag(&m4));
  EXPECT_EQ(12, MeshTet::VertexH(7).tag(&m4));

  for (index_t i = 0; i < 27; ++i)
    EXPECT_TRUE(MeshTet::VertexH(i).tag(&m4) > 0);

  for (index_t i = 0; i < 48; ++i)
    EXPECT_TRUE(MeshTet::CellH(i).tag(&m4) > 0);

  for (index_t i = 0; i < 120; ++i)
    EXPECT_TRUE(MeshTet::FacetH(i).tag(&m4) > 0);

  for (index_t i = 0; i < 98; ++i)
    EXPECT_TRUE(MeshTet::RidgeH(i).tag(&m4) > 0);


}



// ------------------------------------------
// CUSTOM COORDINATES STORAGE VERSION
// ------------------------------------------


class TriMesh1NoCTests : public testing::Test
{
 protected:  // You should make the members protected s.t. they can be
             // accessed from sub-classes.

  typedef MeshTriNoC MeshT;
  typedef MeshT::VertexH VertexH;
  typedef MeshT::CellH CellH;
  typedef MeshT::FacetH FacetH;
  typedef MeshT::RidgeH RidgeH;
  typedef MeshIoVtk<MeshT> MeshWriter;

  TriMesh1NoCTests() : m() {}

  MeshT m;

  static void addTriMesh1(MeshTriNoC &m, bool check = true)
  {
    typedef MeshTriNoC MeshT;
    typedef MeshT::VertexH VertexH;
    typedef MeshT::CellH   CellH;

    int const N = 14;

    VertexH vts[N];
                            // tag
    vts[ 0] = m.addVertex(NULL, 0);
    vts[ 1] = m.addVertex(NULL, 1);
    vts[ 2] = m.addVertex(NULL, 2);
    vts[ 3] = m.addVertex(NULL, 3);
    vts[ 4] = m.addVertex(NULL, 4);
    vts[ 5] = m.addVertex(NULL, 5);
    vts[ 6] = m.addVertex(NULL, 6);
    vts[ 7] = m.addVertex(NULL, 7);
    vts[ 8] = m.addVertex(NULL, 8);
    vts[ 9] = m.addVertex(NULL, 9);
    vts[10] = m.addVertex(NULL,10);
    vts[11] = m.addVertex(NULL,11);
    vts[12] = m.addVertex(NULL,12);
    vts[13] = m.addVertex(NULL,13);

    int const E = 11;

    CellH cells[E];

    cells[ 0] = m.addCell(listOf(vts[ 0], vts[ 1], vts[ 2])); //  0
    cells[ 1] = m.addCell(listOf(vts[ 1], vts[ 3], vts[ 4])); //  1
    cells[ 2] = m.addCell(listOf(vts[ 2], vts[ 6], vts[ 5])); //  2
    cells[ 3] = m.addCell(listOf(vts[ 3], vts[ 4], vts[ 7])); //  3
    cells[ 4] = m.addCell(listOf(vts[ 5], vts[10], vts[ 9])); //  4
    cells[ 5] = m.addCell(listOf(vts[ 5], vts[ 6], vts[10])); //  5
    cells[ 6] = m.addCell(listOf(vts[ 6], vts[11], vts[10])); //  6
    cells[ 7] = m.addCell(listOf(vts[ 6], vts[ 7], vts[11])); //  7
    cells[ 8] = m.addCell(listOf(vts[ 7], vts[ 8], vts[12])); //  8
    cells[ 9] = m.addCell(listOf(vts[13], vts[ 6], vts[10])); //  9
    cells[10] = m.addCell(listOf(vts[ 2], vts[ 7], vts[ 6])); // 10

    // attention: this vertex is boundary because of the third triangle added to its edge
    EXPECT_TRUE(vts[ 6].isBoundary(&m));

    // if we add the triangle:
    m.addCell(listOf(vts[ 6], vts[ 7], vts[13]));

    // now the vertex should not be boundary:
    EXPECT_FALSE(vts[ 6].isBoundary(&m));

    EXPECT_TRUE(cells[ 3].adjCell(&m, 0) == cells[ 1]);
    EXPECT_TRUE(cells[ 1].adjCell(&m, 1) == cells[ 3]);


    // specific checking
    std::vector<VertexH> cverts(3);


    cverts[0] = vts[ 0]; cverts[1] = vts[ 1]; cverts[2] = vts[ 2]; EXPECT_TRUE(cverts == cells[0].vertices(&m));
    cverts[0] = vts[ 1]; cverts[1] = vts[ 3]; cverts[2] = vts[ 4]; EXPECT_TRUE(cverts == cells[1].vertices(&m));
    cverts[0] = vts[ 2]; cverts[1] = vts[ 6]; cverts[2] = vts[ 5]; EXPECT_TRUE(cverts == cells[2].vertices(&m));
    cverts[0] = vts[ 3]; cverts[1] = vts[ 4]; cverts[2] = vts[ 7]; EXPECT_TRUE(cverts == cells[3].vertices(&m));
    cverts[0] = vts[ 5]; cverts[1] = vts[10]; cverts[2] = vts[ 9]; EXPECT_TRUE(cverts == cells[4].vertices(&m));
    cverts[0] = vts[ 5]; cverts[1] = vts[ 6]; cverts[2] = vts[10]; EXPECT_TRUE(cverts == cells[5].vertices(&m));
    cverts[0] = vts[ 6]; cverts[1] = vts[11]; cverts[2] = vts[10]; EXPECT_TRUE(cverts == cells[6].vertices(&m));
    cverts[0] = vts[ 6]; cverts[1] = vts[ 7]; cverts[2] = vts[11]; EXPECT_TRUE(cverts == cells[7].vertices(&m));
    cverts[0] = vts[ 7]; cverts[1] = vts[ 8]; cverts[2] = vts[12]; EXPECT_TRUE(cverts == cells[8].vertices(&m));
    cverts[0] = vts[13]; cverts[1] = vts[ 6]; cverts[2] = vts[10]; EXPECT_TRUE(cverts == cells[9].vertices(&m));

    EXPECT_EQ(14u, m.numVertices());
    EXPECT_EQ(12u, m.numCells());
    EXPECT_EQ(26u, m.numFacets());

    if (!check)
      return;

    checkMesh(m);


  }

  // virtual void SetUp() will be called before each test is run.  You
  // should define it if you need to initialize the varaibles.
  // Otherwise, this can be skipped.
  virtual void SetUp() {
    //std::cout << "HHHHHHHHHHEEEEEEEEEEEERRRRRRRRRRRRREEEEEEEEEEEEEEEEEEEE: " << typeid(MeshT::CellT).name() << std::endl;
    //std::cout << "HHHHHHHHHHEEEEEEEEEEEERRRRRRRRRRRRREEEEEEEEEEEEEEEEEEEE: " << typeid(Cell<TRIANGLE>).name() << std::endl;
    addTriMesh1(m, true);

  }

  // virtual void TearDown() will be called after each test is run.
  // You should define it if there is cleanup work to do.  Otherwise,
  // you don't have to provide it.
  //
  virtual void TearDown() {
  }




};

class TriMesh2NoCTests : public testing::Test
{
 protected:  // You should make the members protected s.t. they can be
             // accessed from sub-classes.

  typedef MeshTriNoC MeshT;
  typedef MeshT::VertexH VertexH;
  typedef MeshT::CellH CellH;
  typedef MeshT::FacetH FacetH;
  typedef MeshT::RidgeH RidgeH;
  typedef MeshIoVtk<MeshT> MeshWriter;

  TriMesh2NoCTests() : m() {}

  MeshT m;

  static void addTriMesh(MeshT &m)
  {
    int const N = 4;

    VertexH vts[N];
                                               // tag
    vts[ 0] = m.addVertex(NULL, 0);
    vts[ 1] = m.addVertex(NULL, 1);
    vts[ 2] = m.addVertex(NULL, 2);
    vts[ 3] = m.addVertex(NULL, 3);

    int const E = 2;

    CellH cells[E];

    cells[ 0] = m.addCell(listOf(vts[ 0], vts[ 1], vts[ 2])); //  0
    cells[ 1] = m.addCell(listOf(vts[ 2], vts[ 1], vts[ 3])); //  1

    checkMesh(m);
  }

  // virtual void SetUp() will be called before each test is run.  You
  // should define it if you need to initialize the varaibles.
  // Otherwise, this can be skipped.
  virtual void SetUp() {
    addTriMesh(m);
  }

  // virtual void TearDown() will be called after each test is run.
  // You should define it if there is cleanup work to do.  Otherwise,
  // you don't have to provide it.
  //
  virtual void TearDown() {
  }


};


class TetMesh1NoCTests : public testing::Test
{
 protected:  // You should make the members protected s.t. they can be
             // accessed from sub-classes.

  typedef MeshTetNoC MeshT;
  typedef MeshT::VertexH VertexH;
  typedef MeshT::CellH CellH;
  typedef MeshT::FacetH FacetH;
  typedef MeshT::RidgeH RidgeH;
  typedef MeshIoVtk<MeshT> MeshWriter;

  TetMesh1NoCTests() : m() {}

  MeshT m;

  static void addTetMesh1(MeshT &m, bool check = true)
  {
    int const N = 35;

    VertexH vts[N];

    vts[ 0] = m.addVertex(NULL);
    vts[ 1] = m.addVertex(NULL);
    vts[ 2] = m.addVertex(NULL);
    vts[ 3] = m.addVertex(NULL);
    vts[ 4] = m.addVertex(NULL);
    vts[ 5] = m.addVertex(NULL);
    vts[ 6] = m.addVertex(NULL);
    vts[ 7] = m.addVertex(NULL);
    vts[ 8] = m.addVertex(NULL);
    vts[ 9] = m.addVertex(NULL);
    vts[10] = m.addVertex(NULL);
    vts[11] = m.addVertex(NULL);
    vts[12] = m.addVertex(NULL);
    vts[13] = m.addVertex(NULL);
    vts[14] = m.addVertex(NULL);
    vts[15] = m.addVertex(NULL);
    vts[16] = m.addVertex(NULL);
    vts[17] = m.addVertex(NULL);
    vts[18] = m.addVertex(NULL);
    vts[19] = m.addVertex(NULL);
    vts[20] = m.addVertex(NULL);
    vts[21] = m.addVertex(NULL);
    vts[22] = m.addVertex(NULL);
    vts[23] = m.addVertex(NULL);
    vts[24] = m.addVertex(NULL);
    vts[25] = m.addVertex(NULL);
    vts[26] = m.addVertex(NULL);
    vts[27] = m.addVertex(NULL);
    vts[28] = m.addVertex(NULL);
    vts[29] = m.addVertex(NULL);
    vts[30] = m.addVertex(NULL);
    vts[31] = m.addVertex(NULL);
    vts[32] = m.addVertex(NULL);
    vts[33] = m.addVertex(NULL);
    vts[34] = m.addVertex(NULL);

    int const E = 64;

    CellH cells[E];

    cells[ 0] = m.addCell(listOf(vts[0 ], vts[4 ], vts[12], vts[15]));
    cells[ 1] = m.addCell(listOf(vts[24], vts[30], vts[15], vts[26]));
    cells[ 2] = m.addCell(listOf(vts[24], vts[15], vts[4 ], vts[26]));
    cells[ 3] = m.addCell(listOf(vts[15], vts[30], vts[24], vts[12]));
    cells[ 4] = m.addCell(listOf(vts[4 ], vts[15], vts[24], vts[12]));
    cells[ 5] = m.addCell(listOf(vts[26], vts[24], vts[30], vts[34]));
    cells[ 6] = m.addCell(listOf(vts[15], vts[26], vts[30], vts[14]));
    cells[ 7] = m.addCell(listOf(vts[34], vts[29], vts[14], vts[25]));
    cells[ 8] = m.addCell(listOf(vts[34], vts[14], vts[26], vts[25]));
    cells[ 9] = m.addCell(listOf(vts[14], vts[29], vts[34], vts[30]));
    cells[10] = m.addCell(listOf(vts[26], vts[14], vts[34], vts[30]));
    cells[11] = m.addCell(listOf(vts[25], vts[34], vts[29], vts[33]));
    cells[12] = m.addCell(listOf(vts[14], vts[25], vts[29], vts[13]));
    cells[13] = m.addCell(listOf(vts[33], vts[16], vts[13], vts[19]));
    cells[14] = m.addCell(listOf(vts[33], vts[13], vts[25], vts[19]));
    cells[15] = m.addCell(listOf(vts[13], vts[16], vts[33], vts[29]));
    cells[16] = m.addCell(listOf(vts[25], vts[13], vts[33], vts[29]));
    cells[17] = m.addCell(listOf(vts[13], vts[19], vts[16], vts[3 ]));
    cells[18] = m.addCell(listOf(vts[12], vts[24], vts[11], vts[30]));
    cells[19] = m.addCell(listOf(vts[22], vts[28], vts[30], vts[34]));
    cells[20] = m.addCell(listOf(vts[22], vts[30], vts[24], vts[34]));
    cells[21] = m.addCell(listOf(vts[30], vts[28], vts[22], vts[11]));
    cells[22] = m.addCell(listOf(vts[24], vts[30], vts[22], vts[11]));
    cells[23] = m.addCell(listOf(vts[34], vts[22], vts[28], vts[32]));
    cells[24] = m.addCell(listOf(vts[30], vts[34], vts[28], vts[29]));
    cells[25] = m.addCell(listOf(vts[32], vts[17], vts[29], vts[33]));
    cells[26] = m.addCell(listOf(vts[32], vts[29], vts[34], vts[33]));
    cells[27] = m.addCell(listOf(vts[29], vts[17], vts[32], vts[28]));
    cells[28] = m.addCell(listOf(vts[34], vts[29], vts[32], vts[28]));
    cells[29] = m.addCell(listOf(vts[29], vts[33], vts[17], vts[16]));
    cells[30] = m.addCell(listOf(vts[11], vts[22], vts[10], vts[28]));
    cells[31] = m.addCell(listOf(vts[9 ], vts[18], vts[28], vts[32]));
    cells[32] = m.addCell(listOf(vts[9 ], vts[28], vts[22], vts[32]));
    cells[33] = m.addCell(listOf(vts[28], vts[18], vts[9 ], vts[10]));
    cells[34] = m.addCell(listOf(vts[22], vts[28], vts[9 ], vts[10]));
    cells[35] = m.addCell(listOf(vts[28], vts[32], vts[18], vts[17]));
    cells[36] = m.addCell(listOf(vts[10], vts[9 ], vts[2 ], vts[18]));
    cells[37] = m.addCell(listOf(vts[4 ], vts[5 ], vts[24], vts[26]));
    cells[38] = m.addCell(listOf(vts[23], vts[34], vts[26], vts[27]));
    cells[39] = m.addCell(listOf(vts[23], vts[26], vts[5 ], vts[27]));
    cells[40] = m.addCell(listOf(vts[26], vts[34], vts[23], vts[24]));
    cells[41] = m.addCell(listOf(vts[5 ], vts[26], vts[23], vts[24]));
    cells[42] = m.addCell(listOf(vts[27], vts[23], vts[34], vts[31]));
    cells[43] = m.addCell(listOf(vts[26], vts[27], vts[34], vts[25]));
    cells[44] = m.addCell(listOf(vts[31], vts[33], vts[25], vts[20]));
    cells[45] = m.addCell(listOf(vts[31], vts[25], vts[27], vts[20]));
    cells[46] = m.addCell(listOf(vts[25], vts[33], vts[31], vts[34]));
    cells[47] = m.addCell(listOf(vts[27], vts[25], vts[31], vts[34]));
    cells[48] = m.addCell(listOf(vts[25], vts[20], vts[33], vts[19]));
    cells[49] = m.addCell(listOf(vts[24], vts[23], vts[22], vts[34]));
    cells[50] = m.addCell(listOf(vts[8 ], vts[32], vts[34], vts[31]));
    cells[51] = m.addCell(listOf(vts[8 ], vts[34], vts[23], vts[31]));
    cells[52] = m.addCell(listOf(vts[34], vts[32], vts[8 ], vts[22]));
    cells[53] = m.addCell(listOf(vts[23], vts[34], vts[8 ], vts[22]));
    cells[54] = m.addCell(listOf(vts[34], vts[31], vts[32], vts[33]));
    cells[55] = m.addCell(listOf(vts[22], vts[8 ], vts[9 ], vts[32]));
    cells[56] = m.addCell(listOf(vts[5 ], vts[6 ], vts[23], vts[27]));
    cells[57] = m.addCell(listOf(vts[7 ], vts[31], vts[27], vts[21]));
    cells[58] = m.addCell(listOf(vts[7 ], vts[27], vts[6 ], vts[21]));
    cells[59] = m.addCell(listOf(vts[27], vts[31], vts[7 ], vts[23]));
    cells[60] = m.addCell(listOf(vts[6 ], vts[27], vts[7 ], vts[23]));
    cells[61] = m.addCell(listOf(vts[27], vts[21], vts[31], vts[20]));
    cells[62] = m.addCell(listOf(vts[23], vts[7 ], vts[8 ], vts[31]));
    cells[63] = m.addCell(listOf(vts[6 ], vts[1 ], vts[7 ], vts[21]));



    //// attention: this vertex is boundary because of the third triangle added to its edge
    //EXPECT_TRUE(vts[ 6].isBoundary(&m));
    //
    //// if we add the triangle:
    //m.addCell(listOf(vts[ 6], vts[ 7], vts[13]));
    //
    //// now the vertex should not be boundary:
    //EXPECT_FALSE(vts[ 6].isBoundary(&m));
    //
    //EXPECT_TRUE(cells[ 3].adjCell(&m, 0) == cells[ 1]);
    //EXPECT_TRUE(cells[ 1].adjCell(&m, 1) == cells[ 3]);
    //
    //
    //// specific checking
    //std::vector<VertexH> cverts(3);
    //
    //
    //cverts[0] = vts[ 0]; cverts[1] = vts[ 1]; cverts[2] = vts[ 2]; EXPECT_TRUE(cverts == cells[0].vertices(&m));
    //cverts[0] = vts[ 1]; cverts[1] = vts[ 3]; cverts[2] = vts[ 4]; EXPECT_TRUE(cverts == cells[1].vertices(&m));
    //cverts[0] = vts[ 2]; cverts[1] = vts[ 6]; cverts[2] = vts[ 5]; EXPECT_TRUE(cverts == cells[2].vertices(&m));
    //cverts[0] = vts[ 3]; cverts[1] = vts[ 4]; cverts[2] = vts[ 7]; EXPECT_TRUE(cverts == cells[3].vertices(&m));
    //cverts[0] = vts[ 5]; cverts[1] = vts[10]; cverts[2] = vts[ 9]; EXPECT_TRUE(cverts == cells[4].vertices(&m));
    //cverts[0] = vts[ 5]; cverts[1] = vts[ 6]; cverts[2] = vts[10]; EXPECT_TRUE(cverts == cells[5].vertices(&m));
    //cverts[0] = vts[ 6]; cverts[1] = vts[11]; cverts[2] = vts[10]; EXPECT_TRUE(cverts == cells[6].vertices(&m));
    //cverts[0] = vts[ 6]; cverts[1] = vts[ 7]; cverts[2] = vts[11]; EXPECT_TRUE(cverts == cells[7].vertices(&m));
    //cverts[0] = vts[ 7]; cverts[1] = vts[ 8]; cverts[2] = vts[12]; EXPECT_TRUE(cverts == cells[8].vertices(&m));
    //cverts[0] = vts[13]; cverts[1] = vts[ 6]; cverts[2] = vts[10]; EXPECT_TRUE(cverts == cells[9].vertices(&m));
    //
    //EXPECT_EQ(14u, m.numVertices());
    //EXPECT_EQ(12u, m.numCells());
    //EXPECT_EQ(26u, m.numFacets());

    if (!check)
      return;

    checkMesh(m);


  }

  // virtual void SetUp() will be called before each test is run.  You
  // should define it if you need to initialize the varaibles.
  // Otherwise, this can be skipped.
  virtual void SetUp() {

    addTetMesh1(m, true);

  }

  // virtual void TearDown() will be called after each test is run.
  // You should define it if there is cleanup work to do.  Otherwise,
  // you don't have to provide it.
  //
  virtual void TearDown() {
  }




};

class TetMesh2NoCTests : public testing::Test
{
 protected:  // You should make the members protected s.t. they can be
             // accessed from sub-classes.

  typedef MeshTetNoC MeshT;
  typedef MeshT::VertexH VertexH;
  typedef MeshT::CellH CellH;
  typedef MeshT::FacetH FacetH;
  typedef MeshT::RidgeH RidgeH;
  typedef MeshIoVtk<MeshT> MeshWriter;

  TetMesh2NoCTests() : m() {}

  MeshT m;

  static void addTetMesh(MeshT &m, bool check = true)
  {
    int const N = 5;

    VertexH vts[N];
                                               // tag
    vts[ 0] = m.addVertex(NULL, 0);
    vts[ 1] = m.addVertex(NULL, 1);
    vts[ 2] = m.addVertex(NULL, 2);
    vts[ 3] = m.addVertex(NULL, 3);
    vts[ 4] = m.addVertex(NULL, 4);

    int const E = 2;

    CellH cells[E];

    //cells[ 0] = m.addCell(listOf(vts[ 0], vts[ 1], vts[ 2], vts[ 3])); //  0
    //cells[ 0] = m.addCell(listOf(vts[ 1], vts[ 2], vts[ 0], vts[ 3])); //  0
    cells[ 0] = m.addCell(listOf(vts[ 2], vts[ 3], vts[ 0], vts[ 1])); //  0
    //cells[ 0] = m.addCell(listOf(vts[ 1], vts[ 3], vts[ 2], vts[ 0])); //  0
    //cells[ 0] = m.addCell(listOf(vts[ 1], vts[ 3], vts[ 2], vts[ 0])); //  0
    //cells[ 0] = m.addCell(listOf(vts[ 1], vts[ 3], vts[ 2], vts[ 0])); //  0

    cells[ 1] = m.addCell(listOf(vts[ 0], vts[ 4], vts[ 1], vts[ 3])); //  1

    if (!check)
      return;

    checkMesh(m);
  }

  // virtual void SetUp() will be called before each test is run.  You
  // should define it if you need to initialize the varaibles.
  // Otherwise, this can be skipped.
  virtual void SetUp() {

    addTetMesh(m, true);

  }

  // virtual void TearDown() will be called after each test is run.
  // You should define it if there is cleanup work to do.  Otherwise,
  // you don't have to provide it.
  //
  virtual void TearDown() {
  }




};


TEST(MeshNoCTest, Initialize2d)
{
  MeshEdgNoC m0;
  MeshTriNoC m1;
  MeshQuaNoC m2;
  MeshTetNoC m3;
  MeshHexNoC m4;

  //MeshEdg::printElementsSize();
  //MeshTri::printElementsSize();
  //MeshQua::printElementsSize();
  MeshTetNoC::printElementsSize();
  //MeshHex::printElementsSize();
}


TEST(MeshNoCTest, AddVertex)
{
  typedef MeshTri MeshT;
  typedef MeshT::VertexH VertexH;
  MeshT m;

  EXPECT_EQ(0u, m.numVertices());

  VertexH vts[5];

  for (unsigned i = 0; i < 5; ++i)
  {
    vts[i] = m.addVertex(listOf<Real>(i,i,0));
    EXPECT_EQ(i+1, m.numVertices());
    EXPECT_EQ(0u, vts[i].valency(&m));
  }

  for (int j = 0; j < 2; ++j)
    for (unsigned i = 0; i < 5; ++i)
      EXPECT_NEAR(Real(i), vts[i].coord(&m, j), 1e-15);

}

TEST(MeshNoCTest, RemoveUnrefVertex)
{
  typedef MeshTriNoC MeshT;
  typedef MeshT::VertexH VertexH;
  MeshT m;

  VertexH vts[5];

  for (unsigned i = 0; i < 5; ++i)
  { vts[i] = m.addVertex(); }

  m.removeUnrefVertex(vts[2]);
  EXPECT_EQ(index_t(4), (index_t)m.numVertices());
  m.removeUnrefVertex(vts[0]);
  EXPECT_EQ(index_t(3), (index_t)m.numVertices());
  m.removeUnrefVertex(vts[4]);
  EXPECT_EQ(index_t(2), (index_t)m.numVertices());

  EXPECT_EQ(index_t(5), (index_t)m.numVerticesTotal());

  EXPECT_TRUE( vts[2].isDisabled(&m));
  EXPECT_TRUE( vts[0].isDisabled(&m));
  EXPECT_TRUE( vts[4].isDisabled(&m));
  EXPECT_TRUE(!vts[1].isDisabled(&m));
  EXPECT_TRUE(!vts[3].isDisabled(&m));

  m.removeUnrefVertices();

  for (unsigned i = 0; i < 5; ++i)
  { EXPECT_TRUE( vts[i].isDisabled(&m)); }

}

TEST_F(TriMesh1NoCTests, AddCell)
{
  checkMesh(m);
}

TEST_F(TriMesh1NoCTests, RemoveSingularCell)
{
  m.removeCell(CellH(9), false);
  // if we remove the cell 9, vtx 6 must be boundary
  EXPECT_TRUE(VertexH(6).isBoundary(&m));
  EXPECT_EQ(14u, m.numVertices());
  EXPECT_EQ(11u, m.numCells());
  EXPECT_EQ(25u, m.numFacets());
  checkMesh(m);

  // now, if we remove the cell 11, vtx 6 must be non-boundary
  m.removeCell(CellH(11), false);
  EXPECT_FALSE(VertexH(6).isBoundary(&m));
  EXPECT_EQ(14u, m.numVertices());
  EXPECT_EQ(10u, m.numCells());
  EXPECT_EQ(23u, m.numFacets());
  checkMesh(m);

  EXPECT_TRUE( m.removeUnrefVertex(13) );
  EXPECT_EQ(13u, m.numVertices());

  m.removeCell(CellH(5), true);
  EXPECT_TRUE(VertexH(6).isBoundary(&m));
  EXPECT_EQ(13u, m.numVertices());
  EXPECT_EQ(9u, m.numCells());
  EXPECT_EQ(23u, m.numFacets());
  checkMesh(m);
}

TEST_F(TriMesh1NoCTests, AddAndRemoveCell)
{
  unsigned k = 0;
  for (CellH ch = m.cellBegin(); ch != m.cellEnd(); ++ch)
  {
    if (ch.isDisabled(&m))
      continue;

    m.removeCell(ch, true);
    ++k;
    EXPECT_EQ(12u-k, m.numCells());
    checkMesh(m);
  }

  EXPECT_EQ(0u, m.numVertices());
  EXPECT_EQ(0u, m.numCells());
  EXPECT_EQ(0u, m.numFacets());


  // add again:
  addTriMesh1(m, false);
  checkMesh(m);
}



struct MyPrinterNoCTri3 : public DefaultGetDataVtk
{
  typedef MeshTriNoC MeshT;
  typedef MeshT::VertexH VertexH;
  typedef MeshT::CellH CellH;
  MeshT const& m;
  Real const* data;
  VarDofs<MeshT> const& var;
  mutable std::vector<index_t> dofs;

  explicit
  MyPrinterNoCTri3(MeshT const& m_, Real const* d, VarDofs<MeshT> const& v) : DefaultGetDataVtk(), m(m_), data(d), var(v), dofs(var.numDofsPerCell()) {}

    // In this example, the numbering of the degrees of freedom coincides with the mesh numbering
  virtual void getData(index_t id, Real *values) const
  {
    var.getVertexDofs(dofs.begin(), VertexH(id));
    values[0] = data[dofs[0]];
    values[1] = data[dofs[1]];
  }

  // In this example, the numbering of the degrees of freedom coincides with the mesh numbering
  virtual void getData(Real const */*x_local*/, index_t cell_id, int ith, Real *values) const
  {
    var.getCellDofs(dofs.data(), CellH(cell_id));

    values[0] = data[dofs[ith*2 + 0]];
    values[1] = data[dofs[ith*2 + 1]];
  }

  virtual int numComps() const
  { return 2; }
};


TEST_F(TriMesh2NoCTests, PrintCustomCoordsFieldVtk)
{
  MeshIoMsh<MeshT> R;

  MeshWriter writer;
  writer.attachMesh(&m);
  writer.setBinaryOutput(false);
  writer.setFamily(true);
  writer.setOutputFileName("output/TriMesh2NoCTestsa.vtk");
  writer.setNamePadding(2);
  // linear
  {
    R.readFile("meshes/circle_tri0.msh", &m);
    EXPECT_EQ(5, (int)m.numVertices());
    EXPECT_EQ(4, (int)m.numCells());
    EXPECT_EQ(8, (int)m.numFacets());

    DofMapper<MeshT> map(&m);  //            ndpv,  ndpr,  ndpf,  ndpc
    map.addVariable("coordinate", MeshT::SpaceDim,     0,     0,     0);
    map.SetUp();
    EXPECT_EQ(10, (int)map.numDofs());
    std::vector<Real> coords(map.numDofs());

    R.readCoordinates("meshes/circle_tri0.msh", &m, coords.data(), map.variable(0));

    writer.writeMesh(MyPrinterNoCTri3(m, coords.data(), map.variable(0)));
  }

  // quadratic
  {
    R.readFile("meshes/circle_tri1.msh", &m);
    EXPECT_EQ(5, (int)m.numVertices());
    EXPECT_EQ(4, (int)m.numCells());
    EXPECT_EQ(8, (int)m.numFacets());
    
    DofMapper<MeshT> map(&m);  //            ndpv, ndpr,            ndpf,  ndpc
    map.addVariable("coordinate", MeshT::SpaceDim,    0, MeshT::SpaceDim,     0);
    map.SetUp();
    EXPECT_EQ(26, (int)map.numDofs());
    std::vector<Real> coords(map.numDofs());

    R.readCoordinates("meshes/circle_tri1.msh", &m, coords.data(), map.variable(0));;

    writer.splitMeshEdges(2);
    writer.writeMesh(MyPrinterNoCTri3(m, coords.data(), map.variable(0)));
  }

  // cubic
  {
    R.readFile("meshes/circle_tri2.msh", &m);
    EXPECT_EQ(5, (int)m.numVertices());
    EXPECT_EQ(4, (int)m.numCells());
    EXPECT_EQ(8, (int)m.numFacets());
    
    DofMapper<MeshT> map(&m);  //            ndpv, ndpr,              ndpf,      ndpc
    map.addVariable("coordinate", MeshT::SpaceDim,    0, 2*MeshT::SpaceDim, MeshT::SpaceDim);
    map.SetUp();
    EXPECT_EQ(50, (int)map.numDofs());
    std::vector<Real> coords(map.numDofs());

    R.readCoordinates("meshes/circle_tri2.msh", &m, coords.data(), map.variable(0));;

    writer.splitMeshEdges(3);
    writer.writeMesh(MyPrinterNoCTri3(m, coords.data(), map.variable(0)));
  }

  // quartic
  {
    R.readFile("meshes/circle_tri3.msh", &m);
    EXPECT_EQ(5, (int)m.numVertices());
    EXPECT_EQ(4, (int)m.numCells());
    EXPECT_EQ(8, (int)m.numFacets());
    
    DofMapper<MeshT> map(&m);  //            ndpv, ndpr,              ndpf,      ndpc
    map.addVariable("coordinate", MeshT::SpaceDim,    0, 3*MeshT::SpaceDim, 3*MeshT::SpaceDim);
    map.SetUp();
    EXPECT_EQ(82, (int)map.numDofs());
    std::vector<Real> coords(map.numDofs());

    R.readCoordinates("meshes/circle_tri3.msh", &m, coords.data(), map.variable(0));;

    writer.splitMeshEdges(4);
    writer.writeMesh(MyPrinterNoCTri3(m, coords.data(), map.variable(0)));
  }

  // 10th
  {
    R.readFile("meshes/circle_tri9.msh", &m);
    EXPECT_EQ(5, (int)m.numVertices());
    EXPECT_EQ(4, (int)m.numCells());
    EXPECT_EQ(8, (int)m.numFacets());
    
    DofMapper<MeshT> map(&m);  //            ndpv, ndpr,              ndpf,      ndpc
    map.addVariable("coordinate", MeshT::SpaceDim,    0, 9*MeshT::SpaceDim, 36*MeshT::SpaceDim);
    map.SetUp();
    EXPECT_EQ(442, (int)map.numDofs());
    std::vector<Real> coords(map.numDofs());

    R.readCoordinates("meshes/circle_tri9.msh", &m, coords.data(), map.variable(0));;

    writer.splitMeshEdges(10);
    writer.writeMesh(MyPrinterNoCTri3(m, coords.data(), map.variable(0)));
  }


}


struct MyPrinterNoCTet2 : public DefaultGetDataVtk
{
  typedef MeshTetNoC MeshT;
  typedef MeshT::VertexH VertexH;
  typedef MeshT::CellH CellH;
  MeshT const& m;
  Real const* data;
  VarDofs<MeshT> const& var;
  mutable std::vector<index_t> dofs;
  
  explicit
  MyPrinterNoCTet2(MeshT const& m_, Real const* d, VarDofs<MeshT> const& v) : DefaultGetDataVtk(), m(m_), data(d), var(v), dofs(var.numDofsPerCell()) {}

  virtual void getData(index_t id, Real *values) const
  {
    var.getVertexDofs(dofs.begin(), VertexH(id));
    values[0] = data[dofs[0]];
    values[1] = data[dofs[1]];
    values[2] = data[dofs[2]];
  }

  virtual void getData(Real const */*x_local*/, index_t cell_id, int ith, Real *values) const
  {
    var.getCellDofs(dofs.data(), CellH(cell_id));

    values[0] = data[dofs[ith*3 + 0]];
    values[1] = data[dofs[ith*3 + 1]];
    values[2] = data[dofs[ith*3 + 2]];
  }

  virtual int numComps() const
  { return 3; }
};


TEST_F(TetMesh2NoCTests, PrintCustomCoordsFieldVtk)
{
  MeshIoMsh<MeshT> R;

  MeshWriter writer;
  writer.attachMesh(&m);
  writer.setBinaryOutput(false);
  writer.setFamily(true);
  writer.setOutputFileName("output/TetMesh2NoC.vtk");
  writer.setNamePadding(2);
  // linear
  {
    R.readFile("meshes/sphere_tet0.msh", &m);
    EXPECT_EQ(6, (int)m.numVertices());
    EXPECT_EQ(4, (int)m.numCells());
    EXPECT_EQ(12, (int)m.numFacets());
    EXPECT_EQ(13, (int)m.numRidges());

    DofMapper<MeshT> map(&m);  //            ndpv,  ndpr,  ndpf,  ndpc
    map.addVariable("coordinate", MeshT::SpaceDim,     0,     0,     0);
    map.SetUp();
    EXPECT_EQ(18, (int)map.numDofs());
    std::vector<Real> coords(map.numDofs());
    
    R.readCoordinates("meshes/sphere_tet0.msh", &m, coords.data(), map.variable(0));
    
    writer.writeMesh(MyPrinterNoCTet2(m, coords.data(), map.variable(0)));
  }


  // quadratic
  {
    R.readFile("meshes/sphere_tet1.msh", &m);
    EXPECT_EQ(6, (int)m.numVertices());
    EXPECT_EQ(4, (int)m.numCells());
    EXPECT_EQ(12, (int)m.numFacets());
    EXPECT_EQ(13, (int)m.numRidges());
    
    DofMapper<MeshT> map(&m);  //            ndpv, ndpr,            ndpf,  ndpc
    map.addVariable("coordinate", MeshT::SpaceDim, MeshT::SpaceDim, 0,     0);
    map.SetUp();
    EXPECT_EQ(57, (int)map.numDofs());
    std::vector<Real> coords(map.numDofs());

    R.readCoordinates("meshes/sphere_tet1.msh", &m, coords.data(), map.variable(0));;

    //for (int i = 0; i < (int)coords.size()/3; ++i)
    //{
    //  for (int j = 0; j < 3; ++j)
    //  {
    //    std::cout << coords[i*3 + j] << " ";
    //  }
    //  std::cout << std::endl;
    //}
    

    writer.splitMeshEdges(2);
    writer.writeMesh(MyPrinterNoCTet2(m, coords.data(), map.variable(0)));
  }


  // cubic
  {
    R.readFile("meshes/sphere_tet2.msh", &m);
    EXPECT_EQ(6, (int)m.numVertices());
    EXPECT_EQ(4, (int)m.numCells());
    EXPECT_EQ(12, (int)m.numFacets());
    EXPECT_EQ(13, (int)m.numRidges());
    
    DofMapper<MeshT> map(&m);  //            ndpv, ndpr,              ndpf,      ndpc
    map.addVariable("coordinate", MeshT::SpaceDim, 2*MeshT::SpaceDim, MeshT::SpaceDim, 0);
    map.SetUp();
    EXPECT_EQ(132, (int)map.numDofs());
    std::vector<Real> coords(map.numDofs());

    R.readCoordinates("meshes/sphere_tet2.msh", &m, coords.data(), map.variable(0));;

    writer.splitMeshEdges(3);
    writer.writeMesh(MyPrinterNoCTet2(m, coords.data(), map.variable(0)));
  }


  // Something is wrong with the element ordering for degrees above 4.
  // I think that the culprit is Gmsh.
  # if 0 // 4th  
  // quartic
  {
    R.readFile("meshes/sphere_tet3.msh", &m);
    EXPECT_EQ(6, (int)m.numVertices());
    EXPECT_EQ(4, (int)m.numCells());
    EXPECT_EQ(12, (int)m.numFacets());
    EXPECT_EQ(13, (int)m.numRidges());
    
    DofMapper<MeshT> map(&m);  //            ndpv, ndpr,              ndpf,      ndpc
    map.addVariable("coordinate", MeshT::SpaceDim, 3*MeshT::SpaceDim, 3*MeshT::SpaceDim, MeshT::SpaceDim);
    map.SetUp();
    EXPECT_EQ(255, (int)map.numDofs());
    std::vector<Real> coords(map.numDofs());

    ASSERT_ANY_THROW(R.readCoordinates("meshes/sphere_tet3.msh", &m, coords.data(), map.variable(0)));

    //writer.splitMeshEdges(4);
    //writer.writeMesh(MyPrinterNoCTet2(m, coords.data(), map.variable(0)));
  }
  
 
  
  // 10th
  {
    R.readFile("meshes/sphere_tet9.msh", &m);
    EXPECT_EQ(6, (int)m.numVertices());
    EXPECT_EQ(4, (int)m.numCells());
    EXPECT_EQ(12, (int)m.numFacets());
    EXPECT_EQ(13, (int)m.numRidges());
    
    DofMapper<MeshT> map(&m);  //            ndpv, ndpr,              ndpf,      ndpc
    map.addVariable("coordinate", MeshT::SpaceDim,    0, 9*MeshT::SpaceDim, 36*MeshT::SpaceDim);
    map.SetUp();
    EXPECT_EQ(442, (int)map.numDofs());
    std::vector<Real> coords(map.numDofs());

    R.readCoordinates("meshes/sphere_tet9.msh", &m, coords.data(), map.variable(0));;

    writer.splitMeshEdges(10);
    writer.writeMesh(MyPrinterNoCTet2(m, coords.data(), map.variable(0)));
  }

  #endif

}



} // MESH_TEST_CPP

