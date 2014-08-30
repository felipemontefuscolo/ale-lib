// This file is part of Alelib, a toolbox for finite element codes.
//
// Alelib is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// Alternatively, you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// Alelib is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License and a copy of the GNU General Public License along with
// Alelib. If not, see <http://www.gnu.org/licenses/>.

#ifndef ALE_REORDER_HPP
#define ALE_REORDER_HPP

#include "../mesh/mesh.hpp"
#include "../shape_functions/parametric_pts.hpp"

namespace alelib
{

namespace internal {

static inline
void reorder_aux(std::vector<int>& v, std::vector<int>& order)
{
  std::vector<int> & map = order;

  for (int i=0; i<(int)map.size()-1; ++i)
  {
    while (map[i] != i)
    {
      std::swap(v[map[i]], v[map[map[i]]]);
      std::swap(map[i], map[map[i]]);
    }
  }
  return;
}

template<ECellType CT, typename Mesh_t, class T>
struct RDL_caller {};

template<typename Mesh_t, class T>
struct RDL_caller<TRIANGLE,Mesh_t,T>
{
  inline
  void operator() (Mesh_t const* mp, typename Mesh_t::CellH c, int n, int ncomps, T * dofs) const
  {
    // n = order

    if (n<3)
      return;

    //int const ndofs = (n+1)*(n+2)/2;

    typedef Mesh_t MeshT;
    typedef typename MeshT::CellH CellH;

    for (int i = 0; i < MeshT::facets_per_cell; ++i)
    {
      
      c.facet(mp,i);
      
      
      if (c.facet(mp,i).icellSide0(mp) == c)
        continue;
      int anchor;
      // check orientation
      c.adjSideAndAnchor(mp, i, &anchor);
      if (!anchor)
      {
        int k = ncomps*( MeshT::verts_per_cell + i*(n-1));
        int l = ncomps*( MeshT::verts_per_cell + (i+1)*(n-1) - 1);

        while (l>k)
        {
          for (int m = 0; m < ncomps; ++m)
            std::swap(dofs[k+m], dofs[l+m]);
          k += ncomps;
          l -= ncomps;
        }
      }
    }

  }
};


template<typename Mesh_t, class T>
struct RDL_caller<TETRAHEDRON,Mesh_t,T>
{
inline
  void operator() (Mesh_t const* mp, typename Mesh_t::CellH c, int n, int ncomps, T * dofs) const
  {
    // n = order

    if (n<3)
      return;

    typedef Mesh_t MeshT;
    typedef typename MeshT::CellH CellH;
    typedef typename MeshT::VertexH VertexH;

    // RIDGES FIRST
    for (int i = 0; i < MeshT::ridges_per_cell; ++i)
    {
      if (c.ridge(mp,i).icell(mp) == c)
        continue;

      VertexH ridge_verts_my[2];
      VertexH ridge_verts_other[2];

      c.ridge(mp, i).vertices(mp, ridge_verts_other);
      c.ridgeVertices(mp, i, ridge_verts_my);

      // check orientation
      if (ridge_verts_other[0] != ridge_verts_my[0])
      {
        int k = ncomps*( MeshT::verts_per_cell + i*(n-1));
        int l = ncomps*( MeshT::verts_per_cell + (i+1)*(n-1) - 1);

        while (l>k)
        {
          for (int m = 0; m < ncomps; ++m)
            std::swap(dofs[k+m], dofs[l+m]);
          k += ncomps;
          l -= ncomps;
        }
      }
    }

    if (n<4)
      return;

    // NOW THE FACETS
    std::vector<int> map;
    int const map_size = (n-1)*(n-2)/2;
    for (int i = 0; i < MeshT::facets_per_cell; ++i)
    {
      if (c.facet(mp,i).icellSide0(mp) == c)
        continue;
      int anchor;
      c.adjSideAndAnchor(mp, i, &anchor);
      mapTriInteriorPtsToOpp(n, anchor, map);
      T * v = dofs + ncomps*( 4+6*(n-1)  +  i*map_size ) ;
      // reordering
      for (int k=0; k<map_size; ++k)
      {
        while (map[k] != k)
        {
          int& mmk = map[map[k]];
          for (int j = 0; j < ncomps; ++j)
            std::swap(v[ncomps*map[k]+j], v[ncomps*mmk+j]);
          std::swap(map[k], mmk);
        }
      }
    }

  }
};


}


/// @param mp mesh
/// @param c which cell the dofs correspond
/// @param n the degree of the Lagrange polinomial
/// @param ncomps numeber of componentes per dof
/// @param[out] dofs the dofs to be reordered
template<typename Mesh_t, class T>
void reorderDofsLagrange(Mesh_t const* mp, typename Mesh_t::CellH c, int n, int ncomps, T * dofs)
{
  ALELIB_CHECK(mp != NULL, "invalid mesh pointer", std::invalid_argument);
  internal::RDL_caller<Mesh_t::CellType, Mesh_t,T> caller;
  caller(mp, c, n, ncomps, dofs);
}



} // namespace alelib

#endif




