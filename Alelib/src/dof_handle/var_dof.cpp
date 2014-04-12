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

#include "../mesh/mesh.hpp"
#include "var_dof.hpp"
#include "../util/assert.hpp"

//void VarDofs::setType(ShapeFunction * sf, int dim, int ntags, int const*tags)
//{
//  setType(sf->numDofsAssociatedToVertice()*dim, sf->numDofsAssociatedToCorner()*dim, sf->numDofsAssociatedToFacet()*dim, sf->numDofsAssociatedToCell()*dim,ntags,tags);
//}
//
//void VarDofs::setType(int ndpv, int ndpr, int ndpf, int ndpc, int ntags, int const*tags)
//{
//  m_n_dof_within_vertice = ndpv;
//  m_n_dof_within_corner = ndpr;
//  m_n_dof_within_facet = ndpf;
//  m_n_dof_within_cell = ndpc;
//
//  if (ntags>0)
//  {
//    ALELIB_CHECK(tags!=NULL, "tags NULL pointer", std::runtime_error);
//    m_considered_tags.resize(ntags);
//  }
//  for (int i = 0; i < ntags; ++i)
//    m_considered_tags[i] = tags[i];
//}
//
void VarDofs::setType(EVarOptions options, unsigned n_regions, int const* regions)
{
  m_options = options;

  if (m_options & SPLITTED_BY_REGION_CELL)
  {
    if (regions != NULL && n_regions > 1)
    {
      for (unsigned i = 0; i < n_regions; ++i)
        m_regions.insert(regions[i]);
    }
    else
    {
      Cell const* cell;
      Mesh const* mesh = m_mesh_ptr;
      for (int i = 0; i < mesh->numCellsTotal(); ++i)
      {
        cell = mesh->getCellPtr(i);
        if (cell->isDisabled()) continue;

        int tag = cell->getTag();

        m_regions.insert(tag);
      }
    }


  }

}

int VarDofs::totalSize() const
{
  return m_vertices_dofs.size() +
         m_corners_dofs.size() +
         m_facets_dofs.size() +
         m_cells_dofs.size();
}

void VarDofs::setUp(int minimum_dof_id)
{
  unsigned const n_nodes_total = m_mesh_ptr->numNodesTotal();
  unsigned const n_corners_total = m_mesh_ptr->numCornersTotal();
  unsigned const n_facets_total = m_mesh_ptr->numFacetsTotal();
  unsigned const n_cells_total = m_mesh_ptr->numCellsTotal();

  Mesh * mesh = m_mesh_ptr;

  if (m_mesh_ptr->cellDim() < 3)
    m_n_dof_within_corner = 0;


  // find considered tags per region

  if (m_regions.size() >= 1u) // if not, the considered tags are those passed by the user
  {
    m_regional_tags.clear();
    m_regional_tags.resize(m_regions.size());
    
    Cell const* cell;
    Mesh const* mesh = m_mesh_ptr;
    SetVector<int>::const_iterator it;
    for (int i = 0; i < mesh->numCellsTotal(); ++i)
    {
      cell = mesh->getCellPtr(i);
      if (cell->isDisabled())
        continue;

      int tag = cell->getTag();

      it = m_regions.find(tag);

      if (m_regions.end() == it)
        continue;

      int reg = (int) (it - m_regions.begin());

      for (int j = 0; j < cell->numNodes(); ++j)
        m_regional_tags[reg].insert( mesh->getNodePtr(cell->getNodeId(j))->getTag() );

      if (mesh->cellDim() > 1)
        for (int j = 0; j < cell->numFacets(); ++j)
          m_regional_tags[reg].insert( mesh->getFacetPtr(cell->getFacetId(j))->getTag() );

      if (mesh->cellDim() > 2)
        for (int j = 0; j < cell->numCorners(); ++j)
          m_regional_tags[reg].insert( mesh->getCornerPtr(cell->getCornerId(j))->getTag() );
    }
    
    int ii = 0;
    for (it = m_regions.begin(); it != m_regions.end(); ++it)
    {
      m_regional_tags[ii++].insert( *it );
    }
    
        
  }
  
  int n_regions = m_regions.empty() ? 1 : (int) m_regions.size();
  
  if (m_n_dof_within_vertice > 0)
    m_vertices_dofs.reshape(n_regions, n_nodes_total, m_n_dof_within_vertice);
  if (m_n_dof_within_corner > 0)
    m_corners_dofs.reshape(n_regions, n_corners_total, m_n_dof_within_corner);  
  if (m_n_dof_within_facet > 0)
    m_facets_dofs.reshape(n_regions, n_facets_total, m_n_dof_within_facet);  
  if (m_n_dof_within_cell > 0)
    m_cells_dofs.reshape(n_regions, n_cells_total, m_n_dof_within_cell);  
  
  
  bool by_regions = m_options & SPLITTED_BY_REGION_CELL;
  
  unsigned dof_counter = minimum_dof_id;
  for (int reg = 0; reg < n_regions; ++reg)
  {
    /*  strategy: puts +1 where there is dof and -1 otherwise. After the, the numbering is done */

    int tag;
    bool is_considered;

    // vertices dof
    if (m_n_dof_within_vertice > 0)
    {
      Point const*p;
      for (unsigned i = 0; i < n_nodes_total; ++i)
      {
        p = mesh->getNodePtr(i);
        tag = p->getTag();

        // check for tag
        if (by_regions)
          is_considered = m_regional_tags.at(reg).end() != m_regional_tags[reg].find(tag);
        else
          is_considered = m_regional_tags[0].empty() ? true : m_regional_tags[0].end() != m_regional_tags[0].find(tag);


        if (!(mesh->isVertex(p)) || !is_considered || p->isDisabled() )
          for (int j = 0; j < m_vertices_dofs.dim(2); ++j)
            m_vertices_dofs.get(reg,i,j) = -1;
        else
          for (int j = 0; j < m_vertices_dofs.dim(2); ++j)
            m_vertices_dofs.get(reg,i,j) = dof_counter++;
      }
    }

    // corners dof
    if (m_n_dof_within_corner > 0)
    {

      Corner const*p;
      for (unsigned i = 0; i < n_corners_total; ++i)
      {
        p = mesh->getCornerPtr(i);
        tag = p->getTag();

        // check for tag
        if (by_regions)
          is_considered = m_regional_tags[reg].end() != m_regional_tags[reg].find(tag);
        else
          is_considered = m_regional_tags[0].empty() ? true : m_regional_tags[0].end() != m_regional_tags[0].find(tag);


        if (!is_considered || p->isDisabled())
          for (int j = 0; j < m_corners_dofs.dim(2); ++j)
            m_corners_dofs(reg,i,j) = -1;
        else
          for (int j = 0; j < m_corners_dofs.dim(2); ++j)
            m_corners_dofs(reg,i,j) = dof_counter++;
      }
    }

    // facets dof
    if (m_n_dof_within_facet > 0)
    {

      Facet const*p;
      for (unsigned i = 0; i < n_facets_total; ++i)
      {
        p = mesh->getFacetPtr(i);
        tag = p->getTag();

        // check for tag
        if (by_regions)
          is_considered = m_regional_tags[reg].end() != m_regional_tags[reg].find(tag);
        else
          is_considered = m_regional_tags[0].empty() ? true : m_regional_tags[0].end() != m_regional_tags[0].find(tag);


        if (!is_considered || p->isDisabled())
          for (int j = 0; j < m_facets_dofs.dim(2); ++j)
            m_facets_dofs(reg,i,j) = -1;
        else
          for (int j = 0; j < m_facets_dofs.dim(2); ++j)
            m_facets_dofs(reg,i,j) = dof_counter++;
      }
    }

    // cells dof
    if (m_n_dof_within_cell > 0)
    {

      Cell const*p;
      for (unsigned i = 0; i < n_cells_total; ++i)
      {
        p = mesh->getCellPtr(i);
        tag = p->getTag();

        // check for tag
        if (by_regions)
          is_considered = m_regional_tags[reg].end() != m_regional_tags[reg].find(tag);
        else
          is_considered = m_regional_tags[0].empty() ? true : m_regional_tags[0].end() != m_regional_tags[0].find(tag);


        if (!is_considered || p->isDisabled())
          for (int j = 0; j < m_cells_dofs.dim(2); ++j)
            m_cells_dofs(reg,i,j) = -1;
        else
          for (int j = 0; j < m_cells_dofs.dim(2); ++j)
            m_cells_dofs(reg,i,j) = dof_counter++;
      }
    }

    
  }
  

  m_n_positive_dofs = dof_counter - minimum_dof_id;

}

int VarDofs::numPositiveDofs() const
{
  return m_n_positive_dofs;
}

int VarDofs::numDofsPerVertex() const
{
  return m_n_dof_within_vertice;
}

int VarDofs::numDofsPerCell() const
{
  return m_n_dof_within_vertice*m_mesh_ptr->numVerticesPerCell() +
         m_n_dof_within_corner *m_mesh_ptr->numCornersPerCell() +
         m_n_dof_within_facet  *m_mesh_ptr->numFacetsPerCell() +
         m_n_dof_within_cell;

}

int VarDofs::numDofsPerFacet() const
{
  // 3D
  int const numm_corners_per_facet = m_mesh_ptr->numVerticesPerFacet();

  return m_n_dof_within_vertice*m_mesh_ptr->numVerticesPerFacet() +
         m_n_dof_within_corner *         numm_corners_per_facet +
         m_n_dof_within_facet;

}

int VarDofs::numDofsPerRidge() const
{
  return m_n_dof_within_vertice*m_mesh_ptr->numVerticesPerCorner() +
         m_n_dof_within_corner;

}

void VarDofs::getCellDofs(int *dofs, Cell const* cell) const
{
  int const n_vtcs_p_cell = m_mesh_ptr->numVerticesPerCell();
  int const n_crns_p_cell = m_mesh_ptr->numCornersPerCell();
  int const n_fcts_p_cell = m_mesh_ptr->numFacetsPerCell();

  // find region
  int tag = cell->getTag();
  int reg = 0;
  if (m_options & SPLITTED_BY_REGION_CELL)
  {
    SetVector<int>::const_iterator it = m_regions.find(tag);
    if (it != m_regions.end())
      reg = (int)(it - m_regions.begin());
  }

  // vertices
  for (int i = 0; i < n_vtcs_p_cell; ++i)
  {
    const int vtx_id = cell->getNodeId(i);

    for (int j = 0; j < m_n_dof_within_vertice; ++j)
    {
      *dofs++ = m_vertices_dofs(reg,vtx_id,j);
    }
  }

  // corners
  for (int i = 0; i < n_crns_p_cell; ++i)
  {
    const int crn_id = cell->getCornerId(i);

    for (int j = 0; j < m_n_dof_within_corner; ++j)
    {
      *dofs++ = m_corners_dofs(reg,crn_id,j);
    }
  }

  // facets
  for (int i = 0; i < n_fcts_p_cell; ++i)
  {
    const int fct_id = cell->getFacetId(i);

    for (int j = 0; j < m_n_dof_within_facet; ++j)
    {
      *dofs++ = m_facets_dofs(reg,fct_id,j);
    }
  }

  // cells
  {
    const int cell_id = m_mesh_ptr->getCellId(cell);

    for (int j = 0; j < m_n_dof_within_cell; ++j)
    {
      *dofs++ = m_cells_dofs(reg,cell_id,j);
    }
  }

}

void VarDofs::getFacetDofs(int *dofs, CellElement const* facet) const
{

  int const n_vtcs_p_facet = m_mesh_ptr->numVerticesPerFacet();
  int const n_crns_p_facet = n_vtcs_p_facet; // belive

  Cell const* icell = m_mesh_ptr->getCellPtr(facet->getIncidCell());
  const int pos = facet->getPosition();

  //int vtcs_ids[n_vtcs_p_facet];
  int *vtcs_ids = new int [n_vtcs_p_facet];
  icell->getFacetVerticesId(pos, vtcs_ids);

  // find region
  int tag = facet->getTag();
  int reg = 0;
  if (m_options & SPLITTED_BY_REGION_CELL)
  {
    for (unsigned k = 0; k < m_regional_tags.size(); ++k)
    {
      SetVector<int>::const_iterator it = m_regional_tags[k].find(tag);
      if (it != m_regional_tags[k].end())
      {
        reg = k;
        break;
      }
    }
  }

  // vertices
  for (int i = 0; i < n_vtcs_p_facet; ++i)
  {
    for (int j = 0; j < m_n_dof_within_vertice; ++j)
    {
      *dofs++ = m_vertices_dofs(reg,vtcs_ids[i],j);
    }
  }

  //int crns_ids[n_crns_p_facet];
  int *crns_ids = new int [n_crns_p_facet];
  icell->getFacetCornersId(pos, crns_ids);

  // corners
  for (int i = 0; i < n_crns_p_facet; ++i)
  {
    for (int j = 0; j < m_n_dof_within_corner; ++j)
    {
      *dofs++ = m_corners_dofs(reg,crns_ids[i],j);
    }
  }

  // facets
  {
    //const int fct_id = m_mesh_ptr->getFacetId(facet);
    const int fct_id = m_mesh_ptr->getCellPtr(facet->getIncidCell())->getFacetId(facet->getPosition());

    for (int j = 0; j < m_n_dof_within_facet; ++j)
    {
      *dofs++ = m_facets_dofs(reg,fct_id,j);
    }
  }

  delete [] vtcs_ids;
  vtcs_ids = NULL;
  delete [] crns_ids;
  crns_ids = NULL;
}

void VarDofs::getCornerDofs(int *dofs, CellElement const* corner) const
{
  int const n_vtcs_p_corner = m_mesh_ptr->numVerticesPerCorner();

  Cell const* icell = m_mesh_ptr->getCellPtr(corner->getIncidCell());
  const int pos = corner->getPosition();

  //int vtcs_ids[n_vtcs_p_corner];
  int *vtcs_ids = new int [n_vtcs_p_corner];
  icell->getCornerVerticesId(pos, vtcs_ids);

  // find region
  int tag = corner->getTag();
  int reg = 0;
  if (m_options & SPLITTED_BY_REGION_CELL)
  {
    for (unsigned k = 0; k < m_regional_tags.size(); ++k)
    {
      SetVector<int>::const_iterator it = m_regional_tags[k].find(tag);
      if (it != m_regional_tags[k].end())
      {
        reg = k;
        break;
      }
    }
  }
  

  // vertices
  for (int i = 0; i < n_vtcs_p_corner; ++i)
  {
    for (int j = 0; j < m_n_dof_within_vertice; ++j)
    {
      *dofs++ = m_vertices_dofs(reg,vtcs_ids[i],j);
    }
  }

  // corners
  {
    //const int crn_id = m_mesh_ptr->getCornerId(corner);
    const int crn_id = m_mesh_ptr->getCellPtr(corner->getIncidCell())->getCornerId(corner->getPosition());

    for (int j = 0; j < m_n_dof_within_corner; ++j)
    {
      *dofs++ = m_corners_dofs(reg,crn_id,j);
    }
  }

  delete [] vtcs_ids;
  vtcs_ids = NULL;
}

void VarDofs::getVertexDofs(int *dofs, CellElement const* point) const
{

  // find region
  int tag = point->getTag();
  int reg = 0;
  if (m_options & SPLITTED_BY_REGION_CELL)
  {
    for (unsigned k = 0; k < m_regional_tags.size(); ++k)
    {
      SetVector<int>::const_iterator it = m_regional_tags[k].find(tag);
      if (it != m_regional_tags[k].end())
      {
        reg = k;
        break;
      }
    }
  }
  

  // vertices
  {
    //const int pt_id = m_mesh_ptr->getPointId(point);
    const int pt_id = m_mesh_ptr->getCellPtr(point->getIncidCell())->getNodeId(point->getPosition());

    for (int j = 0; j < m_n_dof_within_vertice; ++j)
    {
      *dofs++ = m_vertices_dofs.get(reg,pt_id,j);
    }
  }

}

void VarDofs::getVertexDofs(int *dofs, int pt_id) const
{
  // find region
  int tag = m_mesh_ptr->getNodePtr(pt_id)->getTag();
  int reg = 0;
  if (m_options & SPLITTED_BY_REGION_CELL)
  {
    for (unsigned k = 0; k < m_regional_tags.size(); ++k)
    {
      SetVector<int>::const_iterator it = m_regional_tags[k].find(tag);
      if (it != m_regional_tags[k].end())
      {
        reg = k;
        break;
      }
    }
  }

  for (int j = 0; j < m_n_dof_within_vertice; ++j)
  {
    *dofs++ = m_vertices_dofs(reg,pt_id,j);
  }
}


void VarDofs::getCellAssociatedDofs(int* dofs, Cell const* cell) const
{
  // find region
  int tag = cell->getTag();
  int reg = 0;
  if (m_options & SPLITTED_BY_REGION_CELL)
  {
    SetVector<int>::const_iterator it = m_regions.find(tag);
    if (it != m_regions.end())
      reg = (int)(it - m_regions.begin());
  }

  const int cell_id = m_mesh_ptr->getCellId(cell);

  for (int j = 0; j < m_n_dof_within_cell; ++j)
    *dofs++ = m_cells_dofs(reg,cell_id,j);
}
void VarDofs::getFacetAssociatedDofs(int* dofs, CellElement const* facet) const
{
  //const int fct_id = m_mesh_ptr->getFacetId(facet);
  const int fct_id = m_mesh_ptr->getCellPtr(facet->getIncidCell())->getFacetId(facet->getPosition());

  // find region
  int tag = facet->getTag();
  int reg = 0;
  if (m_options & SPLITTED_BY_REGION_CELL)
  {
    for (unsigned k = 0; k < m_regional_tags.size(); ++k)
    {
      SetVector<int>::const_iterator it = m_regional_tags[k].find(tag);
      if (it != m_regional_tags[k].end())
      {
        reg = k;
        break;
      }
    }
  }

  for (int j = 0; j < m_n_dof_within_facet; ++j)
    *dofs++ = m_facets_dofs(reg,fct_id,j);
}
void VarDofs::getCornerAssociatedDofs(int* dofs, CellElement const* corner) const
{
  //const int crn_id = m_mesh_ptr->getCornerId(corner);
  const int crn_id = m_mesh_ptr->getCellPtr(corner->getIncidCell())->getCornerId(corner->getPosition());

  // find region
  int tag = corner->getTag();
  int reg = 0;
  if (m_options & SPLITTED_BY_REGION_CELL)
  {
    for (unsigned k = 0; k < m_regional_tags.size(); ++k)
    {
      SetVector<int>::const_iterator it = m_regional_tags[k].find(tag);
      if (it != m_regional_tags[k].end())
      {
        reg = k;
        break;
      }
    }
  }

  for (int j = 0; j < m_n_dof_within_corner; ++j)
    *dofs++ = m_corners_dofs(reg,crn_id,j);
}
void VarDofs::getVertexAssociatedDofs(int* dofs, CellElement const* point) const
{
  getVertexDofs(dofs, point);
}








