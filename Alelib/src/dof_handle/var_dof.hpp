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

#include "dof_traits.hpp"
#include "../shapefunctions/shape_functions.hpp"
#include "Fepic/src/mesh/elements/cell_element.hpp"
#include "Array/array.hpp"
#include <vector>
#include <string>
#include "contrib/Loki/set_vector.hpp"

class Mesh;
class ShapeFunction;
class Cell;
class Facet;
class Corner;
class Point;

enum EVarOptions{
  DEFAULT                  = 0,
  SPLITTED_BY_REGION_CELL  = 1 << 0,
  SPLITTED_BY_REGION_FACET = 1 << 1
};

class VarDofs
{
  friend class DofHandler;

  typedef marray::Array<int, 3> Container; // (reg, obj_id, component)

  void setMesh(Mesh *m) {m_mesh_ptr = m;}
  //void setType(ShapeFunction * sf, int dim=1, int ntags=0, int const*tags=NULL);
  //void setType(int ndpv, int ndpr, int ndpf, int ndpc, int ntags=0, int const*tags=NULL);

  void setUp(int minimum_dof_id);

  void linkDofs(int size, int const* dofs1, int const* dofs2); // do dofs2 = dofs1

public:

  VarDofs(const char* name, Mesh * m=NULL, int ndpv=0, int ndpr=0, int ndpf=0, int ndpc=0, int ntags=0, int const*tags=NULL)
                              : m_name(name), m_mesh_ptr(m), m_options(DEFAULT)
  {
    m_n_dof_within_vertice = ndpv; // interior
    m_n_dof_within_corner = ndpr;  // interior
    m_n_dof_within_facet = ndpf;   // interior
    m_n_dof_within_cell = ndpc;    // interior
    m_n_positive_dofs = 0;

    if (ntags>0)
    {
      ALELIB_CHECK(tags!=NULL, "tags NULL pointer", std::runtime_error);
      //for (int i = 0; i < ntags; ++i)
      //  m_considered_tags.insert(tags[i]);
      m_regional_tags.reserve(2);
      m_regional_tags.push_back( SetVector<int>(tags, tags+ntags) );
    }
    else
    {
      m_regional_tags.push_back( SetVector<int>() );
    }
    
    // nao precisa disso aqui, pode ser feito no setUp()
    //if (m!=NULL && m->cellDim() < 3)
    //  m_n_dof_within_corner=0;
    
  }

  /* --- users ---*/

  // If 'by_mesh_volume' = true, 'by_mesh_boundary' is ignored. If regions is empty, the regions are found
  // automatically
  void setType(EVarOptions options, unsigned n_regions, int const* regions);
  
  // number of positive dofs
  int numPositiveDofs() const;
  int numDofsPerVertex() const;
  int numDofsPerCell() const;
  int numDofsPerFacet() const;
  int numDofsPerRidge() const;

  void getCellDofs(int *dofs, Cell const*) const;
  void getFacetDofs(int *dofs, CellElement const*) const;
  void getCornerDofs(int *dofs, CellElement const*) const;
  void getVertexDofs(int *dofs, CellElement const*) const;
  void getVertexDofs(int *dofs, int vtx_id) const;
  
  void getCellAssociatedDofs(int* dofs, Cell const*) const;
  void getFacetAssociatedDofs(int* dofs, CellElement const*) const;
  void getCornerAssociatedDofs(int* dofs, CellElement const*) const;
  void getVertexAssociatedDofs(int* dofs, CellElement const*) const;
  
  char const* getName() const {return m_name.c_str();};

  //void linkVertexDofs(Point const* point1, Point const* point2);

  // number of stored integers
  int  totalSize() const;

  float getGrowFactor() const {return m_grow_factor;}

protected:
  std::string    m_name;
  Mesh*          m_mesh_ptr;
  int            m_n_dof_within_vertice;
  int            m_n_dof_within_corner;
  int            m_n_dof_within_facet;
  int            m_n_dof_within_cell;
  int            m_n_positive_dofs;    // It includes linked dofs
  float          m_grow_factor;

  EVarOptions m_options;
  
  std::vector<SetVector<int> > m_regional_tags; // The considered tags of each region. If m_regional_tags[0] is empty then all tags are considered.
  SetVector<int>               m_regions;       // Each region is represented by a cell (or facet) tag. Dofs that lies at intersections are splitted.
                                                // This vector stores the tags of each region.
                                                // if m_regions.size() == 0 or 1, thus there is one region only.

  //SetVector<int> m_considered_tags; /* if m_considered_tags.size()==0, then all tags are considered. This vector is ignored
  //                                     if the variable is defined by regions (SPLITTED_BY_REGION_CELL or SPLITTED_BY_REGION_FACET)*/

  Container m_vertices_dofs;  // 0
  Container m_corners_dofs;   // 1
  Container m_facets_dofs;    // 2
  Container m_cells_dofs;     // 3
  

};


#endif
