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

#ifndef FEPIC_MESHIOVTK_HPP
#define FEPIC_MESHIOVTK_HPP

#include <cstdio>
#include "../../util/assert.hpp"
#include "../mesh.hpp"
#include "meshnamehandler.hpp"
#include "../enums.hpp"

class DefaultGetDataVtk
{
public:

  /**
   */  

  DefaultGetDataVtk(Real * r = NULL, int * i = NULL, Real * vec = NULL, int vsize = 2)
                                          : data_r(r), data_i(i), data_vec(vec), vec_size_(vsize) {}
  //DefaultGetDataVtk(int * i = NULL, Real * r = NULL) : data_r(r), data_i(i) {}
  
  virtual Real get_data_r(int id) const
  {
    return data_r[id];
  }
  virtual int get_data_i(int id) const
  {
    return data_i[id];
  }
  // returns vec_out size
  virtual void get_vec(int id, Real * vec_out) const
  {
    for (int i = 0; i < vec_size_; ++i)
      vec_out[i] = data_vec[id*vec_size_ + i];
  }
  virtual int vec_ncomps() const
  {
    return vec_size_;
  }
  
  virtual ~DefaultGetDataVtk() {}
  
protected:
  Real * data_r;
  int  * data_i;
  Real * data_vec;
  int    vec_size_;
};



class MeshIoVtk : public iPathHandle
{
  typedef void (MeshIoVtk::*CprinterMemFunPtr)(int const*, FILE*) const;
  typedef void (MeshIoVtk::*PprinterMemFunPtr)(Point const*, FILE*) const;

protected:
  bool m_is_binary;
  int  m_filenumVtk;
  int  m_add_node_scalar_vtk_n_calls;
  int  m_add_cell_scalar_vtk_n_calls;
  int  m_spacedim;
  ECellType m_fep_tag;
  Mesh const* m_mesh;
  CprinterMemFunPtr m_c_printer;
  //PprinterMemFunPtr m_p_printer;

public:

  MeshIoVtk() : m_is_binary(false), m_filenumVtk(0), m_add_node_scalar_vtk_n_calls(0), m_mesh(NULL) {};

  explicit MeshIoVtk(Mesh const* mesh, int filenum=0) : m_filenumVtk(filenum), m_add_node_scalar_vtk_n_calls(0)
  {
    attachMesh(mesh);
  }

  void setBinaryOutput(bool b)
  {
    m_is_binary = b;
  }

  MeshIoVtk(MeshIoVtk const&) {};

  void attachMesh(Mesh const* mesh);

  std::string outputFileName()
  {
    return this->fi_popNextName(this->m_filenumVtk, ".vtk");
  }
  
  void writeVtk(std::string outname = "");
  void addNodeScalarVtk(const char* nome_var, DefaultGetDataVtk const& data);
  void addNodeVectorVtk(const char* nome_var, DefaultGetDataVtk const& data);
  void addCellScalarVtk(const char* nome_var, DefaultGetDataVtk const& data);
  void addNodeIntVtk(const char* nome_var, DefaultGetDataVtk const& data);
  void addCellIntVtk(const char* nome_var, DefaultGetDataVtk const& data);
  void printPointTagVtk(const char* nome_var="node_labels"); //  debug
  void printPointIcellVtk(const char* nome_var="pt_icell"); //  debug
  void printPointPositionVtk(const char* nome_var="pt_ic_pos"); //  debug
  void printCellIdVtk(const char* nome_var="cell_id");

  void fi_printPointVtk(Point const* p, FILE *fp) const;

  void fi_printCellVtk_Edge2(int const* ids, FILE *fp) const;
  
  void fi_printCellVtk_Edge3(int const* ids, FILE *fp) const;

  void fi_printCellVtk_Triangle3(int const* ids, FILE *fp) const;

  void fi_printCellVtk_Triangle6(int const* ids, FILE *fp) const;

  void fi_printCellVtk_Quadrangle4(int const* ids, FILE *fp) const;

  void fi_printCellVtk_Quadrangle8(int const* ids, FILE *fp) const;

  void fi_printCellVtk_Quadrangle9(int const* ids, FILE *fp) const;

  void fi_printCellVtk_Tetrahedron4(int const* ids, FILE *fp) const;

  void fi_printCellVtk_Tetrahedron10(int const* ids, FILE *fp) const;

  void fi_printCellVtk_Hexahedron8(int const* ids, FILE *fp) const;

  void fi_printCellVtk_Hexahedron20(int const* ids, FILE *fp) const;

  void fi_printCellVtk_Hexahedron27(int const* ids, FILE *fp) const;


  static int getVtkTag(ECellType type);

  static int getNumDivisions(ECellType type);


};




#endif
