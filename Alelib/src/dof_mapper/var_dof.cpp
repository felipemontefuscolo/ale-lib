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

#include "../mesh/mesh.hpp"
#include "var_dof.hpp"
#include "../util/assert.hpp"
#include "../mesh/mesh.hpp"
#include <algorithm>

namespace alelib
{

template<class Mesh_t>
void VarDofs<Mesh_t>::setUp(index_t first_dof_id)
{
  index_t const n_verts_total   = (index_t)(m_mp->numVerticesTotal());
  index_t const n_ridges_total  = (index_t)(m_mp->numRidgesTotal()  );
  index_t const n_facets_total  = (index_t)(m_mp->numFacetsTotal()  );
  index_t const n_cells_total   = (index_t)(m_mp->numCellsTotal()   );

  if (MeshT::cell_dim < 3)
    m_n_dofs_in_ridge = 0;


  // Detecting regions by cell tags.
  // Assume that the mesh is tagged properly.
  // If a subcell belongs exclusively to a region, then its tag can not be the same of
  // another region
  if (m_options & AUTO_DETECT_REGIONS)
  {
    m_regions_tags.clear();

    // creating regions vector
    {
      SetVector<int> tags;
      for (CellH cell = m_mp->cellBegin(), cell_end = m_mp->cellEnd(); cell != cell_end; ++cell)
      {
        if (cell.isDisabled(m_mp))
          continue;

        int tag = cell.tag(m_mp);

        tags.insert(tag);
      }
      m_regions_tags.reserve(tags.size());
      for(unsigned i=0; i<tags.size(); ++i)
        m_regions_tags.push_back(SetVector<int>(tags.begin()+i, tags.begin()+i+1));
    }

    // including subcells tags
    for (CellH cell = m_mp->cellBegin(), cell_end = m_mp->cellEnd(); cell != cell_end; ++cell)
    {
      if (cell.isDisabled(m_mp))
        continue;

      int ctag = cell.tag(m_mp);
      int region = this->region(ctag);
      ALELIB_CHECK(region >= 0, "Var setUp failed. Can not detect region", std::runtime_error);

      // including vertex tags
      {
        VertexH verts[MeshT::verts_per_cell];
        cell.vertices(m_mp, verts);

        for (int i = 0; i < MeshT::verts_per_cell; ++i)
        {
          int vtag = verts[i].tag(m_mp);
          m_regions_tags.at(region).insert(vtag);
        }
      }

      // including ridges tags
      if (MeshT::cell_dim > 2)
      {
        RidgeH ridges[MeshT::ridges_per_cell + 1]; // +1 to avoid compiler warnings
        cell.ridges(m_mp, ridges);

        for (int i = 0; i < MeshT::ridges_per_cell; ++i)
        {
          int rtag = ridges[i].tag(m_mp);
          m_regions_tags.at(region).insert(rtag);
        }
      }

      // including facets tags
      if (MeshT::cell_dim > 1)
      {
        FacetH facets[MeshT::facets_per_cell];
        cell.facets(m_mp, facets);

        for (int i = 0; i < MeshT::facets_per_cell; ++i)
        {
          int ftag = facets[i].tag(m_mp);
          m_regions_tags.at(region).insert(ftag);
        }
      }

    }
  }

  int n_regions = std::max(1ul, m_regions_tags.size());
  bool const number_by_regions = !m_regions_tags.empty();


  m_verts_dofs.clear();
  m_ridges_dofs.clear();
  m_facets_dofs.clear();
  m_cells_dofs.clear();

  if (m_n_dofs_in_vtx > 0)
    m_verts_dofs.reshape(marray::listify(n_regions, n_verts_total, m_n_dofs_in_vtx).v, -1);
  if (m_n_dofs_in_ridge > 0)
    m_ridges_dofs.reshape(marray::listify(n_regions, n_ridges_total, m_n_dofs_in_ridge).v, -1);
  if (m_n_dofs_in_facet > 0)
    m_facets_dofs.reshape(marray::listify(n_regions, n_facets_total, m_n_dofs_in_facet).v, -1);
  if (m_n_dofs_in_cell > 0)
    m_cells_dofs.reshape(marray::listify(n_regions, n_cells_total, m_n_dofs_in_cell).v, -1);


  index_t dof_counter = first_dof_id;
  for (int reg = 0; reg < n_regions; ++reg)
  {
    // vertices dofs
    if (m_n_dofs_in_vtx > 0)
    {
      for (VertexH p = m_mp->vertexBegin(), p_end = m_mp->vertexEnd(); p!=p_end; ++p)
      {
        if (p.isDisabled(m_mp))
          continue;
        int tag = p.tag(m_mp);

        // check if tag is in the region reg
        bool in_region = true;
        
        if (number_by_regions)
          in_region = m_regions_tags[reg].find(tag) != m_regions_tags[reg].end();
        
        if (in_region)
          for (int j = 0; j < m_n_dofs_in_vtx; ++j)
            m_verts_dofs[reg][p.id(m_mp)][j] = dof_counter++;
      }
    }

    // ridges dofs
    if (m_n_dofs_in_ridge > 0)
    {
      for (RidgeH p = m_mp->ridgeBegin(), p_end = m_mp->ridgeEnd(); p!=p_end; ++p)
      {
        if (p.isDisabled(m_mp))
          continue;
        int tag = p.tag(m_mp);

        // check if tag is in the region reg
        bool in_region = true;
        
        if (number_by_regions)
          in_region = m_regions_tags[reg].find(tag) != m_regions_tags[reg].end();
        
        if (in_region)
          for (int j = 0; j < m_n_dofs_in_ridge; ++j)
            m_ridges_dofs[reg][p.id(m_mp)][j] = dof_counter++;
      }
    }

    // facets dofs
    if (m_n_dofs_in_facet > 0)
    {
      for (FacetH p = m_mp->facetBegin(), p_end = m_mp->facetEnd(); p!=p_end; ++p)
      {
        if (p.isDisabled(m_mp))
          continue;
        int tag = p.tag(m_mp);

        // check if tag is in the region reg
        bool in_region = true;
        
        if (number_by_regions)
          in_region = m_regions_tags[reg].find(tag) != m_regions_tags[reg].end();
        
        if (in_region)
          for (int j = 0; j < m_n_dofs_in_facet; ++j)
            m_facets_dofs[reg][p.id(m_mp)][j] = dof_counter++;
      }
    }

    // cells dofs
    if (m_n_dofs_in_cell > 0)
    {
      for (CellH p = m_mp->cellBegin(), p_end = m_mp->cellEnd(); p!=p_end; ++p)
      {
        if (p.isDisabled(m_mp))
          continue;
        int tag = p.tag(m_mp);

        // check if tag is in the region reg
        bool in_region = true;
        
        if (number_by_regions)
          in_region = m_regions_tags[reg].find(tag) != m_regions_tags[reg].end();
        
        if (in_region)
          for (int j = 0; j < m_n_dofs_in_cell; ++j)
            m_cells_dofs[reg][p.id(m_mp)][j] = dof_counter++;
      }
    }

  }
  m_n_positive_dofs = dof_counter - first_dof_id;

}



template class VarDofs<Mesh<EDGE> >       ;
template class VarDofs<Mesh<TRIANGLE> >   ;
template class VarDofs<Mesh<QUADRANGLE> > ;
template class VarDofs<Mesh<TETRAHEDRON> >;
template class VarDofs<Mesh<HEXAHEDRON> > ;

} // end namespace alelib




