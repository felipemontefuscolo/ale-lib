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

#ifndef ALELIB_MSH_IO_HPP
#define ALELIB_MSH_IO_HPP

#include <tr1/memory>
#include <cstdio>
#include "path_handle.hpp"
#include "msh_tags.hpp"
#include "alelib_tags.hpp"
#include "Alelib/src/util/assert.hpp"
#include "Alelib/src/util/timer.hpp"
#include "Alelib/src/util/assert.hpp"
#include "Alelib/src/mesh/enums.hpp"
#include "Alelib/src/mesh/mesh.hpp"
#include "Alelib/src/dof_mapper/var_dof.hpp"
#include "Alelib/src/dof_mapper/reorder.hpp"

namespace alelib
{

template<typename Mesh_t>
class MeshIoMsh : public iPathHandle
{
public:

  typedef Mesh_t MeshT;
  typedef typename MeshT::VertexH VertexH;
  typedef typename MeshT::RidgeH RidgeH;
  typedef typename MeshT::FacetH FacetH;
  typedef typename MeshT::CellH CellH;

  typedef VarDofs<MeshT> VarT;

  static const ECellType CellType = MeshT::CellType;
  static const int SpaceDim = MeshT::SpaceDim;


  void readFile(const char* filename, MeshT * mesh)
  {
    mesh->clear();

    ALELIB_ASSERT(mesh, "invalid mesh pointer", std::invalid_argument);

    //this->fi_registerFile(filename, ".msh");

    FILE * file_ptr = fopen(filename, "r");

    ALELIB_ASSERT(file_ptr, "could not open mesh file", std::invalid_argument);

    double  coord[3];
    int     type_tag;
    char    buffer[256];
    //std::tr1::shared_ptr<Point> p_ptr(new Point());
    //std::tr1::shared_ptr<Cell> c_ptr(mesh->createCell());

  //  int const spacedim = MeshT::cell_dim;

    long    nodes_file_pos;  // $Nodes
    long    elems_file_pos;  // $Elements

    // nós
    nodes_file_pos = find_keyword("$Nodes", 6, file_ptr);
    //nodes_file_pos++;  // only to avoid gcc warning: variable ‘nodes_file_pos’ set but not used [-Wunused-but-set-variable]

    ALELIB_ASSERT(nodes_file_pos>0, "invalid file format", std::invalid_argument);

    index_t num_pts(0);
    index_t node_number(0);
    if ( EOF == fscanf(file_ptr, "%d", &num_pts) )
      ALELIB_ASSERT(false, "invalid msh format", std::runtime_error);

    mesh->reserveVerts(num_pts);

    int max_valency_est;
    switch (CellType)
    {
      case EDGE:        max_valency_est = 2 ;  break;
      case TRIANGLE:    max_valency_est = 6 ;  break;
      case QUADRANGLE:  max_valency_est = 4 ;  break;
      case TETRAHEDRON: max_valency_est = 24;  break;
      case HEXAHEDRON:  max_valency_est = 8 ;  break;
      default: ALELIB_ASSERT(false, "invalid cell type", std::runtime_error);
    }
    max_valency_est = max_valency_est + max_valency_est/3 - 1;

    if (NULL == fgets(buffer, sizeof(buffer), file_ptr)) // escapa do \n
      ALELIB_ASSERT(false, "invalid msh format", std::runtime_error);
    for (index_t i=0; i< num_pts; ++i)
    {
      if ( NULL == fgets(buffer, sizeof(buffer), file_ptr) )
        ALELIB_ASSERT(false, "invalid msh format", std::runtime_error);
      sscanf(buffer, "%d %lf %lf %lf", &node_number, &coord[0], &coord[1], &coord[2]);
      ALELIB_ASSERT(node_number==i+1, "wrong file format", std::invalid_argument);

      VertexH v = mesh->addVertex(coord, 0);
      v.reserve(mesh, max_valency_est);
    }
    // os pontos não estão completas: falta atribuir os labels



    // contagem de elementos e alocação

    elems_file_pos = find_keyword("$Elements", 9, file_ptr);

    ALELIB_ASSERT(elems_file_pos>0, "invalid file format", std::invalid_argument);

    index_t num_cells=0;
    index_t num_elms;

    if (EOF == fscanf(file_ptr, "%d", &num_elms) )
      ALELIB_ASSERT(false, "invalid msh format", std::runtime_error);


    int nodes_per_cell;
    int msh_cell_type;

    // check mesh cell type and order
    bool wrong_file_err=true;
    index_t  elem_number;               // contagem para verificação de erros.
    for (index_t k = 0; k < num_elms; ++k)
    {

      if (EOF == fscanf(file_ptr, "%d %d", &elem_number, &type_tag) )
        ALELIB_ASSERT(false, "invalid msh format", std::runtime_error);

      // check sequence
      if (elem_number != k+1)
      {
        wrong_file_err=true;
        break;
      }

      ECellType ct;
      int deg;
      int cdim;

      mshTypeAndOrder(EMshTag(type_tag), ct, deg, cdim); // output are ct and deg
      
      // check type
      if (cdim == MeshT::cell_dim)
      {
        ALELIB_ASSERT(ct == CellType, "meshes with mixed elements are not supported", std::invalid_argument);
        wrong_file_err=false;
        ++num_cells;
        msh_cell_type = type_tag;
      }

      if (!fgets(buffer, sizeof(buffer), file_ptr) || feof(file_ptr))
      {
        wrong_file_err=true;
        break;
      }

    }
    ALELIB_ASSERT(!wrong_file_err, "Wrong file format. Make sure you created the mesh with correct file format. ", std::invalid_argument);

    nodes_per_cell = numNodeForMshTag(EMshTag(msh_cell_type));

    mesh->reserveCells(num_cells*1.1);
    mesh->reserveFacets( MeshT::estimateNumFacets(num_cells)*1.1  );
    mesh->reserveRidges( MeshT::estimateNumRidges(num_cells)*1.1  );

    /* --------------------------------------
     * Lendo as células
     * -------------------------------------- */
    fseek (file_ptr , elems_file_pos , SEEK_SET );
    if (EOF == fscanf(file_ptr, "%d", &num_elms) )
      ALELIB_ASSERT(false, "invalid msh format", std::runtime_error);

    this->timer.restart();

    index_t   inc(0);
    VertexH   c_verts[20];
    index_t   id_aux;
    int       numm_tags;
    int       elm_dim;
    int       physical;
    int const cell_dim = MeshT::cell_dim;
    for (index_t k=0; k < num_elms; ++k)
    {
      if ( EOF == fscanf(file_ptr, "%d %d %d %d", &elem_number, &type_tag, &numm_tags, &physical) )
        ALELIB_ASSERT(false, "invalid msh format", std::runtime_error);

      // sincronização
      ALELIB_ASSERT(elem_number==k+1, "invalid file format", std::invalid_argument);

      for (int j=1; j<numm_tags; ++j)
      {
        if ( EOF == fscanf(file_ptr, "%s", buffer) )
          ALELIB_ASSERT(false, "invalid msh format", std::runtime_error);
      }

      elm_dim = dimForMshTag(EMshTag(type_tag));

      if (elm_dim==0)
      {
        if ( EOF == fscanf(file_ptr, "%d", &id_aux) )
          ALELIB_ASSERT(false, "invalid msh format", std::runtime_error);
        --id_aux;
        VertexH(id_aux).setTag(mesh, physical);
      }
      else if (elm_dim == cell_dim)
      {
        ++inc;
        for (int i=0; i< MeshT::verts_per_cell; ++i)
        {
          if ( EOF == fscanf(file_ptr, "%d", &id_aux) )
            ALELIB_ASSERT(false, "invalid msh format", std::runtime_error);
          c_verts[i] = VertexH(--id_aux);
        }
        // ignore high order nodes
        for (int i = MeshT::verts_per_cell; i < nodes_per_cell; ++i)
          if ( EOF == fscanf(file_ptr, "%d", &id_aux) )
            ALELIB_ASSERT(false, "invalid msh format", std::runtime_error);
        
        CellH c = mesh->addCell(c_verts);
        c.setTag(mesh, physical);
      }
      else
      {
        if ( NULL == fgets(buffer, sizeof(buffer), file_ptr) )
          ALELIB_ASSERT(false, "invalid msh format", std::runtime_error);
      }
    }// end for k


    this->timer.elapsed("readFileMsh(): read connectivity");
    // até aqui, apenas foi lido a conectividade
    //





    /*
    ___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|__
    _|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___| */

    this->timer.restart();
    /* Procura por elementos no contorno que coincidam com as facets. Quando encontrados,
     * os labels são propagados para as respectivas facet que por sua vez propagam os
     * labels para seus nós. Os labels só são propagados se eles não foram definidos
     * nos nós anteriormente.
     */
    fseek (file_ptr , elems_file_pos , SEEK_SET );
    fscanf(file_ptr, "%d", &num_elms);


    std::vector<VertexH> nodes(MeshT::verts_per_facet);    // facet nodes
    std::vector<VertexH> bnodes(MeshT::verts_per_ridge);  // corner nodes

    for (int k=0; k < num_elms; ++k)
    {

      fscanf(file_ptr, "%d %d %d %d", &elem_number, &type_tag, &numm_tags, &physical);

      //// sincronização
      //ALELIB_ASSERT(elem_number==k+1, "invalid file format", std::invalid_argument);

      for (int j=1; j<numm_tags; ++j)
      {
        fscanf(file_ptr, "%s", buffer);
      }


      elm_dim = dimForMshTag(EMshTag(type_tag));

      if ((elm_dim == 0) && (cell_dim!=2))
      {
        fscanf(file_ptr, "%d", &id_aux);
      }
      else if (elm_dim == cell_dim-1) // FACETS
      {
        for (int i=0; i<MeshT::verts_per_facet; ++i)
        {
          fscanf(file_ptr, "%d", &id_aux);
          VertexH v(--id_aux);
          if (v.tag(mesh) == 0)
            v.setTag(mesh, physical);
          nodes[i] = v;
        }
        // ignore high order nodes
        int const n_nodes = numNodeForMshTag(EMshTag(type_tag));
        for (int i = MeshT::verts_per_facet; i < n_nodes; ++i)
          fscanf(file_ptr, "%d", &id_aux);
        
        if (cell_dim > 1)
        {
          FacetH f = mesh->getFacetFromVertices(nodes.data());
          if (!f.isNull())
            f.setTag(mesh, physical); //std::cout << (++TESTE) << std::endl;
          else
          {
            printf("WARNING: INVALID FACET IN INPUT MESH! vtcs: ");
            for (int zz = 0; zz < MeshT::verts_per_facet; ++zz)
              printf("%d ", nodes[zz].id(mesh));
            printf("\n");
          }
        }
      }
      else if (elm_dim == cell_dim-2) // RIDGES
      {
        for (int i=0; i<MeshT::verts_per_ridge; ++i)
        {
          fscanf(file_ptr, "%d", &id_aux);
          VertexH v(--id_aux);
          if (v.tag(mesh) == 0)
            v.setTag(mesh, physical);
          bnodes[i] = v;
        }
        // ignore high order nodes
        int const n_nodes = numNodeForMshTag(EMshTag(type_tag));
        for (int i = MeshT::verts_per_ridge; i < n_nodes; ++i)
          fscanf(file_ptr, "%d", &id_aux);        
        if (cell_dim>2)
        {
          RidgeH r = mesh->getRidgeFromVertices(bnodes.data());
          if (!r.isNull())
            r.setTag(mesh, physical);
          else
            printf("WARNING: INVALID CORNER IN INPUT MESH!\n");
        }
      }
      else
      {
        for (int i=0; i<MeshT::verts_per_cell; ++i)
        {
          fscanf(file_ptr, "%d", &id_aux);
          VertexH v(--id_aux);

          if (v.tag(mesh) == 0)
            v.setTag(mesh, physical);
        }
        // ignore high order nodes
        int const n_nodes = numNodeForMshTag(EMshTag(type_tag));
        for (int i = MeshT::verts_per_cell; i < n_nodes; ++i)
          fscanf(file_ptr, "%d", &id_aux);            
      }


    }

    this->timer.elapsed("readFileMsh(): search for boundary elements");

    if (MeshT::cell_dim>2)
    {
      RidgeH f_ridges[MeshT::ridges_per_facet + !MeshT::ridges_per_facet];

      FacetH f = mesh->facetBegin();
      FacetH const f_end = mesh->facetEnd();

      for (; f != f_end; ++f)
      {
        // the is no disabled facets

        f.ridges(mesh, f_ridges);

        for (int i = 0; i < MeshT::ridges_per_facet; ++i)
        {
          if (f_ridges[i].tag(mesh) == 0)
            f_ridges[i].setTag(mesh, f.tag(mesh));
        }

      }
    }

    if (MeshT::cell_dim>1)
    {
      FacetH c_facets[MeshT::facets_per_cell];
      RidgeH c_ridges[MeshT::ridges_per_cell + !MeshT::ridges_per_cell];

      CellH c = mesh->cellBegin();
      CellH const c_end = mesh->cellEnd();

      for (; c != c_end; ++c)
      {
        // the is no disabled cells

        c.facets(mesh, c_facets);

        for (int i = 0; i < MeshT::facets_per_cell; ++i)
        {
          if (c_facets[i].tag(mesh) == 0)
            c_facets[i].setTag(mesh, c.tag(mesh));
        }

        if (MeshT::cell_dim>2)
        {
          c.ridges(mesh, c_ridges);

          for (int i = 0; i < MeshT::ridges_per_cell; ++i)
          {
            if (c_ridges[i].tag(mesh) == 0)
              c_ridges[i].setTag(mesh, c.tag(mesh));
          }
        }
      }
    } // endif


    mesh->timer.addItems(this->timer);

    fclose(file_ptr);

    mesh->removeUnrefVertices();
  }

  // read coordinates from a file
  // it assumes that the space dimension of the mesh is the same of the `coords'
  void readCoordinates(const char* filename, MeshT const* mesh, Real* coords, VarT const& var)
  {
    ALELIB_ASSERT(mesh, "invalid mesh pointer", std::invalid_argument);

    //this->fi_registerFile(filename, ".msh");

    FILE * file_ptr = fopen(filename, "r");

    ALELIB_ASSERT(file_ptr, "could not open mesh file", std::invalid_argument);

    double  coord[3];
    int     type_tag;
    char    buffer[256];
    //std::tr1::shared_ptr<Point> p_ptr(new Point());
    //std::tr1::shared_ptr<Cell> c_ptr(mesh->createCell());

  //  int const spacedim = MeshT::cell_dim;

    long    nodes_file_pos;  // $Nodes
    long    elems_file_pos;  // $Elements

    // nós
    nodes_file_pos = find_keyword("$Nodes", 6, file_ptr);
    //nodes_file_pos++;  // only to avoid gcc warning: variable ‘nodes_file_pos’ set but not used [-Wunused-but-set-variable]

    ALELIB_ASSERT(nodes_file_pos>0, "invalid file format", std::invalid_argument);

    index_t num_pts(0);
    index_t node_number(0);
    if ( EOF == fscanf(file_ptr, "%d", &num_pts) )
      ALELIB_ASSERT(false, "invalid msh format", std::runtime_error);

    if (NULL == fgets(buffer, sizeof(buffer), file_ptr)) // escapa do \n
      ALELIB_ASSERT(false, "invalid msh format", std::runtime_error);
    
    std::vector<Real> file_coords(3*num_pts);
    
    for (index_t i=0; i< num_pts; ++i)
    {
      if ( NULL == fgets(buffer, sizeof(buffer), file_ptr) )
        ALELIB_ASSERT(false, "invalid msh format", std::runtime_error);
      sscanf(buffer, "%d %lf %lf %lf", &node_number, &coord[0], &coord[1], &coord[2]);
      file_coords[3*i + 0] = coord[0];
      file_coords[3*i + 1] = coord[1];
      file_coords[3*i + 2] = coord[2];
      ALELIB_ASSERT(node_number==i+1, "wrong file format", std::invalid_argument);
    }


    elems_file_pos = find_keyword("$Elements", 9, file_ptr);

    ALELIB_ASSERT(elems_file_pos>0, "invalid file format", std::invalid_argument);

    index_t num_cells=0;
    index_t num_elms;

    if (EOF == fscanf(file_ptr, "%d", &num_elms) )
      ALELIB_ASSERT(false, "invalid msh format", std::runtime_error);


    // detect expected degree
    int const cell_dim = CTypeDim(CellType);
    int expected_deg;
    if (cell_dim == 1)
      expected_deg = var.numDofsInCell()/SpaceDim + 1;
    else if(cell_dim == 2)
      expected_deg = var.numDofsInFacet()/SpaceDim + 1;
    else
      expected_deg = var.numDofsInRidge()/SpaceDim + 1;

    int nodes_per_cell;
    int msh_cell_type;

    // check mesh cell type and order
    bool wrong_file_err=true;
    index_t  elem_number;               // contagem para verificação de erros.
    for (index_t k = 0; k < num_elms; ++k)
    {

      if (EOF == fscanf(file_ptr, "%d %d", &elem_number, &type_tag) )
        ALELIB_ASSERT(false, "invalid msh format", std::runtime_error);

      // check sequence
      if (elem_number != k+1)
      {
        wrong_file_err=true;
        break;
      }

      ECellType ct;
      int deg;
      int cdim;

      mshTypeAndOrder(EMshTag(type_tag), ct, deg, cdim);

      // check type
      if (cdim == MeshT::cell_dim)
      {
        ALELIB_ASSERT(ct == CellType, "meshes with mixed elements are not supported", std::invalid_argument);
        ALELIB_ASSERT(expected_deg == deg, "current mesh order and file mesh order are incompatible", std::invalid_argument);
        wrong_file_err=false;
        ++num_cells;
        msh_cell_type = type_tag;
      }

      if (!fgets(buffer, sizeof(buffer), file_ptr) || feof(file_ptr))
      {
        wrong_file_err=true;
        break;
      }

    }
    ALELIB_ASSERT(!wrong_file_err, "Wrong file format. Make sure you created the mesh with correct file format. ", std::invalid_argument);
    
    nodes_per_cell = numNodeForMshTag(EMshTag(msh_cell_type));


    
    // copy coordinates from file_coords to the coords
    fseek (file_ptr , elems_file_pos , SEEK_SET );
    if (EOF == fscanf(file_ptr, "%d", &num_elms) )
      ALELIB_ASSERT(false, "invalid msh format", std::runtime_error);

    
    std::vector<index_t> x_dofs(nodes_per_cell*SpaceDim);
    
    index_t   id_aux;
    int       numm_tags;
    int       elm_dim;
    int       physical;
    //int const cell_msh_tag = CType2mshTag(MeshT::CellType);
    index_t   cell_id = 0;
    for (index_t k=0; k < num_elms; ++k)
    {
      if ( EOF == fscanf(file_ptr, "%d %d %d %d", &elem_number, &type_tag, &numm_tags, &physical) )
        ALELIB_ASSERT(false, "invalid msh format", std::runtime_error);

      // synchronization
      ALELIB_ASSERT(elem_number==k+1, "invalid file format", std::invalid_argument);

      for (int j=1; j<numm_tags; ++j)
      {
        if ( EOF == fscanf(file_ptr, "%s", buffer) )
          ALELIB_ASSERT(false, "invalid msh format", std::runtime_error);
      }

      elm_dim = dimForMshTag(EMshTag(type_tag));

      if (elm_dim == cell_dim)
      {
        var.getCellDofs(x_dofs.data(), CellH(cell_id));
        reorderDofsLagrange<MeshT,index_t>(mesh, CellH(cell_id), expected_deg, SpaceDim, x_dofs.data());
        for (int i=0; i< nodes_per_cell; ++i)
        {
          if ( EOF == fscanf(file_ptr, "%d", &id_aux) )
            ALELIB_ASSERT(false, "invalid msh format", std::runtime_error);
          --id_aux; // converting to C numbering
          for (int k = 0; k < SpaceDim; ++k)
            coords[x_dofs[i*SpaceDim + k]] = file_coords[id_aux*3 + k];
          
          //std::cout << "(" << x_dofs[i*SpaceDim] << " " << x_dofs[i*SpaceDim+1] << "), ";
        }
        //std::cout << std::endl;
        ++cell_id;
      }
      
    }// end for k

    fclose(file_ptr);

  }





  ECellType identifiesMeshType(const char* filename, int* space_dim_ = NULL) const
  {

    FILE * file_ptr = fopen(filename, "r");

    ALELIB_ASSERT(file_ptr, "can not find mesh file", std::invalid_argument);

    double  coord[3];
    int     type_tag;
    char    buffer[256];

    long    nodes_file_pos;  // $Nodes
    long    elems_file_pos;  // $Elements

    if (space_dim_ != NULL)
    {
      int& space_dim = *space_dim_;
      // nós
      nodes_file_pos = find_keyword("$Nodes", 6, file_ptr);
      //nodes_file_pos++;  // only to avoid gcc warning: variable ‘nodes_file_pos’ set but not used [-Wunused-but-set-variable]

      ALELIB_ASSERT(nodes_file_pos>0, "invalid file format", std::invalid_argument);

      space_dim = 1;

      int num_pts(0);
      int node_number(0);
      fscanf(file_ptr, "%d", &num_pts);

      fgets(buffer, sizeof(buffer), file_ptr); // escapa do \n
      for (int i=0; i< num_pts; ++i)
      {
        fgets(buffer, sizeof(buffer), file_ptr);
        sscanf(buffer, "%d %lf %lf %lf", &node_number, &coord[0], &coord[1], &coord[2]);
        ALELIB_ASSERT(node_number==i+1, "wrong file format", std::invalid_argument);
        if (coord[1] != 0)
          space_dim = 2;
        if (coord[2] != 0)
        {
          space_dim = 3;
          break;
        }
      }
    }

    // contagem de elementos e alocação
    elems_file_pos = find_keyword("$Elements", 9, file_ptr);

    ALELIB_ASSERT(elems_file_pos>0, "invalid file format", std::invalid_argument);

  //  int num_cells=0;
    int num_elms;

    fscanf(file_ptr, "%d", &num_elms);

    /* ---------------------------------------
     * Detectando a ordem da malha, verificando sequencia dos elementos,
     * e contando o número de células.
     * --------------------------------------- */
  //  bool wrong_file_err=true;
    int  elem_number, elm_dim, numm_tags, physical;
    int current_elm_dim = 0;
    ECellType current_elm_type = UNDEFINED_CELLT;
    fgets(buffer, sizeof(buffer), file_ptr); // escapa do \n
    for (int k=0; k < num_elms; ++k)
    {

      fscanf(file_ptr, "%d %d %d %d", &elem_number, &type_tag, &numm_tags, &physical);

      //// sincronização
      ALELIB_ASSERT(elem_number==k+1, "invalid file format", std::invalid_argument);

      for (int j=1; j<numm_tags; ++j)
      {
        fscanf(file_ptr, "%s", buffer);
      }

      elm_dim = dimForMshTag(EMshTag(type_tag));

      if(elm_dim > current_elm_dim)
      {
        current_elm_type = mshTag2CType(EMshTag(type_tag));
        current_elm_dim = elm_dim;
        if (elm_dim==3)
          break;
      }

      fgets(buffer, sizeof(buffer), file_ptr);

    }

    fclose(file_ptr);

    return current_elm_type;

  }


  Timer timer;
};



}

#endif




