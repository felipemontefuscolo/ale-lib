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
#include <tr1/array>
#include <tr1/tuple>
#include <algorithm>
#include <tr1/memory>

using namespace alelib;
using namespace std;

const int dim1 = 1;
const int dim2 = 2;
const int dim3 = 3;

typedef Mesh<EDGE>        MeshE0;
typedef Mesh<TRIANGLE>    MeshE1;
typedef Mesh<QUADRANGLE>  MeshE2;
typedef Mesh<TETRAHEDRON> MeshE3;
typedef Mesh<HEXAHEDRON>  MeshE4;

TEST(MeshTest, Initialize2d)
{
  MeshE0 m0(dim3);
  MeshE1 m1(dim3);
  MeshE2 m2(dim3);
  MeshE3 m3(dim3);
  MeshE4 m4(dim3);

  //MeshE0::printElementsSize();
  //MeshE1::printElementsSize();
  //MeshE2::printElementsSize();
  MeshE3::printElementsSize();
  //MeshE4::printElementsSize();
}

TEST(MeshTest, AddVertex)
{
  typedef MeshE1 MeshT;
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
  typedef MeshE1 MeshT;
  typedef MeshT::VertexH VertexH;
  MeshT m(dim2);

  VertexH vts[5];

  for (unsigned i = 0; i < 5; ++i)
  { vts[i] = m.addVertex(Point(i,i)); }

  m.removeUnrefVertex(vts[2]);
  EXPECT_EQ(index_t(4), m.numVertices());
  m.removeUnrefVertex(vts[0]);
  EXPECT_EQ(index_t(3), m.numVertices());
  m.removeUnrefVertex(vts[4]);
  EXPECT_EQ(index_t(2), m.numVertices());

  EXPECT_EQ(index_t(5), m.numVerticesTotal());

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
      
    for (unsigned side = 0; side < c.numFacets(&m); ++side)
    {
      FacetH facet = c.facet(&m,side);
      
      CellH adj = c.adjCell(&m, side);
      if (!adj.isValid())
      {
        EXPECT_EQ(1u, facet.valency(&m));
        continue;
      }
        
      int anch;
      std::vector<VertexH> f_vtcs = c.facetVertices(&m, side);
      int other_side = c.adjSideAndAnchor(&m, side, &anch);
      std::vector<VertexH> adj_f_vtcs = adj.facetVertices(&m, abs(other_side));

      if (facet.valency(&m) < 3)
        EXPECT_EQ(c, adj.adjCell(&m, abs(other_side)) );
      
      if (other_side>=0)
        std::reverse(adj_f_vtcs.begin(), adj_f_vtcs.end());
      std::rotate(adj_f_vtcs.begin(), adj_f_vtcs.begin()+anch, adj_f_vtcs.end());
      EXPECT_EQ(f_vtcs, adj_f_vtcs) << "ANCH = " << anch << endl;
      
    }
  }  


}

void addTriMesh1(MeshE1 &m, bool check = true)
{
  typedef MeshE1 MeshT;
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

  int const E = 10;

  CellH cells[E];

  cells[0] = m.addCell(listOf(vts[ 0], vts[ 1], vts[ 2]));
  cells[1] = m.addCell(listOf(vts[ 1], vts[ 3], vts[ 4]));
  cells[2] = m.addCell(listOf(vts[ 2], vts[ 6], vts[ 5]));
  cells[3] = m.addCell(listOf(vts[ 3], vts[ 4], vts[ 7]));
  cells[4] = m.addCell(listOf(vts[ 5], vts[10], vts[ 9]));
  cells[5] = m.addCell(listOf(vts[ 5], vts[ 6], vts[10]));
  cells[6] = m.addCell(listOf(vts[ 6], vts[11], vts[10]));
  cells[7] = m.addCell(listOf(vts[ 6], vts[ 7], vts[11]));
  cells[8] = m.addCell(listOf(vts[ 7], vts[ 8], vts[12]));
  cells[9] = m.addCell(listOf(vts[13], vts[ 6], vts[10]));

  if (!check)
    return;

  checkMesh(m);

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


}

TEST(MeshTest, AddCellTri)
{
  typedef MeshE1 MeshT;
  typedef MeshT::VertexH VertexH;
  MeshT m(dim3);

  addTriMesh1(m, true);
}












