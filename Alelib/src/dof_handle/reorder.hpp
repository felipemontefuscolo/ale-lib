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
#include "../mesh/handler_cell.hpp"
#include "../mesh/handler_facet.hpp"
#include "../mesh/handler_ridge.hpp"
#include "../mesh/handler_vertex.hpp"

namespace alelib
{

namespace internal {
  
template<ECellType CT, class T>
struct RDL_caller {};

template<class T>
struct RDL_caller<TRIANGLE,T>
{
  inline
  void operator() (Mesh<TRIANGLE> const* mp, typename Mesh<TRIANGLE>::CellH c, int n, int ncomps, T * dofs) const
  {
    // n = order
    
    if (n<3)
      return;
    
    int const ndofs = (n+1)*(n+2)/2;
    
    typedef Mesh<TRIANGLE> MeshT;
    typedef typename Mesh<TRIANGLE>::CellH CellH;
    
    for (int i = 0; i < MeshT::facets_per_cell; ++i)
    {
      if (c.facet(mp,i).icellSide0(mp) == c)
        continue;
      int anchor;
      // check orientation
      c.adjSideAndAnchor(mp, i, &anchor);
      if (anchor)
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

  
}

template<ECellType CT, class T>
void reorderDofsLagrange(Mesh<CT> const* mp, typename Mesh<CT>::CellH c, int n, int ncomps, T * dofs)
{
  internal::RDL_caller<CT,T> caller;
  return caller(mp, c, n, ncomps, dofs);
}



} // namespace alelib

#endif




