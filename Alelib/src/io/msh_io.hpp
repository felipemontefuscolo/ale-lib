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


namespace alelib
{

template<typename Mesh_t>
class MeshIoMsh : public iPathHandle
{
  Mesh_t const* m_mesh;
public:

  typedef Mesh_t MeshT;
  typedef typename MeshT::VertexH VertexH;
  typedef typename MeshT::RidgeH RidgeH;
  typedef typename MeshT::FacetH FacetH;
  typedef typename MeshT::CellH CellH;

  typedef VarDofs<MeshT> VarT;

  static const ECellType CellType = MeshT::CellType;


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

    // ---------------------------------------
    // Detectando a ordem da malha, verificando sequencia dos elementos,
    // e contando o número de células.
    // ---------------------------------------
    const int mesh_cell_msh_tag = CType2mshTag(CellType);//mesh->cellMshTag();

    //if (mshTag2CType(EMshTag(mesh_cell_msh_tag)) != mesh->cellType())
    //{
    //  //ALELIB_ASSERT(false, "invalid msh format", std::runtime_error);
    //  printf("ERROR: CType2mshTag() = %d\n", CType2mshTag(mesh->cellType()));
    //  printf("ERROR: cell->getMshTag() = %d\n", c_ptr->getMshTag());
    //  throw;
    //}


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

      // check type
      if (type_tag == mesh_cell_msh_tag)
      {
        wrong_file_err=false;
        ++num_cells;
      }

      if (!fgets(buffer, sizeof(buffer), file_ptr) || feof(file_ptr))
      {
        wrong_file_err=true;
        break;
      }

    }
    ALELIB_ASSERT(!wrong_file_err, "Wrong file format. Make sure you created the mesh with correct file format. ", std::invalid_argument);



    /* --------------------------------------
     * Lendo as células
     * -------------------------------------- */
    fseek (file_ptr , elems_file_pos , SEEK_SET );
    if (EOF == fscanf(file_ptr, "%d", &num_elms) )
      ALELIB_ASSERT(false, "invalid msh format", std::runtime_error);

    this->timer.restart();

    index_t   inc(0);
    int       nodes_per_cell = MeshT::verts_per_cell;
    VertexH   c_verts[20];
    index_t   id_aux;
    int       numm_tags;
    int       elm_dim;
    int       physical;
    int const cell_dim = MeshT::cell_dim;
    int const cell_msh_tag = CType2mshTag(MeshT::CellType);
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
        //mesh->getNodePtr(id_aux)->setTag(physical);
        VertexH(id_aux).setTag(mesh, physical);
      }
      else if (elm_dim == cell_dim)
      {
        //cell = mesh->pushCell((int*)0);
        ++inc;
        ALELIB_ASSERT(cell_msh_tag == type_tag, "Invalid cell or invalid mesh", std::runtime_error);
        for (int i=0; i< nodes_per_cell; ++i)
        {
          if ( EOF == fscanf(file_ptr, "%d", &id_aux) )
            ALELIB_ASSERT(false, "invalid msh format", std::runtime_error);
          //cell->setNodeId(i, id_aux-1);
          c_verts[i] = VertexH(--id_aux);
        }
        CellH c = mesh->addCell(c_verts);
        c.setTag(mesh, physical);
        //mesh->pushCell(cell);
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

    int n_nodes               = MeshT::verts_per_cell;// mesh->nodesPerCell();
    //int n_vertices_per_facet  = mesh->verticesPerFacet();
    int n_nodes_per_facet     = MeshT::verts_per_facet;//mesh->nodesPerFacet();
    //int n_vertices_per_corner = mesh->verticesPerCorner();
    int n_nodes_per_corner    = MeshT::verts_per_ridge;


    std::vector<VertexH> nodes(n_nodes_per_facet);    // facet nodes
    std::vector<VertexH> bnodes(n_nodes_per_corner);  // corner nodes

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
        for (int i=0; i<n_nodes_per_facet; ++i)
        {
          fscanf(file_ptr, "%d", &id_aux);
          VertexH v(--id_aux);
          if (v.tag(mesh) == 0)
            v.setTag(mesh, physical);
          nodes[i] = v;
        }
        //std::copy( nodes, nodes + n_vertices_per_facet, vtcs );
        if (cell_dim > 1)
        {
          FacetH f = mesh->getFacetFromVertices(nodes.data());
          if (!f.isNull())
            f.setTag(mesh, physical); //std::cout << (++TESTE) << std::endl;
          else
          {
            printf("WARNING: INVALID FACET IN INPUT MESH! vtcs: ");
            for (int zz = 0; zz < n_nodes_per_facet; ++zz)
              printf("%d ", nodes[zz].id(mesh));
            printf("\n");
          }
        }
      }
      else if (elm_dim == cell_dim-2) // RIDGES
      {
        for (int i=0; i<n_nodes_per_corner; ++i)
        {
          fscanf(file_ptr, "%d", &id_aux);
          VertexH v(--id_aux);
          if (v.tag(mesh) == 0)
            v.setTag(mesh, physical);
          bnodes[i] = v;
        }
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
        for (int i=0; i<n_nodes; ++i)
        {
          fscanf(file_ptr, "%d", &id_aux);
          VertexH v(--id_aux);

          if (v.tag(mesh) == 0)
            v.setTag(mesh, physical);
        }
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




