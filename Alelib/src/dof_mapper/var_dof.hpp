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

#ifndef ALELIB_VAR_DOF_HPP
#define ALELIB_VAR_DOF_HPP

#include "Array/array.hpp"
#include <vector>
#include <string>
#include "contrib/Loki/set_vector.hpp"
#include "conf/directives.hpp"
#include "Alelib/src/util/assert.hpp"

namespace alelib
{

template<class Mesh_t> class DofMapper;

enum VarOptions
{
  AUTO_DETECT_REGIONS = 1 << 0
};

template<class Mesh_t>
class VarDofs
{
  friend
  class DofMapper<Mesh_t>;

  typedef marray::Array<index_t, 3> Container; // (region(defined by tags), obj_id(vts, cells, etc.), component)

public:
  typedef Mesh_t MeshT;
  typedef typename MeshT::VertexH VertexH;
  typedef typename MeshT::CellH CellH;
  typedef typename MeshT::FacetH FacetH;
  typedef typename MeshT::RidgeH RidgeH;

  static const int cell_dim = MeshT::cell_dim;

protected:
  std::string    m_name;
  MeshT const*   m_mp;
  int            m_n_dofs_in_vtx;
  int            m_n_dofs_in_ridge;
  int            m_n_dofs_in_facet;
  int            m_n_dofs_in_cell;
  index_t        m_n_positive_dofs;    // It is not the number of dofs, because may exist duplicated dofs in the container (e.g. periodic)
                                       // It means that there may be gaps in the dofs numbering
  float          m_grow_factor;        // for vector reserve

  std::vector<SetVector<int> > m_regions_tags; // Each region is defined by a set of tags.
                                               // If m_regions_tags is empty then this variable is defined along all tags.

  char m_options;

                              // ordering of c. below
  Container m_verts_dofs;     // 0
  Container m_ridges_dofs;    // 1
  Container m_facets_dofs;    // 2
  Container m_cells_dofs;     // 3

  void setUp(index_t first_dof_id);

  void linkDofs(int size, int const* dofs1, int const* dofs2); // do dofs2 = dofs1

public:

  //
  // The region i has n_tags_per_region[i] tags.
  //
  //explicit
  template<class Iterator1, class Iterator2>
  VarDofs(MeshT const* mp, const char* name, int ndpv, int ndpr, int ndpf, int ndpc,
                            int n_regions, Iterator1 n_tags_per_region, Iterator2 regions_tags)
                            : m_name(name),
                              m_mp(mp),
                              m_n_dofs_in_vtx  (ndpv),
                              m_n_dofs_in_ridge(ndpr),
                              m_n_dofs_in_facet(ndpf),
                              m_n_dofs_in_cell (ndpc),
                              m_n_positive_dofs(0),
                              m_grow_factor(0),
                              m_regions_tags(),
                              m_options(0)
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
  }

  // Some notes about AUTO_DETECT_REGIONS:
  // In this option, each different cell tag defined a region. Additionally, sub-cells tags are included
  // in the correspondent region. If a subcell belongs exclusively to a region, then its tag can not be the same of
  // another region
  VarDofs(MeshT const* mp, const char* name, int ndpv, int ndpr, int ndpf, int ndpc, char options = 0)
                            : m_name(name),
                              m_mp(mp),
                              m_n_dofs_in_vtx  (ndpv),
                              m_n_dofs_in_ridge(ndpr),
                              m_n_dofs_in_facet(ndpf),
                              m_n_dofs_in_cell (ndpc),
                              m_n_positive_dofs(0),
                              m_grow_factor(0),
                              m_regions_tags(),
                              m_options(options)
  { }






  /* --- users ---*/

  int numRegions() const
  {
    std::size_t a = std::max(1ul, m_verts_dofs.dim(0));
    a = std::max(a, m_ridges_dofs.dim(0));
    a = std::max(a, m_facets_dofs.dim(0));
    a = std::max(a, m_cells_dofs.dim(0));
    
    return a;
  }

  // return the number of non-negative stores dofs (not the number of dofs)
  index_t numPositiveDofs() const
  { return m_n_positive_dofs; }

  int numDofsInVertex() const
  { return m_n_dofs_in_vtx; }

  int numDofsInRidge() const
  { return m_n_dofs_in_ridge; }

  int numDofsInFacet() const
  { return m_n_dofs_in_facet; }

  int numDofsInCell() const
  { return m_n_dofs_in_cell; }


  int numDofsPerVertex() const
  {
    return m_n_dofs_in_vtx;
  }

  int numDofsPerCell() const
  {
    return m_n_dofs_in_vtx    * MeshT::verts_per_cell +
           m_n_dofs_in_ridge  * MeshT::ridges_per_cell +
           m_n_dofs_in_facet  * MeshT::facets_per_cell +
           m_n_dofs_in_cell;
  }

  int numDofsPerFacet() const
  {
    return m_n_dofs_in_vtx   *  MeshT::verts_per_facet +
           m_n_dofs_in_ridge *  MeshT::ridges_per_facet +
           m_n_dofs_in_facet;
  }

  int numDofsPerRidge() const
  {
    return m_n_dofs_in_vtx    *MeshT::verts_per_ridge +
           m_n_dofs_in_ridge;
  }




  /// returns the region in which tag lives
  int region(int tag) const
  {
    if (m_regions_tags.empty())
      return 0;

    for (unsigned i = 0; i < m_regions_tags.size(); ++i)
    {
      SetVector<int>::const_iterator it = m_regions_tags[i].find(tag);
      if (it != m_regions_tags[i].end())
        // so the tag was found
        return i;
    }
    return -1; // no region was found
  }

  template<class OutIterator>
  OutIterator getVertexDofs(OutIterator dofs, VertexH vtx, int region = 0) const
  {
    ALELIB_CHECK(vtx.isValid(m_mp), "invalid vertex", std::runtime_error);
    
    if (region < 0)
    {
      int const n_dofs = numDofsPerVertex();
      for (int i = 0; i < n_dofs; ++i)
        *dofs++ = -1;
      return dofs;
    }

    index_t const pt_id = vtx.id(m_mp);
    for (int j = 0; j < m_n_dofs_in_vtx; ++j)
    {
      *dofs++ = m_verts_dofs[region][pt_id][j];
    }
    return dofs;
  }

  template<class OutIterator>
  OutIterator getRidgeDofs(OutIterator dofs, RidgeH ridge, int region = 0) const
  {
    ALELIB_CHECK(ridge.isValid(m_mp), "invalid ridge", std::runtime_error);
    
    if (MeshT::cell_dim < 3)
      return dofs;
    
    if (region < 0)
    {
      int const n_dofs = numDofsPerRidge();
      for (int i = 0; i < n_dofs; ++i)
        *dofs++ = -1;
      return dofs;
    }

    // vertices
    {
      VertexH verts[MeshT::verts_per_ridge];
      ridge.vertices(m_mp, verts);

      for (int i = 0; i < MeshT::verts_per_ridge; ++i)
      {
        index_t const pt_id = verts[i].id(m_mp);
        for (int j = 0; j < m_n_dofs_in_vtx; ++j)
        {
          *dofs++ = m_verts_dofs[region][pt_id][j];
        }
      }
    }

    // ridges
    {
      index_t const ridge_id = ridge.id(m_mp);
      for (int j = 0; j < m_n_dofs_in_ridge; ++j)
      {
        *dofs++ = m_ridges_dofs[region][ridge_id][j];
      }
    }

    return dofs;
  }

  template<class OutIterator>
  OutIterator getFacetDofs(OutIterator dofs, FacetH facet, int region = 0) const
  {
    ALELIB_CHECK(facet.isValid(m_mp), "invalid facet", std::runtime_error);
    
    if (region < 0)
    {
      int const n_dofs = numDofsPerFacet();
      for (int i = 0; i < n_dofs; ++i)
        *dofs++ = -1;
      return dofs;
    }

    // vertices
    {
      VertexH verts[MeshT::verts_per_facet];
      facet.vertices(m_mp, verts);
      for (int i = 0; i < MeshT::verts_per_facet; ++i)
      {
        index_t const pt_id = verts[i].id(m_mp);
        for (int j = 0; j < m_n_dofs_in_vtx; ++j)
        {
          *dofs++ = m_verts_dofs[region][pt_id][j];
        }
      }
    }

    // ridges
    if (MeshT::cell_dim > 2)
    {
      RidgeH ridges[MeshT::ridges_per_facet];
      facet.ridges(m_mp, ridges);

      // ridges
      for (int i = 0; i < MeshT::ridges_per_facet; ++i)
      {
        index_t const rd_id = ridges[i].id(m_mp);
        for (int j = 0; j < m_n_dofs_in_ridge; ++j)
        {
          *dofs++ = m_ridges_dofs[region][rd_id][j];
        }
      }
    }

    // facets
    {
      index_t const f_id = facet.id(m_mp);
      for (int j = 0; j < m_n_dofs_in_facet; ++j)
      {
        *dofs++ = m_facets_dofs[region][f_id][j];
      }
    }

    return dofs;
  }

  template<class OutIterator>
  OutIterator getCellDofs(OutIterator dofs, CellH cell) const
  {
    ALELIB_CHECK(cell.isValid(m_mp), "invalid cell", std::runtime_error);
    
    int const reg = region(cell.tag(m_mp));

    if (reg < 0)
    {
      int const n_dofs = numDofsPerCell();
      for (int i = 0; i < n_dofs; ++i)
        *dofs++ = -1;
      return dofs;
    }

    // vertices
    {
      VertexH verts[MeshT::verts_per_cell];
      cell.vertices(m_mp, verts);
      for (int i = 0; i < MeshT::verts_per_cell; ++i)
      {
        index_t const pt_id = verts[i].id(m_mp);
        for (int j = 0; j < m_n_dofs_in_vtx; ++j)
        {
          *dofs++ = m_verts_dofs[reg][pt_id][j];
        }
      }
    }

    // ridges
    if (MeshT::cell_dim > 2)
    {
      RidgeH ridges[MeshT::ridges_per_cell];
      cell.ridges(m_mp, ridges);
      for (int i = 0; i < MeshT::ridges_per_cell; ++i)
      {
        index_t const rd_id = ridges[i].id(m_mp);
        for (int j = 0; j < m_n_dofs_in_ridge; ++j)
        {
          *dofs++ = m_ridges_dofs[reg][rd_id][j];
        }
      }
    }

    // facets
    {
      FacetH facets[MeshT::facets_per_cell];
      cell.facets(m_mp, facets);

      for (int i = 0; i < MeshT::facets_per_cell; ++i)
      {
        index_t const f_id = facets[i].id(m_mp);
        for (int j = 0; j < m_n_dofs_in_facet; ++j)
        {
          *dofs++ = m_facets_dofs[reg][f_id][j];
        }
      }

    }

    // cells
    {
      const index_t cell_id = cell.id(m_mp);
      for (int j = 0; j < m_n_dofs_in_cell; ++j)
      {
        *dofs++ = m_cells_dofs[reg][cell_id][j];
      }
    }

    return dofs;
  }

  // an auxilary
  template<class OutIterator>
  OutIterator getCellDofsTags(OutIterator dofs, CellH cell) const
  {
    int const reg = region(cell.tag(m_mp));

    if (reg < 0)
    {
      int const n_dofs = numDofsPerCell();
      for (int i = 0; i < n_dofs; ++i)
        *dofs++ = -1;
      return dofs;
    }

    // vertices
    {
      VertexH verts[MeshT::verts_per_cell];
      cell.vertices(m_mp, verts);
      for (int i = 0; i < MeshT::verts_per_cell; ++i)
      {
        index_t const pt_id = verts[i].id(m_mp);
        for (int j = 0; j < m_n_dofs_in_vtx; ++j)
        {
          *dofs++ = m_verts_dofs[reg][pt_id][j];
        }
      }
    }

    // ridges
    if (MeshT::cell_dim > 2)
    {
      RidgeH ridges[MeshT::ridges_per_cell];
      cell.ridges(m_mp, ridges);
      for (int i = 0; i < MeshT::ridges_per_cell; ++i)
      {
        index_t const rd_id = ridges[i].id(m_mp);
        for (int j = 0; j < m_n_dofs_in_ridge; ++j)
        {
          *dofs++ = m_ridges_dofs[reg][rd_id][j];
        }
      }
    }

    // facets
    {
      FacetH facets[MeshT::facets_per_cell];
      cell.facets(m_mp, facets);

      for (int i = 0; i < MeshT::facets_per_cell; ++i)
      {
        index_t const f_id = facets[i].id(m_mp);
        for (int j = 0; j < m_n_dofs_in_facet; ++j)
        {
          *dofs++ = m_facets_dofs[reg][f_id][j];
        }
      }

    }

    // cells
    {
      const index_t cell_id = cell.id(m_mp);
      for (int j = 0; j < m_n_dofs_in_cell; ++j)
      {
        *dofs++ = m_cells_dofs[reg][cell_id][j];
      }
    }

    return dofs;
  }




  template<class OutIterator>
  OutIterator getDofsAtPoint(OutIterator dofs, VertexH vtx, int region = 0) const
  {
    return getVertexDofs(dofs, vtx, region);
  }

  template<class OutIterator>
  OutIterator getDofsInRidge(OutIterator dofs, RidgeH ridge, int region = 0) const
  {
    if (region < 0)
    {
      int const n_dofs = numDofsInRidge();
      for (int i = 0; i < n_dofs; ++i)
        *dofs++ = -1;
      return dofs;
    }

    // ridges
    {
      index_t const ridge_id = ridge.id(m_mp);
      for (int j = 0; j < m_n_dofs_in_ridge; ++j)
      {
        *dofs++ = m_ridges_dofs[region][ridge_id][j];
      }
    }

    return dofs;
  }

  template<class OutIterator>
  OutIterator getDofsInFacet(OutIterator dofs, FacetH facet, int region = 0) const
  {
    if (region < 0)
    {
      int const n_dofs = numDofsInFacet();
      for (int i = 0; i < n_dofs; ++i)
        *dofs++ = -1;
      return dofs;
    }

    // facets
    {
      index_t const f_id = facet.id(m_mp);
      for (int j = 0; j < m_n_dofs_in_facet; ++j)
      {
        *dofs++ = m_facets_dofs[region][f_id][j];
      }
    }

  }

  template<class OutIterator>
  OutIterator getDofsInCell(OutIterator dofs, CellH cell) const
  {
    int const reg = region(cell.tag(m_mp));

    if (reg < 0)
    {
      int const n_dofs = numDofsInCell();
      for (int i = 0; i < n_dofs; ++i)
        *dofs++ = -1;
      return dofs;
    }

    // cells
    {
      const index_t cell_id = cell.id(m_mp);
      for (int j = 0; j < m_n_dofs_in_cell; ++j)
      {
        *dofs++ = m_cells_dofs[reg][cell_id][j];
      }
    }
    
    return dofs;
  }






  //
  //void getCellAssociatedDofs(int* dofs, Cell const*) const;
  //void getFacetAssociatedDofs(int* dofs, CellElement const*) const;
  //void getCornerAssociatedDofs(int* dofs, CellElement const*) const;
  //void getVertexAssociatedDofs(int* dofs, CellElement const*) const;

  char const* getName() const
  { return m_name.c_str();}

  //void linkVertexDofs(Point const* point1, Point const* point2);



};

} // end namepspace alelib

#endif
