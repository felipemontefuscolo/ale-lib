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

#ifndef COMMON_TEST_HPP
#define COMMON_TEST_HPP


#include <gtest/gtest.h>
#include <Alelib/Mesh>



#define MAX(a,b) ((a) > (b) ? (a) : (b))

const double pi = 3.14159265359;

using namespace alelib;
using namespace std;

static const int dim1 = 1;
static const int dim2 = 2;
static const int dim3 = 3;


// customizing types
struct MyCellEdg : public Cell<EDGE       > { int mycolor; };
struct MyCellTri : public Cell<TRIANGLE   > { int mycolor; };
struct MyCellQua : public Cell<QUADRANGLE > { int mycolor; };
struct MyCellTet : public Cell<TETRAHEDRON> { int mycolor; };
struct MyCellHex : public Cell<HEXAHEDRON > { int mycolor; };
struct MyVertex : public Vertex { int mycolor; };
struct MyFacet : public Facet { int mycolor;};
struct MyRidge : public Ridge { int mycolor;};

//typedef Cell<EDGE       > MyCellEdg;
//typedef Cell<TRIANGLE   > MyCellTri;
//typedef Cell<QUADRANGLE > MyCellQua;
//typedef Cell<TETRAHEDRON> MyCellTet;
//typedef Cell<HEXAHEDRON > MyCellHex;
//typedef Vertex            MyVertex ;
//typedef Facet             MyFacet  ;
//typedef Ridge             MyRidge  ;

struct TraitsEdg : public DefaultTraits<EDGE       , true, 3> { typedef MyCellEdg CellT; typedef MyVertex VertexT; typedef MyFacet FacetT; typedef MyRidge RidgeT;};
struct TraitsTri : public DefaultTraits<TRIANGLE   , true, 3> { typedef MyCellTri CellT; typedef MyVertex VertexT; typedef MyFacet FacetT; typedef MyRidge RidgeT;};
struct TraitsQua : public DefaultTraits<QUADRANGLE , true, 3> { typedef MyCellQua CellT; typedef MyVertex VertexT; typedef MyFacet FacetT; typedef MyRidge RidgeT;};
struct TraitsTet : public DefaultTraits<TETRAHEDRON, true, 3> { typedef MyCellTet CellT; typedef MyVertex VertexT; typedef MyFacet FacetT; typedef MyRidge RidgeT;};
struct TraitsHex : public DefaultTraits<HEXAHEDRON , true, 3> { typedef MyCellHex CellT; typedef MyVertex VertexT; typedef MyFacet FacetT; typedef MyRidge RidgeT;};


struct TraitsEdgNoC : public DefaultTraits<EDGE       , false, 1> { typedef MyCellEdg CellT; typedef MyVertex VertexT; typedef MyFacet FacetT; typedef MyRidge RidgeT;};
struct TraitsTriNoC : public DefaultTraits<TRIANGLE   , false, 2> { typedef MyCellTri CellT; typedef MyVertex VertexT; typedef MyFacet FacetT; typedef MyRidge RidgeT;};
struct TraitsQuaNoC : public DefaultTraits<QUADRANGLE , false, 2> { typedef MyCellQua CellT; typedef MyVertex VertexT; typedef MyFacet FacetT; typedef MyRidge RidgeT;};
struct TraitsTetNoC : public DefaultTraits<TETRAHEDRON, false, 3> { typedef MyCellTet CellT; typedef MyVertex VertexT; typedef MyFacet FacetT; typedef MyRidge RidgeT;};
struct TraitsHexNoC : public DefaultTraits<HEXAHEDRON , false, 3> { typedef MyCellHex CellT; typedef MyVertex VertexT; typedef MyFacet FacetT; typedef MyRidge RidgeT;};


typedef Mesh<TraitsEdg> MeshEdg;
typedef Mesh<TraitsTri> MeshTri;
typedef Mesh<TraitsQua> MeshQua;
typedef Mesh<TraitsTet> MeshTet;
typedef Mesh<TraitsHex> MeshHex;

typedef Mesh<TraitsEdgNoC> MeshEdgNoC;
typedef Mesh<TraitsTriNoC> MeshTriNoC;
typedef Mesh<TraitsQuaNoC> MeshQuaNoC;
typedef Mesh<TraitsTetNoC> MeshTetNoC;
typedef Mesh<TraitsHexNoC> MeshHexNoC;


template<typename Mesh_t>
void checkMesh(Mesh_t const&m)
{
  typedef Mesh_t MeshT;
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
      if (adj.isNull())
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
        is_interior *= !mates_by_vtx[k].isNull();
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




















#endif // COMMON_TEST_HPP


