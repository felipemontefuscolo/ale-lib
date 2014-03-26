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

#include <iostream>
#include "../util/assert.hpp"
#include "dof_handler.hpp"
#include "../mesh/mesh.hpp"
#include <cstdio>
#include "../util/cuthil_mckee.hpp"

#ifdef FEP_HAS_BOOST
#  include <boost/config.hpp>
#  include "boost/graph/adjacency_list.hpp"
#  include "boost/graph/graph_utility.hpp"
#  include "boost/graph/minimumm_degreem_ordering.hpp"
#  include <boost/graph/cuthill_mckeem_ordering.hpp>
#  include <boost/graph/properties.hpp>
#  include <boost/graph/bandwidth.hpp>
#endif

#ifdef FEP_HAS_METIS
extern "C" {
	#include <metis.h>
};
#endif


// a safe copy
// NOTE: this function can not depends on the mesh
void DofHandler::copy(DofHandler const& c)
{
  *this = c;
}


void DofHandler::addVariable(const char* var_name, ShapeFunction *sf, int ncomps, int ntags, int const* tags)
{
  addVariable(var_name, sf->numDofsAssociatedToVertice()*ncomps,
                        sf->numDofsAssociatedToCorner()*ncomps,
                        sf->numDofsAssociatedToFacet()*ncomps,
                        sf->numDofsAssociatedToCell()*ncomps,
                        ntags, tags);
}


void DofHandler::addVariable(const char* var_name, int ndpv, int ndpr, int ndpf, int ndpc, int ntags, int const* tags)
{
  ALELIB_CHECK(m_mesh_ptr!=NULL, "mesh is NULL", std::invalid_argument);
  ALELIB_CHECK(m_mesh_ptr->numNodesTotal()>0, "did you forget to setup the mesh?", std::invalid_argument);
  
  m_vars.push_back(VarDofs(var_name,m_mesh_ptr,ndpv,ndpr,ndpf,ndpc, ntags,tags));
}

void DofHandler::SetUp()
{
  if (m_relations.size() < 1)
  {
    m_relations.reshape(numVars(), numVars());
    for (int i = 0; i < numVars()*numVars(); ++i)
      m_relations[i] = 1;
  }
  
  int  initial_dof=0;
  for (unsigned i = 0; i < m_vars.size(); ++i)
  {
    m_vars[i].setUp(initial_dof);
    initial_dof += m_vars[i].numPositiveDofs();
  }
  
}

int DofHandler::numDofs() const
{
  int total = 0;
  for (unsigned i = 0; i < m_vars.size(); ++i) 
    total += m_vars[i].numPositiveDofs();
  
  
  return total  - m_n_links;
  
}

int DofHandler::numPositiveDofs() const
{
  int total = 0;
  for (unsigned i = 0; i < m_vars.size(); ++i) 
    total += m_vars[i].numPositiveDofs();
  
  return total;
  
}


/* a matrix of bools with size numVars() x numVars()
 */ 
void DofHandler::setVariablesRelationship(bool const* v)
{
  m_relations.reshape(numVars(), numVars());
  for (int i = 0; i < numVars(); ++i)
  {
    for (int j = 0; j < numVars(); ++j)
    {
      m_relations(i,j) = v[i*numVars() + j];
    }
  }
}


void DofHandler::printSparsityMatlab(std::string matname, std::string filename)
{
  
  typedef std::vector<SetVector<int> > TableT;
  typedef SetVector<int>::const_iterator SetIter;

  //const int n_vars = numVars();
  const int n_dofs = numDofs();

  TableT table; // dofs x neighbors
  getSparsityTable(table);
  
  int nnz = 0;
  FEP_PRAGMA_OMP(parallel)
  {
    int nnz_local=0;
    FEP_PRAGMA_OMP(for)
    for (int i = 0; i < n_dofs; ++i)
      nnz_local += table[i].size();
    FEP_PRAGMA_OMP(critical)
      nnz += nnz_local;
  }
  
  if (filename == "")
  filename = matname + ".m";
  FILE* fp = fopen(filename.c_str(), "w");
  if (fp==NULL)
    printf("ERROR: printSparsityMatlab\n");
  
  fprintf(fp, "%% Size = %d %d\n", n_dofs, n_dofs);
  fprintf(fp, "%% Nonzeros = %d\n", nnz);
  fprintf(fp, "zxzxzx = zeros(%d, 3);\n", nnz);
  fprintf(fp, "zxzxzx = [\n");
  
  SetIter sit, sit_end;
  for (int i = 0; i < n_dofs; ++i)
  {
    sit = table[i].begin();
    sit_end = table[i].end();
    for (; sit != sit_end; ++sit)
    {
      fprintf(fp, "%d %d %d\n", i+1, (*sit)+1, 1);
    }
  }
  
  fprintf(fp, "];\n");
  fprintf(fp, "%s = spconvert(zxzxzx);\n", matname.c_str());
  fprintf(fp, "clear zxzxzx;\n");
  
  fclose(fp);
}



// automatic resize
void DofHandler::getSparsityTable(std::vector<SetVector<int> > & table)
{
  typedef std::vector<SetVector<int> > TableT;
  //typedef std::set<int>::const_iterator SetIter;
  
  const int n_vars = numVars();
  const int n_dofs = numDofs();
  //const int tsize  = totalSize();
  std::vector<std::vector<int> > var_cell_dofs(n_vars); // var x cell dofs
  
  int estimate = 0;
  for (int i = 0; i < n_vars; ++i)
  {
    int const n_dofs_cell = getVariable(i).numDofsPerCell();
    var_cell_dofs[i].resize( n_dofs_cell );
    estimate += n_dofs_cell;
  }

  table.resize(n_dofs);
  if (m_mesh_ptr->cellDim() < 3)
    estimate *= 3;
  else
    estimate *= 16;
  for (unsigned i = 0; i < table.size(); ++i)
    table[i].reserve(estimate);
  
  cell_iterator cell = m_mesh_ptr->cellBegin();
  cell_iterator cell_end = m_mesh_ptr->cellEnd();  
  // compute connectivity
  for (; cell != cell_end; ++cell)
  {
    // gets dofs of this cell
    for (int i = 0; i < n_vars; ++i)
      getVariable(i).getCellDofs(var_cell_dofs[i].data(), &*cell);
      
    // connectivity
    for (int i = 0; i < n_vars; ++i)
    {
      const int n_dof_p_cell_i = var_cell_dofs[i].size();
      
      for (int j = 0; j < n_vars; ++j)
      {
        if (!m_relations(i,j))
          continue;
        const int n_dof_p_cell_j = var_cell_dofs[j].size();
        
        for (int m = 0; m < n_dof_p_cell_i; ++m)
        {
          int const dof_i = var_cell_dofs[i][m];
          
          for (int n = 0; n < n_dof_p_cell_j; ++n)
          {
            int const dof_j = var_cell_dofs[j][n];
            table.at(dof_i).insert(dof_j);
          }
        }
      }
    }
  }
  
  //printf("DEBUG TABLE SIZE %d\n", table.size());
  //for (int i = 0; i < table.size(); ++i)
  //{
  //  SetIter it = table[i].begin();
  //  printf("DEBUG ===== ");
  //  for (; it != table[i].end(); ++it)
  //  {
  //    printf("%d ", *it);
  //  }
  //  printf("\n");
  //}
  
}


// size of adjncy = nnz-numDofs, because there is not interaction ith-ith
// size of xadj = numDofs+1 (or table.size()+1)
//
void DofHandler::getMetisCSRfromTable(int* adjncy, int* xadj, std::vector<std::set<int> > const& table)
{
  typedef std::vector<std::set<int> > TableT;
  typedef std::set<int>::const_iterator SetIter; 
  
  int const n_dofs = table.size();
  
  SetIter it;
  SetIter it_end;
  *xadj = 0;
  //int counter=0;
  for (int i = 0; i < n_dofs; ++i)
  {
    it = table[i].begin();
    it_end = table[i].end();
    for (; it!= it_end; ++it)
      if (*it != i)
      {
        *adjncy++ = *it;
        //printf("DEBUG >>> %d\n", counter++);
      }
    
    ++xadj;
    *xadj = *(xadj-1) + table[i].size()-1;
  }
  
}


void DofHandler::metisRenumber()
#ifdef FEP_HAS_METIS
{
  const int n_dofs = numDofs();

  std::vector<int> xadj(n_dofs+1);
  std::vector<int> adjncy;

  {
    typedef std::vector<std::set<int> > TableT;
    typedef std::set<int>::const_iterator SetIter;
  
    TableT table; // dofs x neighbors
    getSparsityTable(table);

    int nnz = 0;
    FEP_PRAGMA_OMP(parallel)
    {
      int nnz_local=0;
      FEP_PRAGMA_OMP(for)
      for (int i = 0; i < n_dofs; ++i)
        nnz_local += table[i].size();
      FEP_PRAGMA_OMP(critical)
        nnz += nnz_local;
    }
    
    int diags=0;
    for (int i = 0; i < numVars(); ++i)
      diags += getVariable(i).numDofs() * m_relations(i,i);
    adjncy.resize(nnz-diags);
    
    getMetisCSRfromTable(adjncy.data(), xadj.data(), table);
    //printf("DEBUG NNZ-n_dofs  %d\n", nnz-diags);
  } 
  std::vector<int> perm(n_dofs);
  std::vector<int> iperm(n_dofs);  
  int options[METIS_NOPTIONS];
  
  int temp = n_dofs;
  METIS_SetDefaultOptions(options);
  
  //options[METIS_OPTION_DBGLVL] = METIS_DBG_SEPINFO | METIS_DBG_CONNINFO;
  //options[METIS_OPTION_RTYPE] = METIS_RTYPE_SEP2SIDED ;
	METIS_NodeND(&temp, xadj.data(), adjncy.data(), NULL, options, perm.data(), iperm.data());
  
  //
  //for (int i = 0; i < adjncy.size(); ++i)
  //  printf("%d ", adjncy[i]);
  //printf("\n");
  
  const int tsize = totalSize();
  
  FEP_PRAGMA_OMP(parallel for)
  for (int k = 0; k < tsize; ++k)
  {
    if (m_data[k]==-1)
      continue;
    m_data[k] = perm[m_data[k]];
  }
}
#else
{}
#endif


void DofHandler::boostMinimumDegreeRenumber()
#ifdef FEP_HAS_BOOST
{
  typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS>  Graph;
  typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;

  const int n_dofs = numDofs();

  Graph G(n_dofs);

  int num_edge = 0;  

  {
    typedef std::vector<std::set<int> > TableT;
    typedef std::set<int>::const_iterator SetIter;
  
    TableT table; // dofs x neighbors
    getSparsityTable(table);

    SetIter jt, jt_end;
    for (int i = n_dofs-1; i >= 0; --i)
    {
      jt = table[i].begin();
      jt_end = table[i].end();
      
      for (; jt != jt_end; ++jt)
      {
        if ( *jt > i ) {
          add_edge(*jt, i, G);
          add_edge(i, *jt, G);
          num_edge++;
        }
      }
      
      table.pop_back();
    }
    
  } 

  std::vector<int> perm(n_dofs,0), iperm(n_dofs,0);

  std::vector<int> supernode_sizes(n_dofs, 1); // init has to be 1

  std::vector<int> degree(n_dofs, 0);

  boost::property_map<Graph, boost::vertex_index_t>::type 
    id = get(boost::vertex_index, G);

  int delta = 0;
  minimumm_degreem_ordering
    (G,
     make_iterator_property_map(&degree[0], id, degree[0]),
     &iperm[0],
     &perm[0],
     make_iterator_property_map(&supernode_sizes[0], id, supernode_sizes[0]), 
     delta, id);  

  const int tsize = totalSize();
  
  FEP_PRAGMA_OMP(parallel for)
  for (int k = 0; k < tsize; ++k)
  {
    if (m_data[k]==-1)
      continue;
    m_data[k] = perm[m_data[k]];
  }  
}
#else
{}
#endif



void DofHandler::boostCuthillMcKeeRenumber()
#ifdef FEP_HAS_BOOST
{
  typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, 
                          boost::property<boost::vertex_color_t, boost::default_color_type,
                          boost::property<boost::vertexm_degree_t,int> > >   Graph;
                          
  typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
  typedef boost::graph_traits<Graph>::vertices_size_type size_type;

  const int n_dofs = numDofs();

  Graph G(n_dofs);

  {
    typedef std::vector<std::set<int> > TableT;
    typedef std::set<int>::const_iterator SetIter;
  
    TableT table; // dofs x neighbors
    getSparsityTable(table);

    SetIter jt, jt_end;
    for (int i = n_dofs-1; i >= 0; --i)
    {
      jt = table[i].begin();
      jt_end = table[i].end();
      
      for (; jt != jt_end; ++jt)
      {
        if ( *jt > i ) {
          add_edge(i, *jt, G);
        }
      }
      table.pop_back();
    }
    
  }
  boost::property_map<Graph, boost::vertex_index_t>::type index_map = get(boost::vertex_index, G);
  
  std::vector<Vertex> iperm(boost::num_vertices(G));
  std::vector<int> perm(boost::num_vertices(G));
  //reverse cuthill_mckeem_ordering
  cuthill_mckeem_ordering(G, iperm.rbegin(), get(boost::vertex_color, G),
                                 boost::makem_degree_map(G));
  
  FEP_PRAGMA_OMP(parallel for)
  for (unsigned c = 0; c < iperm.size(); ++c)
     perm[index_map[iperm[c]]] = c;

  const int tsize = totalSize();
  
  FEP_PRAGMA_OMP(parallel for)
  for (int k = 0; k < tsize; ++k)
  {
    if (m_data[k]==-1)
      continue;
    m_data[k] = perm[m_data[k]];
  } 
     
}
#else
{}
#endif


// auto-resize vectors
void DofHandler::getCSR_adjacency(std::vector<int> &adjncy, std::vector<int> &xadj)
{
  const int n_dofs = numDofs();

  xadj.resize(n_dofs+1);
  std::vector<int> adjncy_temp;
  //(100*n_dofs)
  
  {
    typedef std::vector<SetVector<int> > TableT;
    typedef SetVector<int>::const_iterator SetIter;
  
    TableT table; // dofs x neighbors
    getSparsityTable(table);
    
    int nnz = 0;
    FEP_PRAGMA_OMP(parallel)
    {
      int nnz_local=0;
      FEP_PRAGMA_OMP(for)
      for (int i = 0; i < n_dofs; ++i)
        nnz_local += table[i].size();
      FEP_PRAGMA_OMP(critical)
        nnz += nnz_local;
    }
    
    adjncy_temp.resize(nnz);
    
    SetIter it;
    SetIter it_end;
    std::vector<int>::iterator xadj_it = xadj.begin();
    std::vector<int>::iterator adjncy_it = adjncy_temp.begin();
    *xadj_it = 0;
    for (int i = 0; i < n_dofs; ++i)
    {
      it = table[i].begin();
      it_end = table[i].end();
      for (; it!= it_end; ++it)
          *adjncy_it++ = *it;
      
      ++xadj_it;
      *xadj_it = *(xadj_it-1) + table[i].size()-1;
    }    
    
  }
  
  int const nnz = adjncy_temp.size();
  adjncy.resize(nnz); // to avoid fragmentation
  
  FEP_PRAGMA_OMP(parallel for)
  for (int i = 0; i < nnz; ++i)
    adjncy[i] = adjncy_temp[i];
  
}

void DofHandler::CuthillMcKeeRenumber()
{
//
//  const int n_dofs = numDofs();
//  std::vector<int> perm(n_dofs);
//  
//  {
//    typedef std::vector<std::set<int> > TableT;
//    
//    TableT table; // dofs x neighbors
//    getSparsityTable(table);  
//    
//    CuthilMckee()(table, 0, perm.data());
//  }
//  
//  const int tsize = totalSize();
//  
//  FEP_PRAGMA_OMP(parallel for)
//  for (int k = 0; k < tsize; ++k)
//  {
//    if (m_data[k]==-1)
//      continue;
//    m_data[k] = perm[m_data[k]];
//  }   
//  
}


class AuxRemoveGaps
{
public:
  static
  void doit(vector<int> &a)
  {
    vector<int*> b(a.size());

    const int N = a.size();

    vector<int>::iterator  ita;
    vector<int*>::iterator itb;

    for (int i = 0; i < N; ++i)
      b[i] = &a[i];

    sort(b.begin(), b.end(), compare_ptr);

    // removing gaps from b
    itb = b.begin();
    while(**itb < 0 && itb != b.end())
      ++itb;

    if (itb == b.end())
      return;

    vector<int*>::iterator itb1 = itb+1, itr;

    // removing ... initial step
    if (itb != b.end())
    {
      int diff = **itb - 0;
      if (diff > 0)
      {
        itr = itb;
        while(itr != b.end())
          **itr++ -= diff;        
      }
    }
    // removing
    while(itb1 != b.end())
    {
      int diff = **itb1 - **itb;
      if (diff > 1)
      {
        itr = itb1;
        while(itr != b.end())
          **itr++ -= diff - 1;
      }
      else
      {
        ++itb1;
        ++itb;
      }
    }
  } // end removeGaps


  static bool compare_ptr(int *a, int*b)
  {
    return *a < *b;
  }
};


//void DofHandler::removeDofsGaps()
//{
////  AuxRemoveGaps::doit(m_data);
//}

void DofHandler::linkDofs(int size, int * dofs1, int * dofs2)
{
  std::vector<int*> data(numPositiveDofs(),NULL);

  // is also check user's input
  for (int j = 0; j < size; ++j)
  {
    bool err = (dofs2[j] != dofs1[j]) || dofs2[j] == -1;
    
    ALELIB_CHECK(err, "can not link same dofs", std::runtime_error);
    
    if (dofs2[j] < dofs1[j])
      swap(dofs2[j], dofs1[j]);
  }

  int k = 0;
  for (unsigned i = 0; i < m_vars.size(); ++i)
  {
    for (int j = 0; j < m_vars[i].m_vertices_dofs.size(); ++j)
    {
      if (m_vars[i].m_vertices_dofs[j] >= 0)
        data.at(k++) = & m_vars[i].m_vertices_dofs[j];
    }
    
    for (int j = 0; j < m_vars[i].m_corners_dofs.size(); ++j)
    {
      if (m_vars[i].m_corners_dofs[j] >= 0)
        data.at(k++) = & m_vars[i].m_corners_dofs[j];
    }

    for (int j = 0; j < m_vars[i].m_facets_dofs.size(); ++j)
    {
      if (m_vars[i].m_facets_dofs[j] >= 0)
        data.at(k++) = & m_vars[i].m_facets_dofs[j];
    }
    
    for (int j = 0; j < m_vars[i].m_cells_dofs.size(); ++j)
    {
      if (m_vars[i].m_cells_dofs[j] >= 0)
        data.at(k++) = & m_vars[i].m_cells_dofs[j];
    }

  }

  std::sort(data.begin(), data.end(), AuxRemoveGaps::compare_ptr);


  int sub = 0;
  for (unsigned i = 0; i < data.size(); ++i)
  {
    for (int j = 0; j < size; ++j)
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





