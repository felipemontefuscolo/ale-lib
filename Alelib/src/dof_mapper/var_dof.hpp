// This file is part of FEPiC++, a toolbox for finite element codes.
//
// FEPiC++ is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// Alternatively, you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// FEPiC++ is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License and a copy of the GNU General Public License along with
// FEPiC++. If not, see <http://www.gnu.org/licenses/>.

#ifndef ALELIB_VAR_DOF_HPP
#define ALELIB_VAR_DOF_HPP

#include "Array/array.hpp"
#include <vector>
#include <string>
#include "contrib/Loki/set_vector.hpp"

namespace alelib
{

template<class Mesh_t> class DofMapper;

template<class Mesh_t>
class VarDofs
{
  friend
  class DofMapper<Mesh_t>;
  
  typedef marray::Array<index_t, 3> Container; // (region(defined by tags), obj_id(vts, cells, etc.), component)

public:
  typedef Mesh_t MeshT;
  static const int cell_dim = MeshT::cell_dim;

protected:
  std::string    m_name;
  int            m_n_dofs_in_vtx;
  int            m_n_dofs_in_ridge;
  int            m_n_dofs_in_facet;
  int            m_n_dofs_in_cell;
  int            m_n_positive_dofs;    // It includes linked dofs
  float          m_grow_factor;        // for vector reserve

  std::vector<SetVector<int> > m_regions_tags; // Each region is defined by a set of tags.
                                               // If m_regional_tags[0] is empty then all tags are considered.

                              // ordering of c. below
  Container m_verts_dofs;     // 0
  Container m_ridges_dofs;    // 1
  Container m_facets_dofs;    // 2
  Container m_cells_dofs;     // 3

  void setUp(int first_dof_id);

  void linkDofs(int size, int const* dofs1, int const* dofs2); // do dofs2 = dofs1

public:

  //
  // The region i has n_tags_per_region[i] tags.
  //
  //explicit
  template<class Iterator1, class Iterator2>
  VarDofs(const char* name, int ndpv, int ndpr, int ndpf, int ndpc,
                            int n_regions, Iterator1 n_tags_per_region, Iterator2 regions_tags)
                            : m_name(name),
                              m_n_dofs_in_vtx  (ndpv),
                              m_n_dofs_in_ridge(ndpr),
                              m_n_dofs_in_facet(ndpf),
                              m_n_dofs_in_cell (ndpc),
                              m_n_positive_dofs(0),
                              m_grow_factor(0)
  {
    if (n_regions>0)
    {
      m_regions_tags.reserve(n_regions);

      int k = 0;
      for (int i = 0; i < n_regions; ++i)
      {
        m_regions_tags.push_back( SetVector<int>(regions_tags+k, regions_tags+k+n_tags_per_region[i]) );
        k += n_tags_per_region[i];
      }
    }
    else
    {
      m_regions_tags.push_back( SetVector<int>() );
    }
  }

  /* --- users ---*/

  // number of positive dofs
  index_t numPositiveDofs() const;
  int numDofsPerVertex() const;
  int numDofsPerCell() const;
  int numDofsPerFacet() const;
  int numDofsPerRidge() const;

  //void getCellDofs(int *dofs, Cell const*) const;
  //void getFacetDofs(int *dofs, CellElement const*) const;
  //void getCornerDofs(int *dofs, CellElement const*) const;
  //void getVertexDofs(int *dofs, CellElement const*) const;
  //void getVertexDofs(int *dofs, int vtx_id) const;
  //
  //void getCellAssociatedDofs(int* dofs, Cell const*) const;
  //void getFacetAssociatedDofs(int* dofs, CellElement const*) const;
  //void getCornerAssociatedDofs(int* dofs, CellElement const*) const;
  //void getVertexAssociatedDofs(int* dofs, CellElement const*) const;

  char const* getName() const {return m_name.c_str();};

  //void linkVertexDofs(Point const* point1, Point const* point2);



};

} // end namepspace alelib

#endif
