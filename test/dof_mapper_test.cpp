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
#include <Alelib/DofMapper>
#include <Alelib/Mesh>
#include <Alelib/IO>
#include <Alelib/ShapeFunction>
#include <Alelib/Quadrature>
#include <Alelib/src/shape_functions/parametric_pts.hpp>
#include <tr1/array>
#include <tr1/tuple>
#include <algorithm>
#include <tr1/memory>
#include <limits> // for std::numeric_limits<Real>::epsilon()
#include <tr1/functional>
//#include <functional>
#include <tr1/array>
#include <tr1/tuple>
#include <cmath>

using namespace std::tr1::placeholders;


using namespace alelib;
using namespace std;

#define MAX(a,b) ((a) > (b) ? (a) : (b))

//Real const ALE_EPS = std::numeric_limits<Real>::epsilon();
//Real const ALE_TOL = 500000*ALE_EPS; // ~ 1.1e-10 para double

static const int dim1 = 1;
static const int dim2 = 2;
static const int dim3 = 3;

struct TraitsEdg { static const ECellType CType = EDGE        ; };
struct TraitsTri { static const ECellType CType = TRIANGLE    ; };
struct TraitsQua { static const ECellType CType = QUADRANGLE  ; };
struct TraitsTet { static const ECellType CType = TETRAHEDRON ; };
struct TraitsHex { static const ECellType CType = HEXAHEDRON  ; };

typedef Mesh<TraitsEdg> MeshEdg;
typedef Mesh<TraitsTri> MeshTri;
typedef Mesh<TraitsQua> MeshQua;
typedef Mesh<TraitsTet> MeshTet;
typedef Mesh<TraitsHex> MeshHex;

typedef MeshIoMsh<MeshEdg> IoMshEdg;
typedef MeshIoMsh<MeshTri> IoMshTri;
typedef MeshIoMsh<MeshQua> IoMshQua;
typedef MeshIoMsh<MeshTet> IoMshTet;
typedef MeshIoMsh<MeshHex> IoMshHex;

typedef DofMapper<MeshEdg> DofMapEdg;
typedef DofMapper<MeshTri> DofMapTri;
typedef DofMapper<MeshQua> DofMapQua;
typedef DofMapper<MeshTet> DofMapTet;
typedef DofMapper<MeshHex> DofMapHex;

template<class MeshT, class DofMapperT>
void getAllDofs(std::vector<index_t> & dat, DofMapperT const& mapper, MeshT const* mp)
{
  typedef typename MeshT::VertexH VertexH;
  typedef typename MeshT::CellH CellH;
  typedef typename MeshT::FacetH FacetH;
  typedef typename MeshT::RidgeH RidgeH;  
  
  index_t dofs[20];

  dat.clear();

  // vtxs
  for (VertexH vtx = mp->vertexBegin(), vtx_end = mp->vertexEnd(); vtx != vtx_end; ++vtx)
  {
    if ( vtx.isDisabled(mp))
      continue;

    for (int i = 0; i < mapper.numVars(); ++i)
    {
      mapper.variable(i).getVertexDofs(dofs, vtx);
      for (int j = 0; j < mapper.variable(i).numDofsPerVertex(); ++j)
        dat.push_back(dofs[j]);
    }
  }
  // cells
  for (CellH cell = mp->cellBegin(), c_end = mp->cellEnd(); cell != c_end; ++cell)
  {
    if (cell.isDisabled(mp))
      continue;

    for (int i = 0; i < mapper.numVars(); ++i)
    {
      mapper.variable(i).getCellDofs(dofs, cell);
      for (int j = 0; j < mapper.variable(i).numDofsPerCell(); ++j)
        dat.push_back(dofs[j]);
    }
  }
  // facets
  for (FacetH facet = mp->facetBegin(), f_end = mp->facetEnd(); facet != f_end; ++facet)
  {
    if (facet.isDisabled(mp))
      continue;

    for (int i = 0; i < mapper.numVars(); ++i)
    {
      mapper.variable(i).getFacetDofs(dofs, facet);
      for (int j = 0; j < mapper.variable(i).numDofsPerFacet(); ++j)
        dat.push_back(dofs[j]);
    }
  }
  // ridges
  if (MeshT::cell_dim == 3)
    for (RidgeH ridge = mp->ridgeBegin(), r_end = mp->ridgeEnd(); ridge != r_end; ++ridge)
    {
      if (ridge.isDisabled(mp))
        continue;

      for (int i = 0; i < mapper.numVars(); ++i)
      {
        mapper.variable(i).getRidgeDofs(dofs, ridge);
        for (int j = 0; j < mapper.variable(i).numDofsPerRidge(); ++j)
          dat.push_back(dofs[j]);
      }
    }
}




TEST(DoffMapper, AssignsDofsTri)
{
  typedef MeshTri MeshT;
  typedef MeshT::VertexH VertexH;
  typedef MeshT::CellH CellH;
  typedef MeshT::FacetH FacetH;
  typedef MeshT::RidgeH RidgeH;

  MeshTri m;
  IoMshTri io;

  const char* mesh_in  = "meshes/simptri3.msh";

  io.readFile(mesh_in, &m);

  DofMapTri mapper(&m);

  //                         ndpv,  ndpr,  ndpf,  ndpc
  mapper.addVariable("altura",    1,     0,     0,     0); // 12
  mapper.addVariable("vetor",     2,     0,     0,     0); // 24
  mapper.addVariable("foo",       0,     1,     0,     0); // 0
  mapper.addVariable("bar",       0,     0,     1,     0); // 25
  mapper.addVariable("moo",       0,     0,     0,     1); // 14

  mapper.SetUp();

  EXPECT_EQ(75, mapper.numDofs());

  // checking variable 1
  index_t dofs[] = {-2,-2,-2,-2,-2,-2,-2,-2};
  index_t * dofs_end = dofs+sizeof(dofs)/sizeof(index_t);
  index_t nn;
  for (VertexH l = m.vertexBegin(), l_end = m.vertexEnd(); l<l_end; ++l)
  {
    mapper.variable(1).getVertexDofs(dofs, l);
    nn = count_if(dofs, dofs_end, bind2nd(greater_equal<int>(),0) ); // non-negative
    EXPECT_EQ(2, nn);
    fill(dofs, dofs_end, -1);
  }

  m.removeCell(CellH(2), true); // true = remove unref vertices
  m.removeCell(CellH(3), true); // true = remove unref vertices

  mapper.SetUp();

  //11x3 + 22 + 12
  EXPECT_EQ(11, mapper.variable(0).numPositiveDofs());
  EXPECT_EQ(22, mapper.variable(1).numPositiveDofs());
  EXPECT_EQ( 0, mapper.variable(2).numPositiveDofs());
  EXPECT_EQ(22, mapper.variable(3).numPositiveDofs());
  EXPECT_EQ(12, mapper.variable(4).numPositiveDofs());

  EXPECT_EQ(67, mapper.numDofs());


  // ckeck
  std::vector<index_t> dat;

  EXPECT_EQ(5, mapper.numVars());

  getAllDofs(dat,mapper,&m);

  std::sort(dat.begin(), dat.end());

  EXPECT_TRUE(dat[0] >= 0); // grau de liberdade definidos em todo lugar

  dat.erase(std::remove(dat.begin(), dat.end(), -1), dat.end()); // remove -1
  dat.erase(std::unique(dat.begin(), dat.end()), dat.end());     // remove duplicated

  EXPECT_EQ((index_t)mapper.numDofs(), (index_t)dat.size());





}

TEST(DoffMapper, AssignsDofsTriTags)
{
  typedef MeshTri MeshT;
  typedef MeshT::VertexH VertexH;
  typedef MeshT::CellH CellH;
  typedef MeshT::FacetH FacetH;
  typedef MeshT::RidgeH RidgeH;

  MeshTri m;
  IoMshTri io;

  const char* mesh_in  = "meshes/simptri3.msh";

  io.readFile(mesh_in, &m);

  int n_regions = 1;
  int ntags=1;
  int tags=1;

  DofMapTri mapper(&m);
  //                         ndpv,  ndpr,  ndpf,  ndpc
  mapper.addVariable("altura",    1,     0,     0,     0, n_regions, &ntags, &tags); // 4
  mapper.addVariable("vetor",     2,     0,     0,     0, n_regions, &ntags, &tags); // 8
  mapper.addVariable("foo",       0,     1,     0,     0, n_regions, &ntags, &tags); // 0
  mapper.addVariable("bar",       0,     0,     1,     0, n_regions, &ntags, &tags); // 4
  mapper.addVariable("moo",       0,     0,     0,     1, n_regions, &ntags, &tags); // 0
  
  
  mapper.SetUp();
  
  EXPECT_EQ(16, mapper.numDofs());
  
  
  m.removeCell(CellH(2), true);
  m.removeCell(CellH(3), true);
  
  mapper.SetUp();
  
  // 15
  EXPECT_EQ( 3, mapper.variable(0).numPositiveDofs());
  EXPECT_EQ( 6, mapper.variable(1).numPositiveDofs());
  EXPECT_EQ( 0, mapper.variable(2).numPositiveDofs());
  EXPECT_EQ( 3, mapper.variable(3).numPositiveDofs());
  EXPECT_EQ( 0, mapper.variable(4).numPositiveDofs());

  EXPECT_EQ(12, mapper.numDofs());
  
  
  // checking dofs
  std::vector<index_t> dat;
  
  EXPECT_EQ(5, mapper.numVars());
  
  getAllDofs(dat,mapper,&m);

  std::sort(dat.begin(), dat.end());

  //EXPECT_EQ(-1,dat[0]); // tags

  dat.erase(std::remove(dat.begin(), dat.end(), -1), dat.end()); // remove -1
  dat.erase(std::unique(dat.begin(), dat.end()), dat.end());     // remove duplicated
  
  EXPECT_EQ(mapper.numDofs(), (int)dat.size());
  
  index_t counter = 0;
  for (index_t i = 0; i < (index_t)dat.size(); ++i)
  {
    EXPECT_EQ(counter, dat[i]);
    ++counter;
  }  
  
}


TEST(DoffMapper, AssignsDofsTet)
{
  typedef MeshTet MeshT;
  typedef MeshT::VertexH VertexH;
  typedef MeshT::CellH CellH;
  typedef MeshT::FacetH FacetH;
  typedef MeshT::RidgeH RidgeH;

  MeshTet m;
  IoMshTet io;
  
  const char* mesh_in  = "meshes/simptet4.msh";

  io.readFile(mesh_in, &m);

  DofMapTet mapper(&m);
  //                         ndpv,  ndpr,  ndpf,  ndpc
  mapper.addVariable("numero",    1,     0,     0,     0); //  9
  mapper.addVariable("foo",       0,     1,     0,     0); // 26
  mapper.addVariable("bar",       0,     0,     1,     0); // 30
  mapper.addVariable("moo",       0,     0,     0,     2); // 24
 
  mapper.SetUp();
 
  EXPECT_EQ( 9, mapper.variable(0).numPositiveDofs());
  EXPECT_EQ(26, mapper.variable(1).numPositiveDofs());
  EXPECT_EQ(30, mapper.variable(2).numPositiveDofs());
  EXPECT_EQ(24, mapper.variable(3).numPositiveDofs());
 
  EXPECT_EQ(89, mapper.numDofs());
 
  //mapper.SetUp();
  //
  m.removeCell(CellH(2), true);
  m.removeCell(CellH(3), true);
  //
  mapper.SetUp();
  //
 
  EXPECT_EQ( 9, mapper.variable(0).numPositiveDofs());
  EXPECT_EQ(26, mapper.variable(1).numPositiveDofs());
  EXPECT_EQ(28, mapper.variable(2).numPositiveDofs());
  EXPECT_EQ(20, mapper.variable(3).numPositiveDofs());
  ////
  EXPECT_EQ(83, mapper.numDofs());



  // checking dofs
  std::vector<index_t> dat;
 
  EXPECT_EQ(4, mapper.numVars());
 
  getAllDofs(dat,mapper,&m);

  std::sort(dat.begin(), dat.end());

  dat.erase(std::remove(dat.begin(), dat.end(), -1), dat.end()); // remove -1
  dat.erase(std::unique(dat.begin(), dat.end()), dat.end());     // remove duplicated
 
  EXPECT_EQ((index_t)mapper.numDofs(), (index_t)dat.size());
 
  index_t counter = 0;
  for (index_t i = 0; i < (index_t)dat.size(); ++i)
  {
    EXPECT_EQ(counter, dat[i]);
    ++counter;
  }
 
}


TEST(DoffMapper, AssignsDofsTetTags)
{
  typedef MeshTet MeshT;
  typedef MeshT::VertexH VertexH;
  typedef MeshT::CellH CellH;
  typedef MeshT::FacetH FacetH;
  typedef MeshT::RidgeH RidgeH;

  MeshTet m;
  IoMshTet io;
  
  const char* mesh_in  = "meshes/simptet4.msh";

  io.readFile(mesh_in, &m);

  int const n_regions = 0; // when n_regions=0, the variable is defined everywhere

  DofMapTet mapper(&m);
  //                         ndpv,  ndpr,  ndpf,  ndpc
  mapper.addVariable("numero",    1,     0,     0,     0, n_regions, (int*)NULL, (int*)NULL); //  9
  mapper.addVariable("foo",       0,     1,     0,     0, n_regions, (int*)NULL, (int*)NULL); // 26
  mapper.addVariable("bar",       0,     0,     1,     0, n_regions, (int*)NULL, (int*)NULL); // 30
  mapper.addVariable("moo",       0,     0,     0,     2, n_regions, (int*)NULL, (int*)NULL); // 24
 
  mapper.SetUp();
 
  EXPECT_EQ( 9, mapper.variable(0).numPositiveDofs());
  EXPECT_EQ(26, mapper.variable(1).numPositiveDofs());
  EXPECT_EQ(30, mapper.variable(2).numPositiveDofs());
  EXPECT_EQ(24, mapper.variable(3).numPositiveDofs());
 
  EXPECT_EQ(89, mapper.numDofs());
 
  //mapper.SetUp();
  //
  m.removeCell(CellH(2), true);
  m.removeCell(CellH(3), true);
  //
  mapper.SetUp();
  //
 
  EXPECT_EQ( 9, mapper.variable(0).numPositiveDofs());
  EXPECT_EQ(26, mapper.variable(1).numPositiveDofs());
  EXPECT_EQ(28, mapper.variable(2).numPositiveDofs());
  EXPECT_EQ(20, mapper.variable(3).numPositiveDofs());
  ////
  EXPECT_EQ(83, mapper.numDofs());



  // checking dofs
  std::vector<index_t> dat;
 
  EXPECT_EQ(4, mapper.numVars());
 
  getAllDofs(dat,mapper,&m);

  std::sort(dat.begin(), dat.end());

  dat.erase(std::remove(dat.begin(), dat.end(), -1), dat.end()); // remove -1
  dat.erase(std::unique(dat.begin(), dat.end()), dat.end());     // remove duplicated
 
  EXPECT_EQ((index_t)mapper.numDofs(), (index_t)dat.size());
 
  index_t counter = 0;
  for (index_t i = 0; i < (index_t)dat.size(); ++i)
  {
    EXPECT_EQ(counter, dat[i]);
    ++counter;
  }
 
}




TEST(DoffMapper, TagsLinkTri3)
{
  typedef MeshTri MeshT;
  typedef MeshT::VertexH VertexH;
  typedef MeshT::CellH CellH;
  typedef MeshT::FacetH FacetH;
  typedef MeshT::RidgeH RidgeH;

  MeshTri m;
  IoMshTri io;

  const char* mesh_in  = "meshes/simptri3.msh";

  io.readFile(mesh_in, &m);

  int n_regions = 1;
  int ntags=1;
  int tags=1;

  DofMapTri mapper(&m);
  //                         ndpv,  ndpr,  ndpf,  ndpc
  mapper.addVariable("altura",    1,     0,     0,     0, n_regions, &ntags, &tags);
  mapper.addVariable("vetor",     2,     0,     0,     0, n_regions, &ntags, &tags);
  
  
  mapper.SetUp();
  
  EXPECT_EQ(12, mapper.numDofs());

  // .variable(0)
  //int *dat = mapper.data();

#define PRINT_DAT                            \
  for (int i = 0; i < mapper.totalSize(); ++i) \
  {                                          \
    std::cout.width (3);                     \
    std::cout << dat[i] << "\t";             \
    if (i==11)                               \
      std::cout <<"|\t";                     \
  }                                          \
  std::cout << std::endl;


  //PRINT_DAT
  
  index_t dofs1[3] = {-1,-1,-1};
  index_t dofs2[3] = {-1,-1,-1};
  
  mapper.variable(0).getVertexDofs(dofs1,   VertexH(7));
  mapper.variable(1).getVertexDofs(dofs1+1, VertexH(7));

  mapper.variable(0).getVertexDofs(dofs2,   VertexH(0));
  mapper.variable(1).getVertexDofs(dofs2+1, VertexH(0));

  mapper.linkDofs(3, dofs1, dofs2);

  EXPECT_EQ(9, mapper.numDofs()) << dofs1[0] << " "<< dofs1[1] << " "<< dofs1[2] << "\n"<< dofs2[0] << " "<< dofs2[1] << " "<< dofs2[2] << " ";


  // ckeck
  std::vector<index_t> dat;
  
  EXPECT_EQ(2, mapper.numVars());
  
  getAllDofs(dat,mapper,&m);

  std::sort(dat.begin(), dat.end());

  //EXPECT_TRUE(dat[0] == -1); // grau de liberdade nao definidos em todo lugar

  dat.erase(std::remove(dat.begin(), dat.end(), -1), dat.end()); // remove -1
  dat.erase(std::unique(dat.begin(), dat.end()), dat.end());     // remove duplicated
  
  EXPECT_EQ(mapper.numDofs(), (int)dat.size());
  
  index_t counter = 0;
  for (index_t i = 0; i < (index_t)dat.size(); ++i)
  {
    EXPECT_EQ(counter, dat[i]);
    ++counter;
  }
  
  //PRINT_DAT
  
}

TEST(DoffMapper, TagsLink2Tri3)
{
  typedef MeshTri MeshT;
  typedef MeshT::VertexH VertexH;
  typedef MeshT::CellH CellH;
  typedef MeshT::FacetH FacetH;
  typedef MeshT::RidgeH RidgeH;

  MeshTri m;
  IoMshTri io;

  const char* mesh_in  = "meshes/simptri3.msh";

  io.readFile(mesh_in, &m);

  DofMapTri mapper(&m);
  //                         ndpv,  ndpr,  ndpf,  ndpc
  mapper.addVariable("numero",    1,     0,     0,     0); // 12
  mapper.addVariable("vetor",     2,     0,     0,     0); // 24
  mapper.addVariable("foo",       0,     1,     0,     0); //  0
  mapper.addVariable("bar",       0,     0,     1,     0); // 25
  mapper.addVariable("moo",       0,     0,     0,     1); // 14
  
  mapper.SetUp();
  
  EXPECT_EQ(75, mapper.numDofs());
  
  m.removeCell(CellH(2), true);
  m.removeCell(CellH(3), true);
  
  mapper.SetUp();
  
  //11x3 + 22 + 12 + 12
  EXPECT_EQ(11, mapper.variable(0).numPositiveDofs());
  EXPECT_EQ(22, mapper.variable(1).numPositiveDofs());
  EXPECT_EQ( 0, mapper.variable(2).numPositiveDofs());
  EXPECT_EQ(22, mapper.variable(3).numPositiveDofs());
  EXPECT_EQ(12, mapper.variable(4).numPositiveDofs());

  EXPECT_EQ(67, mapper.numDofs());
  

  // .variable(0)
  //int *dat = mapper.data();

  int dofs1[] = {-1,-1,-1,-1};
  int dofs2[] = {-1,-1,-1,-1};
  
  mapper.variable(0).getVertexDofs(dofs1,   VertexH(4));
  mapper.variable(1).getVertexDofs(dofs1+1, VertexH(4));

  mapper.variable(0).getVertexDofs(dofs2,   VertexH(3));
  mapper.variable(1).getVertexDofs(dofs2+1, VertexH(3));

  mapper.variable(4).getDofsInCell(dofs1+3, CellH(10));
  mapper.variable(4).getDofsInCell(dofs2+3, CellH(11));

  mapper.linkDofs(4, dofs1, dofs2);

  EXPECT_EQ(63, mapper.numDofs()) << dofs1[0] << " "<< dofs1[1] << " "<< dofs1[2] << " "<< dofs1[3] << "\n"<< dofs2[0] << " "<< dofs2[1] << " "<< dofs2[2] << " "<< dofs2[3] << " ";


  // ckeck
  std::vector<index_t> dat;
  
  EXPECT_EQ(5, mapper.numVars());
  
  getAllDofs(dat,mapper,&m);

  std::sort(dat.begin(), dat.end());

  //EXPECT_TRUE(dat[0] == -1);

  dat.erase(std::remove(dat.begin(), dat.end(), -1), dat.end()); // remove -1
  dat.erase(std::unique(dat.begin(), dat.end()), dat.end());     // remove duplicated
  
  EXPECT_EQ((index_t)mapper.numDofs(), (index_t)dat.size());
  
  index_t counter = 0;
  for (index_t i = 0; i < (index_t)dat.size(); ++i)
  {
    EXPECT_EQ(counter, dat[i]);
    ++counter;
  }
  
  //PRINT_DAT
  
}


TEST(DoffMapper, TagsLinkTri3DoNothing)
{
  typedef MeshTri MeshT;
  typedef MeshT::VertexH VertexH;
  typedef MeshT::CellH CellH;
  typedef MeshT::FacetH FacetH;
  typedef MeshT::RidgeH RidgeH;

  MeshTri m;
  IoMshTri io;

  const char* mesh_in  = "meshes/simptri3.msh";

  io.readFile(mesh_in, &m);

  int n_regions = 1;
  int ntags=1;
  int tags=1;

  DofMapTri mapper(&m);
  //                         ndpv,  ndpr,  ndpf,  ndpc
  mapper.addVariable("altura",    1,     0,     0,     0, n_regions, &ntags, &tags);
  mapper.addVariable("vetor",     2,     0,     0,     0, n_regions, &ntags, &tags);
  
  
  mapper.SetUp();
  
  EXPECT_EQ(12, mapper.numDofs());

  // .variable(0)
  //int *dat = mapper.data();

#define PRINT_DAT                            \
  for (int i = 0; i < mapper.totalSize(); ++i) \
  {                                          \
    std::cout.width (3);                     \
    std::cout << dat[i] << "\t";             \
    if (i==11)                               \
      std::cout <<"|\t";                     \
  }                                          \
  std::cout << std::endl;


  //PRINT_DAT
  
  index_t dofs1[3] = {-1,-1,-1};
  index_t dofs2[3] = {-1,-1,-1};
  
  //mapper.variable(0).getVertexDofs(dofs1,   VertexH(7));
  //mapper.variable(1).getVertexDofs(dofs1+1, VertexH(7));
  //
  //mapper.variable(0).getVertexDofs(dofs2,   VertexH(0));
  //mapper.variable(1).getVertexDofs(dofs2+1, VertexH(0));

  mapper.linkDofs(3, dofs1, dofs2);

  EXPECT_EQ(12, mapper.numDofs()) << dofs1[0] << " "<< dofs1[1] << " "<< dofs1[2] << "\n"<< dofs2[0] << " "<< dofs2[1] << " "<< dofs2[2] << " ";


  // ckeck
  std::vector<index_t> dat;
  
  EXPECT_EQ(2, mapper.numVars());
  
  getAllDofs(dat,mapper,&m);

  std::sort(dat.begin(), dat.end());

  //EXPECT_TRUE(dat[0] == -1); // grau de liberdade nao definidos em todo lugar

  dat.erase(std::remove(dat.begin(), dat.end(), -1), dat.end()); // remove -1
  dat.erase(std::unique(dat.begin(), dat.end()), dat.end());     // remove duplicated
  
  EXPECT_EQ(mapper.numDofs(), (int)dat.size());
  
  index_t counter = 0;
  for (index_t i = 0; i < (index_t)dat.size(); ++i)
  {
    EXPECT_EQ(counter, dat[i]);
    ++counter;
  }
  
  //PRINT_DAT
  
}

TEST(DoffMapper, RegionSplitting)
{
  typedef MeshTri MeshT;
  typedef MeshT::VertexH VertexH;
  typedef MeshT::CellH CellH;
  typedef MeshT::FacetH FacetH;
  typedef MeshT::RidgeH RidgeH;

  MeshTri m;
  IoMshTri io;

  const char* mesh_in  = "meshes/1level_tri3.msh";

  io.readFile(mesh_in, &m);

  int n_regions = 2;
  int ntags[] = {3, 3};
  int tags[] = {71,72,74,  70,72,73};

  DofMapTri mapper(&m);
  //                         ndpv,  ndpr,  ndpf,  ndpc
  mapper.addVariable("altura",    1,     0,     0,     0);                         // 16
  mapper.addVariable("vetor",     0,     0,     1,     0, n_regions, ntags, tags); // 33 -> 36
  mapper.addVariable("coisa",     0,     0,     0,     1, n_regions, ntags, tags); // 18
  
  //mapper.SetUp();
  //
  //EXPECT_EQ(67, mapper.numDofs());

  mapper.SetUp();

  EXPECT_EQ(70, mapper.numDofs());

  EXPECT_EQ(1, mapper.variable(0).numRegions());
  EXPECT_EQ(2, mapper.variable(1).numRegions());
  EXPECT_EQ(2, mapper.variable(2).numRegions());

  // .getVariable(0)
  //int *dat = mapper.data();

  //PRINT_DAT


  //// ckeck
  //std::vector<index_t> dat;
  //
  //EXPECT_EQ(3, mapper.numVars());
  //
  //getAllDofs(dat,mapper,&m);
  //
  //std::sort(dat.begin(), dat.end());
  //
  ////EXPECT_TRUE(dat[0] == 0);
  //
  //dat.erase(std::remove(dat.begin(), dat.end(), -1), dat.end()); // remove -1
  //dat.erase(std::unique(dat.begin(), dat.end()), dat.end());     // remove duplicated
  //
  //EXPECT_EQ((index_t)mapper.numDofs(), (index_t)dat.size());
  //
  //index_t counter = 0;
  //for (index_t i = 0; i < (index_t)dat.size(); ++i)
  //{
  //  EXPECT_EQ(counter, dat[i]);
  //  ++counter;
  //}
  //
  ////PRINT_DAT
  
}



