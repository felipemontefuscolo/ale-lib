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

#ifndef ALELIB_DOF_MAPPER_HPP
#define ALELIB_DOF_MAPPER_HPP

#include <vector>
#include "var_dof.hpp"
#include "../util/assert.hpp"

namespace alelib
{

template<class Mesh_t>
class DofMapper
{


public:
  typedef Mesh_t MeshT;
  typedef typename MeshT::VertexH VertexH;
  typedef typename MeshT::CellH CellH;
  typedef typename MeshT::FacetH FacetH;
  typedef typename MeshT::RidgeH RidgeH;
  typedef VarDofs<Mesh_t> VarT;

protected:
  MeshT const* m_mp;
  index_t m_n_links;
  std::vector<VarT> m_vars;

public:

  DofMapper(MeshT const* mesh = NULL) : m_mp(mesh), m_n_links(0), m_vars()
  {}

  /*  Add a variable.
   * @param var_name name of the variable
   * @param ndpv number of dofs per vertice
   * @param ndpr number of dofs per corner (ignored for 1D and 2D cells)
   * @param ndpf number of dofs per facet (ignored for 1D cells)
   * @param ndpc number of dofs per cell
   */
  template<class Iterator1, class Iterator2>
  void addVariable(const char* var_name, int ndpv, int ndpr, int ndpf, int ndpc,
                                         int n_regions, Iterator1 n_tags_per_region, Iterator2 regions_tags)
  {
    m_vars.push_back(VarT(m_mp,var_name,ndpv,ndpr,ndpf,ndpc,n_regions,n_tags_per_region,regions_tags));
  }

  /*  Add a variable.
   * @param var_name name of the variable
   * @param ndpv number of dofs per vertice
   * @param ndpr number of dofs per corner (ignored for 1D and 2D cells)
   * @param ndpf number of dofs per facet (ignored for 1D cells)
   * @param ndpc number of dofs per cell
   * @param options avaibles: AUTO_DETECT_REGIONS
   */
  void addVariable(const char* var_name, int ndpv, int ndpr, int ndpf, int ndpc, char options = 0)
  {
    m_vars.push_back(VarT(m_mp,var_name,ndpv,ndpr,ndpf,ndpc,options));
  }

  VarT const& variable(int i) const
  { return m_vars[i]; }

  void attachMesh(MeshT const* mesh)
  {
    m_mp = mesh;
  }

  int numVars() const
  { return m_vars.size(); }

  index_t numDofs() const
  {
    index_t total = 0;
    for (unsigned i = 0; i < m_vars.size(); ++i)
      total += m_vars[i].numPositiveDofs();
    return total - m_n_links;
  }

  index_t numPositiveDofs() const
  {
    index_t total = 0;
    for (unsigned i = 0; i < m_vars.size(); ++i)
      total += m_vars[i].numPositiveDofs();
    return total;
  }

  void SetUp(index_t initial_dof = 0)
  {
    for (unsigned i = 0; i < m_vars.size(); ++i)
    {
      m_vars[i].setUp(initial_dof);
      initial_dof += m_vars[i].numPositiveDofs();
    }
  }

  private:
  struct AuxRemoveGaps
  {
    static bool compare_ptr(index_t const* a, index_t const* b)
    {
      return *a < *b;
    }
  };
  public:

  // this function will change the order of the dofs
  void linkDofs(index_t size, index_t* dofs1, index_t* dofs2)
  {
    std::vector<index_t*> data(numPositiveDofs(),NULL);

    // is also check user's input
    for (index_t j = 0; j < size; ++j)
    {
      bool err = (dofs2[j] != dofs1[j]) || dofs2[j] == -1;

      ALELIB_CHECK(err, "can not link same dofs", std::runtime_error);

      if (dofs2[j] < dofs1[j])
        std::swap(dofs2[j], dofs1[j]);
    }

    index_t k = 0;
    for (unsigned i = 0; i < m_vars.size(); ++i)
    {
      for (index_t j = 0; j < (index_t)m_vars[i].m_verts_dofs.size(); ++j)
      {
        if (m_vars[i].m_verts_dofs.access(j) >= 0)
          data.at(k++) = & m_vars[i].m_verts_dofs.access(j);
      }

      for (index_t j = 0; j < (index_t)m_vars[i].m_ridges_dofs.size(); ++j)
      {
        if (m_vars[i].m_ridges_dofs.access(j) >= 0)
          data.at(k++) = & m_vars[i].m_ridges_dofs.access(j);
      }

      for (index_t j = 0; j < (index_t)m_vars[i].m_facets_dofs.size(); ++j)
      {
        if (m_vars[i].m_facets_dofs.access(j) >= 0)
          data.at(k++) = & m_vars[i].m_facets_dofs.access(j);
      }

      for (index_t j = 0; j < (index_t)m_vars[i].m_cells_dofs.size(); ++j)
      {
        if (m_vars[i].m_cells_dofs.access(j) >= 0)
          data.at(k++) = & m_vars[i].m_cells_dofs.access(j);
      }

    }

    std::sort(data.begin(), data.end(), AuxRemoveGaps::compare_ptr);

    index_t sub = 0;
    for (index_t i = 0; i < (index_t)data.size(); ++i)
    {
      for (index_t j = 0; j < size; ++j)
      {
        if (*data[i] == dofs2[j])
        {
          ++sub;
          *data[i] = *data[ dofs1[j] ];
          ++m_n_links;
          break;
        }
        if (j==size-1)
          *data[i] -= sub;
      }
    }

  }




};














} // namespace alelib


#endif















