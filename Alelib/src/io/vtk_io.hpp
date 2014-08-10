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

#ifndef ALELIB_MESHIOVTK_HPP
#define ALELIB_MESHIOVTK_HPP

#include <cstdio>
#include "../util/assert.hpp"
#include "../mesh/mesh.hpp"
#include "path_handle.hpp"
#include "../mesh/enums.hpp"


namespace alelib
{

class DefaultGetDataVtk
{
public:

  /**
   */  

  DefaultGetDataVtk(Real * r = NULL, int ncomps_ = 1) : data_r(r), ncomps(ncomps_) {}
  
  /// @param id id of the vertex or cell
  /// @param values the data
  virtual void getData(index_t id, Real *values) const
  {
    for (int i = 0; i < ncomps; ++i)
      *values++ = data_r[ncomps*id + i];
  }

  // This function is called when printing higher-order meshes.
  // The default behaviour is to printing zero at high-order nodes.
  /// @param x_local the coordinate in the cell reference from where data is extracted
  /// @param cell_id the cell from where data is extracted
  /// @param ith number of the correspondent Lagrange degree of freedom in the cell reference.
  /// @param[out] values the values of the data at `x_local'.
  virtual void getData(Real const *x_local, index_t cell_id, int ith, Real *values) const
  {
    for (int i = 0; i < ncomps; ++i)
      *values++ = 0;
    // the next lines are added to avoid compiler warnings:
    ++x_local;
    ++cell_id;
    ++ith;
  }

  virtual int numComps() const
  { return ncomps; }
  
  virtual ~DefaultGetDataVtk() {data_r = NULL;}
  
protected:
  Real * data_r;
  int    ncomps;
};


template<ECellType ECT>
class MeshIoVtk : public iPathHandle
{

protected:
  bool m_is_binary;
  int  m_filenum;
  int  m_add_node_scalar_n_calls;
  int  m_add_cell_scalar_n_calls;
  int  m_spacedim;
  Mesh<ECT> const* m_mesh;
  int m_subdivs_lvl;  // < 2 means do nothing, otherwise means the number of parts that an edge is divided
  std::vector<int> m_subcells;
  std::vector<Real> m_ref_cpts;
  std::vector<Real> m_ref_fpts;

public:

  typedef Mesh<ECT> MeshT;
  typedef MeshIoVtk<ECT> Self;
  typedef typename MeshT::VertexH VertexH;
  typedef typename MeshT::RidgeH RidgeH;
  typedef typename MeshT::FacetH FacetH;
  typedef typename MeshT::CellH CellH;
  
  
  static const ECellType CellType = ECT;

  MeshIoVtk() : m_is_binary(false), m_filenum(0), m_add_node_scalar_n_calls(0), m_add_cell_scalar_n_calls(0),
                m_spacedim(0), m_mesh(NULL)
  {
    splitMeshEdges(1);
  };

  explicit MeshIoVtk(MeshT const* mesh) : m_is_binary(false), m_filenum(0), m_add_node_scalar_n_calls(0), m_add_cell_scalar_n_calls(0)
  {
    attachMesh(mesh);
    splitMeshEdges(1);
  }

  MeshIoVtk(MeshIoVtk const&) {};

  void setBinaryOutput(bool b)
  {
    m_is_binary = b;
  }

  void attachMesh(MeshT const* mesh)
  {
    ALELIB_ASSERT(mesh != NULL, "invalid mesh", std::invalid_argument);
    m_mesh = mesh;
    m_spacedim = m_mesh->spaceDim();    
  }

  std::string outputFileName()
  {
    return this->paddedName(this->m_filenum, ".vtk");
  }

  void splitMeshEdges(unsigned n_parts);
  
  void writeMesh(double* time = NULL); // time = optional parameter
  void writeMesh(DefaultGetDataVtk const& data, double* time = NULL); // time = optional parameter
  void addNodalScalarField(const char* nome_var, DefaultGetDataVtk const& data);
  void addNodalVectorField(const char* nome_var, DefaultGetDataVtk const& data);
  void addCellScalarField(const char* nome_var, DefaultGetDataVtk const& data);
  void addNodeIntVtk(const char* nome_var, DefaultGetDataVtk const& data);
  void addCellIntVtk(const char* nome_var, DefaultGetDataVtk const& data);
  void printPointTagVtk(const char* nome_var="node_labels"); //  debug
  void printPointIcellVtk(const char* nome_var="pt_icell"); //  debug
  void printPointPositionVtk(const char* nome_var="pt_ic_pos"); //  debug
  void printCellIdVtk(const char* nome_var="cell_id");

protected:

  void fi_printPointVtk(double x[], FILE *fp) const;
  void fi_printCellVtk(index_t const* ids, FILE *fp) const;
  void fi_printNodeData(double x[], int ncomps, FILE *fp) const;

  static int getVtkTag(ECellType type);


};

}


#endif
