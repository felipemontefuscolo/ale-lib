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

#ifndef ALELIB_DOF_HANDLER_HPP
#define ALELIB_DOF_HANDLER_HPP

#include "../shapefunctions/shape_declarations.hpp"
#include "var_dof.hpp"
#include <vector>
#include <set>
#include <string>
#include "Array/array.hpp"
#include "contrib/Loki/set_vector.hpp"

class DofHandler
{
  typedef std::vector<int>                  DataContainer;
  typedef std::vector<VarDofs>              DofsContainer;
  typedef marray::Array<int,2>              MatrixBool;
  
  //// if the compilers complains, just comment these lines
  //DofHandler(DofHandler const& ) = default;
  //DofHandler& operator=(DofHandler const& ) = default;
  
public:  
  
  DofHandler(Mesh* mesh=NULL, float gf=0.07) : m_mesh_ptr(mesh), m_grow_factor(gf), m_n_links(0) {}
  
  // a safe copy
  void copy(DofHandler const& c);
  
  
  void setMesh(Mesh * mesh) {m_mesh_ptr=mesh;};
  
  /* Add a variable
   * @param var_name name of the variable
   * @param sf pointer to a ShapeFunction object. This object contains info about dofs.
   * @param ncomps number of components of the variable.
   * @param ntags number of different mesh elements tags which will be considered. 
   * @param tags only mesh elements with these tags will be considered.
   */ 
  void addVariable(const char* var_name, ShapeFunction *sf, int ncomps=1, int ntags=0, int const* tags=NULL);
  
  /*  Add a variable.
   * @param var_name name of the variable
   * @param ndpv number of dofs per vertice
   * @param ndpr number of dofs per corner (ignored for 1D and 2D cells)
   * @param ndpf number of dofs per facet (ignored for 1D cells)
   * @param ndpc number of dofs per cell
   * @param ntags number of different mesh elements tags which will be considered. 
   * @param tags only mesh elements with these tags will be considered.
   */ 
  void addVariable(const char* var_name, int ndpv, int ndpr, int ndpf, int ndpc, int ntags=0, int const* tags=NULL);
  VarDofs const& getVariable(int i) const {return m_vars[i];}
  VarDofs & getVariable(int i) {return m_vars[i];}
  
  void setVariablesRelationship(bool const* v);
  
  void printSparsityMatlab(std::string matname, std::string filename = "");
  void getSparsityTable(std::vector<SetVector<int> > & table);
  void getMetisCSRfromTable(int *, int *, std::vector<std::set<int> > const& table);
  void getCSR_adjacency(std::vector<int> &adjncy, std::vector<int> &xadj);

  void metisRenumber();
  void boostMinimumDegreeRenumber();
  void boostCuthillMcKeeRenumber();
  void CuthillMcKeeRenumber();
  
  ///*  Remove gaps in the dofs numbering */
  //void removeDofsGaps();

  /* @param deleted The deleted vertices
   * @param added The added vertices
   * @return A list of the deleted degree of freedoms
   * @note TODO
   */
  std::vector<int> updateVerticesDofs(std::vector<int> &deleted, std::vector<int> &added);
  
  // it uses the vector dofs1 and dofs2
  // for better performance, pass all dofs at once.
  void linkDofs(int size, int * dofs1, int * dofs2); // do dofs2 = dofs1
  
  
  int numVars() const {return m_vars.size();};
  int numDofs() const;
  int numPositiveDofs() const;
  void SetUp();
  
private:
  Mesh*         m_mesh_ptr;
  float         m_grow_factor;
  MatrixBool    m_relations;
  DofsContainer m_vars;
  int           m_n_links;
};



#endif
