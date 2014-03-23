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



#include <gtest/gtest.h>
#include <Alelib/Mesh>
#include <Alelib/IO>
#include <tr1/array>
#include <tr1/tuple>
#include <algorithm>
#include <tr1/memory>

using namespace alelib;
using namespace std;

static const int dim1 = 1;
static const int dim2 = 2;
static const int dim3 = 3;

typedef Mesh<EDGE>        MeshEdg;
typedef Mesh<TRIANGLE>    MeshTri;
typedef Mesh<QUADRANGLE>  MeshQua;
typedef Mesh<TETRAHEDRON> MeshTet;
typedef Mesh<HEXAHEDRON>  MeshHex;

template<ECellType T>
void checkMesh(Mesh<T> const&m)
{
  typedef Mesh<T> MeshT;
  typedef typename MeshT::VertexH VertexH;
  typedef typename MeshT::CellH CellH;
  typedef typename MeshT::FacetH FacetH;
  typedef typename MeshT::RidgeH RidgeH;

  std::vector<SetVector<CellH> > stars(m.numVerticesTotal());

  // compute the expected star
  for (CellH c = m.cellBegin(), cend = m.cellEnd(); c != cend; ++c)
  {
    if (c.isDisabled(&m))
      continue;
    std::vector<VertexH> vv = c.vertices(&m);
    for (unsigned i = 0; i < vv.size(); ++i)
      stars[vv[i].id(&m)].insert(c);
  }

  // checking the star
  for (VertexH v = m.vertexBegin(), vend = m.vertexEnd(); v!=vend; ++v)
  {
    if (v.isDisabled(&m))
      continue;
    
    std::vector<CellH> exact(stars[v.id(&m)].begin(), stars[v.id(&m)].end());
    EXPECT_EQ(exact, v.star(&m));
    EXPECT_EQ(exact.size(), v.valency(&m));
  }
  
  // check cell adjacencies
  for (CellH c = m.cellBegin(), cend = m.cellEnd(); c != cend; ++c)
  {
    if (c.isDisabled(&m))
      continue;
    
    std::vector<VertexH> vts = c.vertices(&m);
    // check vertices indices
    for (int i = 0; i < MeshT::verts_per_cell; ++i)
      EXPECT_TRUE((vts[i].id(&m) < (index_t)m.numVerticesTotal()) && (vts[i].id(&m) >= 0));
      
    for (unsigned side = 0; side < c.numFacets(&m); ++side)
    {
      FacetH facet = c.facet(&m,side);
      std::pair<CellH, CellH> f_icells = facet.icellAndMate(&m);
      
      CellH adj = c.adjCell(&m, side);
      if (!adj.isValid())
      {
        EXPECT_EQ(1u, facet.valency(&m));
        EXPECT_TRUE(f_icells.first  == c);
        EXPECT_TRUE(f_icells.second == NULL_IDX);
        continue;
      }
        
      int anch;
      std::vector<VertexH> f_vtcs = c.facetVertices(&m, side);
      int other_side = c.adjSideAndAnchor(&m, side, &anch);
      std::vector<VertexH> adj_f_vtcs = adj.facetVertices(&m, abs(other_side));

      if (facet.valency(&m) < 3) // if it is not singular
        EXPECT_EQ(c, adj.adjCell(&m, abs(other_side)) ) << "adj is " << adj;
      
      if (other_side>=0)
        std::reverse(adj_f_vtcs.begin(), adj_f_vtcs.end());
      std::rotate(adj_f_vtcs.begin(), adj_f_vtcs.begin()+anch, adj_f_vtcs.end());
      EXPECT_EQ(f_vtcs, adj_f_vtcs) << "ANCH = " << anch << endl;
      
      if (facet.valency(&m) < 3) // if it is not singular
        EXPECT_TRUE((f_icells.first == c && f_icells.second == adj) || (f_icells.second == c && f_icells.first == adj))
          << f_icells.first<<" "<<f_icells.second<<" "<<c<<" "<<adj;
    }
  }

  // check boundary vtcs
  for (VertexH v = m.vertexBegin(), vend = m.vertexEnd(); v!=vend; ++v)
  {
    std::vector<CellH> star = v.star(&m); 
    
    bool is_interior = 1;
    for (unsigned i = 0; i < star.size(); ++i)
    {
      CellH mates_by_vtx[MeshT::cell_dim]; // number os facets connected to this vertex
      int l = v.localId(&m, star[i]);
      star[i].matesByVtx(&m, l, mates_by_vtx);
      for (int k = 0; k < MeshT::cell_dim; ++k)
        is_interior *= mates_by_vtx[k].isValid();
      if (!is_interior)
        break;
    }
    bool is_boundary = !is_interior;
    EXPECT_EQ(is_boundary, v.isBoundary(&m));
    
  }

  // Check ridge adjacencies
  //  Warning: modify the vector stars
  if (MeshT::cell_dim > 2)
  {
    stars.clear();
    stars.resize(m.numRidgesTotal());
    // compute the expected star
    for (CellH c = m.cellBegin(), cend = m.cellEnd(); c != cend; ++c)
    {
      if (c.isDisabled(&m))
        continue;
      std::vector<VertexH> vv = c.ridges(&m);
      for (unsigned i = 0; i < vv.size(); ++i)
        stars.at(vv[i].id(&m)).insert(c);
    }

    // checking the star
    for (RidgeH v = m.ridgeBegin(), vend = m.ridgeEnd(); v!=vend; ++v)
    {
      if (v.isDisabled(&m))
        continue;
      
      std::vector<CellH> exact(stars.at(v.id(&m)).begin(), stars.at(v.id(&m)).end());
      EXPECT_EQ(exact, v.star(&m));
      EXPECT_EQ(exact.size(), v.valency(&m));
    }
    
  }



}


class TriMesh1Tests : public testing::Test
{
 protected:  // You should make the members protected s.t. they can be
             // accessed from sub-classes.

  typedef MeshTri MeshT;
  typedef typename MeshT::VertexH VertexH;
  typedef typename MeshT::CellH CellH;
  typedef typename MeshT::FacetH FacetH;
  typedef typename MeshT::RidgeH RidgeH;
  typedef MeshIoVtk<MeshT::CellType> MeshWriter;
  
  TriMesh1Tests() : m(dim3) {}
  
  MeshT m;

  static void addTriMesh1(MeshTri &m, bool check = true)
  {
    typedef MeshTri MeshT;
    typedef MeshT::VertexH VertexH;
    typedef MeshT::CellH   CellH;

    int const N = 14;

    VertexH vts[N];
                                    // tag
    vts[ 0] = m.addVertex(Point(0,0,0), 0);
    vts[ 1] = m.addVertex(Point(1,0,0), 1);
    vts[ 2] = m.addVertex(Point(0,1,0), 2);
    vts[ 3] = m.addVertex(Point(1,1,0), 3);
    vts[ 4] = m.addVertex(Point(2,1,0), 4);
    vts[ 5] = m.addVertex(Point(0,2,0), 5);
    vts[ 6] = m.addVertex(Point(1,2,0), 6);
    vts[ 7] = m.addVertex(Point(2,2,0), 7);
    vts[ 8] = m.addVertex(Point(3,2,0), 8);
    vts[ 9] = m.addVertex(Point(0,3,0), 9);
    vts[10] = m.addVertex(Point(1,3,0),10);
    vts[11] = m.addVertex(Point(2,3,0),11);
    vts[12] = m.addVertex(Point(3,3,0),12);
    vts[13] = m.addVertex(Point(1,2,1),13);

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
    
    addTriMesh1(m, true);
    
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
  typedef typename MeshT::VertexH VertexH;
  typedef typename MeshT::CellH CellH;
  typedef typename MeshT::FacetH FacetH;
  typedef typename MeshT::RidgeH RidgeH;
  typedef MeshIoVtk<MeshT::CellType> MeshWriter;
  
  TetMesh1Tests() : m(dim3) {}
  
  MeshT m;

  static void addTetMesh1(MeshTet &m, bool check = true)
  {
    typedef MeshTet MeshT;
    typedef MeshT::VertexH VertexH;
    typedef MeshT::CellH   CellH;

    int const N = 35;

    VertexH vts[N];

    vts[ 0] = m.addVertex(Point(0   ,  0   ,  0   ));
    vts[ 1] = m.addVertex(Point(1   ,  0   ,  0   ));
    vts[ 2] = m.addVertex(Point(0   ,  1   ,  0   ));
    vts[ 3] = m.addVertex(Point(0   ,  0   ,  1   ));
    vts[ 4] = m.addVertex(Point(0.25,  0   ,  0   ));
    vts[ 5] = m.addVertex(Point(0.5 ,  0   ,  0   ));
    vts[ 6] = m.addVertex(Point(0.75,  0   ,  0   ));
    vts[ 7] = m.addVertex(Point(0.75,  0.25,  0   ));
    vts[ 8] = m.addVertex(Point(0.5 ,  0.5 ,  0   ));
    vts[ 9] = m.addVertex(Point(0.25,  0.75,  0   ));
    vts[10] = m.addVertex(Point(0   ,  0.75,  0   ));
    vts[11] = m.addVertex(Point(0   ,  0.5 ,  0   ));
    vts[12] = m.addVertex(Point(0   ,  0.25,  0   ));
    vts[13] = m.addVertex(Point(0   ,  0   ,  0.75));
    vts[14] = m.addVertex(Point(0   ,  0   ,  0.5 ));
    vts[15] = m.addVertex(Point(0   ,  0   ,  0.25));
    vts[16] = m.addVertex(Point(0   ,  0.25,  0.75));
    vts[17] = m.addVertex(Point(0   ,  0.5 ,  0.5 ));
    vts[18] = m.addVertex(Point(0   ,  0.75,  0.25));
    vts[19] = m.addVertex(Point(0.25,  0   ,  0.75));
    vts[20] = m.addVertex(Point(0.5 ,  0   ,  0.5 ));
    vts[21] = m.addVertex(Point(0.75,  0   ,  0.25));
    vts[22] = m.addVertex(Point(0.25,  0.5 ,  0   ));
    vts[23] = m.addVertex(Point(0.5 ,  0.25,  0   ));
    vts[24] = m.addVertex(Point(0.25,  0.25,  0   ));
    vts[25] = m.addVertex(Point(0.25,  0   ,  0.5 ));
    vts[26] = m.addVertex(Point(0.25,  0   ,  0.25));
    vts[27] = m.addVertex(Point(0.5 ,  0   ,  0.25));
    vts[28] = m.addVertex(Point(0   ,  0.5 ,  0.25));
    vts[29] = m.addVertex(Point(0   ,  0.25,  0.5 ));
    vts[30] = m.addVertex(Point(0   ,  0.25,  0.25));
    vts[31] = m.addVertex(Point(0.5 ,  0.25,  0.25));
    vts[32] = m.addVertex(Point(0.25,  0.5 ,  0.25));
    vts[33] = m.addVertex(Point(0.25,  0.25,  0.5 ));
    vts[34] = m.addVertex(Point(0.25,  0.25,  0.25));

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
  typedef typename MeshT::VertexH VertexH;
  typedef typename MeshT::CellH CellH;
  typedef typename MeshT::FacetH FacetH;
  typedef typename MeshT::RidgeH RidgeH;
  typedef MeshIoVtk<MeshT::CellType> MeshWriter;
  
  TetMesh2Tests() : m(dim3) {}
  
  MeshT m;

  static void addTetMesh(MeshTet &m, bool check = true)
  {
    typedef MeshTet MeshT;
    typedef MeshT::VertexH VertexH;
    typedef MeshT::CellH   CellH;

    int const N = 5;

    VertexH vts[N];
                                    // tag
    vts[ 0] = m.addVertex(Point(0,0,0), 0);
    vts[ 1] = m.addVertex(Point(1,0,0), 1);
    vts[ 2] = m.addVertex(Point(0,1,0), 2);
    vts[ 3] = m.addVertex(Point(0,0,1), 3);
    vts[ 4] = m.addVertex(Point(0,-1,0), 4);
    
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
  MeshEdg m0(dim3);
  MeshTri m1(dim3);
  MeshQua m2(dim3);
  MeshTet m3(dim3);
  MeshHex m4(dim3);

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
  MeshT m(dim2);

  EXPECT_EQ(0u, m.numVertices());

  VertexH vts[5];

  for (unsigned i = 0; i < 5; ++i)
  {
    vts[i] = m.addVertex(Point(i,i));
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
  MeshT m(dim2);

  VertexH vts[5];

  for (unsigned i = 0; i < 5; ++i)
  { vts[i] = m.addVertex(Point(i,i)); }

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
  writer.writeVtk(&time);
  
}

TEST_F(TriMesh1Tests, PrintVtkBinSplitEdge)
{
  MeshWriter writer(&m);
  
  writer.setOutputFileName("output/trimesh1.vtk");
  writer.setBinaryOutput(true);
  writer.setFamily(true);
  
  writer.setNamePadding(2);
  
  double time = 1.1;
  writer.splitEdge(2);
  writer.writeVtk(&time);
  
  m.removeCell(CellH(9), true);
  
  time = 1.2;
  writer.splitEdge(3);
  writer.writeVtk(&time);
  
  m.removeCell(CellH(5), true);
  
  time = 1.3;
  writer.splitEdge(4);
  writer.writeVtk(&time);
  
  m.removeCell(CellH(1), true);
  
  time = 1.4;
  writer.splitEdge(5);
  writer.writeVtk(&time);
  
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
  
  writer.setOutputFileName("output/tetmesh1.vtk");
  
  double time = 1.2345;
  writer.writeVtk(&time);  
}

TEST_F(TetMesh1Tests, PrintVtkBinSplitEdge)
{
  MeshWriter writer(&m);
  
  writer.setOutputFileName("output/tetmesh1.vtk");
  writer.setBinaryOutput(true);
  writer.setFamily(true);
  
  writer.setNamePadding(2);
  
  // remove some cells for better visibility
  for (int i = 0; i < 50; ++i)
    m.removeCell(CellH(i), true);
  
  
  double time = 1.1;
  writer.splitEdge(2);
  writer.writeVtk(&time);
  
  //m.removeCell(CellH(9), true);
  //
  time = 1.2;
  writer.splitEdge(3);
  writer.writeVtk(&time);
  //
  //m.removeCell(CellH(5), true);
  //
  time = 1.3;
  writer.splitEdge(4);
  writer.writeVtk(&time);
  //
  //m.removeCell(CellH(1), true);
  //
  time = 1.4;
  writer.splitEdge(5);
  writer.writeVtk(&time);
  //
  //checkMesh(m);
}




