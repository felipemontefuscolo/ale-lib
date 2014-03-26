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
#include <Alelib/src/shape_functions/parametric_pts.hpp>

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

class ShapeTri1Tests : public testing::Test
{
 protected:  // You should make the members protected s.t. they can be
             // accessed from sub-classes.

  typedef MeshTri MeshT;
  typedef typename MeshT::VertexH VertexH;
  typedef typename MeshT::CellH CellH;
  typedef typename MeshT::FacetH FacetH;
  typedef typename MeshT::RidgeH RidgeH;
  typedef MeshIoVtk<MeshT::CellType> MeshWriter;
  
  ShapeTri1Tests() : m(dim2) {}
  
  MeshT m;

  static void addMesh1(MeshTri &m, int n)
  {
    std::vector<double> xyz;
    std::vector<int> tris;
    
    genTriParametricPts(n, xyz);
    subCellsTet(n, tris);
    
    for (int i = 0; i < (n+1)*(n+2)/2; ++i)
      m.addVertex(Point(xyz.at(2*i + 0),xyz.at(2*i + 1)));
    
    for (int i = 0; i < n*n; ++i)
      m.addCell(listOf(VertexH(tris.at(3*i)), VertexH(tris.at(3*i+1)), VertexH(tris.at(3*i+2))));
  }

  // virtual void SetUp() will be called before each test is run.  You
  // should define it if you need to initialize the varaibles.
  // Otherwise, this can be skipped.
  virtual void SetUp() {
    
  }

  // virtual void TearDown() will be called after each test is run.
  // You should define it if there is cleanup work to do.  Otherwise,
  // you don't have to provide it.
  //
  virtual void TearDown() {
  }




}; 


TEST_F(ShapeTri1Tests, LagrangeTriPou)
{
  
  
  
  
  
}
