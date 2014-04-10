#include <tr1/memory>
#include "msh_io.hpp"
#include <cstdio>
#include "../util/assert.hpp"
#include "msh_tags.hpp"
#include "alelib_tags.hpp"

namespace alelib
{

template<ECellType CT>
ECellType MeshIoMsh<CT>::identifiesMeshType(const char* filename, int* space_dim_) const
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
      current_elm_type = mshTag2ctype(EMshTag(type_tag));
      current_elm_dim = elm_dim;
      if (elm_dim==3)
        break;
    }
  
    fgets(buffer, sizeof(buffer), file_ptr);

  }

  fclose(file_ptr);
  
  return current_elm_type;

}



#if 0

// A malha deve estar alocada.
//
void MeshIoMsh::readFileMsh(const char* filename, Mesh * mesh)
{
  /*
   * O que é feito:
   *
   * 1) primeiramente lê-se apenas as células (conectividade clássica);
   * 2) depois são construídos as facet-edges e(ou) facet-faces, lendo-se
   *    os elementos de dimensões menores.
   *
   * */

  ALELIB_ASSERT(mesh, "invalid mesh pointer", std::invalid_argument);


  this->fi_registerFile(filename, ".msh");

  FILE * file_ptr = fopen(filename, "r");

  double  coord[3];
  int     type_tag;
  char    buffer[256];
  std::tr1::shared_ptr<Point> p_ptr(new Point());
  std::tr1::shared_ptr<Cell> c_ptr(mesh->createCell());

  int const spacedim = mesh->spaceDim();
  ALELIB_ASSERT(spacedim>0 && spacedim < 4, "mesh has invalid spacedim", std::invalid_argument);

  long    nodes_file_pos;  // $Nodes
  long    elems_file_pos;  // $Elements

  // nós
  nodes_file_pos = find_keyword("$Nodes", 6, file_ptr);
  //nodes_file_pos++;  // only to avoid gcc warning: variable ‘nodes_file_pos’ set but not used [-Wunused-but-set-variable]

  ALELIB_ASSERT(nodes_file_pos>0, "invalid file format", std::invalid_argument);

  int num_pts(0);
  int node_number(0);
  if ( EOF == fscanf(file_ptr, "%d", &num_pts) )
    ALELIB_ASSERT(false, "invalid msh format", std::runtime_error);

  //mesh->resizePointL(num_pts);
  
  //mesh->printInfo();
  //std::cout << "DEBUGGGGGGGGGGGGGGGGGGG:  "<<mesh << std::endl;
  //printf("DEBUGGGGGGGGGGGGGGGGGGGGGGGGGGG num_pts=%d; numNodesTotal()=%d; numNodes()=%d\n",num_pts,mesh->numNodesTotal(), mesh->numNodes());
  
  if (NULL == fgets(buffer, sizeof(buffer), file_ptr)) // escapa do \n
    ALELIB_ASSERT(false, "invalid msh format", std::runtime_error);
  for (int i=0; i< num_pts; ++i)
  {
    if ( NULL == fgets(buffer, sizeof(buffer), file_ptr) )
      ALELIB_ASSERT(false, "invalid msh format", std::runtime_error);
    sscanf(buffer, "%d %lf %lf %lf", &node_number, &coord[0], &coord[1], &coord[2]);
    ALELIB_ASSERT(node_number==i+1, "wrong file format", std::invalid_argument);
    p_ptr->setCoord(coord, spacedim);
    //mesh->getNodePtr(i)->setCoord(coord,spacedim);
    mesh->pushPoint(p_ptr.get());
  }
  // os pontos não estão completas: falta atribuir os labels

  // contagem de elementos e alocação
  
  elems_file_pos = find_keyword("$Elements", 9, file_ptr);

  ALELIB_ASSERT(elems_file_pos>0, "invalid file format", std::invalid_argument);

  int num_cells=0;
  int num_elms;

  if (EOF == fscanf(file_ptr, "%d", &num_elms) )
    ALELIB_ASSERT(false, "invalid msh format", std::runtime_error);

  /* ---------------------------------------
   * Detectando a ordem da malha, verificando sequencia dos elementos,
   * e contando o número de células.
   * --------------------------------------- */
  const int meshm_cell_msh_tag = mesh->cellMshTag();
  
  if (mshTag2ctype(EMshTag(meshm_cell_msh_tag)) != mesh->cellType())
  {
    //ALELIB_ASSERT(false, "invalid msh format", std::runtime_error);
    printf("ERROR: ctype2mshTag() = %d\n", ctype2mshTag(mesh->cellType()));
    printf("ERROR: cell->getMshTag() = %d\n", c_ptr->getMshTag());
    throw;
  }
  
  
  bool wrong_file_err=true;
  int  elem_number;               // contagem para verificação de erros.
  for (int k = 0; k < num_elms; ++k)
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
    if (type_tag == meshm_cell_msh_tag)
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

  Cell*  cell;
  //cell = Cell::create(mesh->cellType());



  /* --------------------------------------
   * Lendo as células
   * -------------------------------------- */
  fseek (file_ptr , elems_file_pos , SEEK_SET );
  if (EOF == fscanf(file_ptr, "%d", &num_elms) )
    ALELIB_ASSERT(false, "invalid msh format", std::runtime_error);

  this->timer.restart();

  int  inc(0);
  int  nodes_per_cell = mesh->nodesPerCell();
  int  id_aux;
  int  numm_tags;
  int  elm_dim;
  int  physical;
  int const cell_dim = mesh->cellDim();
  int const cell_msh_tag = mesh->cellMshTag();
  for (int k=0; k < num_elms; ++k)
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
      mesh->getNodePtr(id_aux)->setTag(physical);
    }
    else if (elm_dim == cell_dim)
    {
      cell = mesh->pushCell((int*)0);
      ++inc;
      ALELIB_ASSERT(cell_msh_tag == type_tag, "Invalid cell or invalid mesh", std::runtime_error);
      for (int i=0; i< nodes_per_cell; ++i)
      {
        if ( EOF == fscanf(file_ptr, "%d", &id_aux) )
          ALELIB_ASSERT(false, "invalid msh format", std::runtime_error);
        cell->setNodeId(i, id_aux-1);
      }
      cell->setTag(physical);
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

  /* constroi as facets e Corners */
  if (mesh->qBuildAdjacency())
    mesh->buildAdjacency();
  else
  {
    fclose(file_ptr);
    return;
  }

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

  int n_nodes               = mesh->nodesPerCell();
  //int n_vertices_per_facet  = mesh->verticesPerFacet();
  int n_nodes_per_facet     = mesh->nodesPerFacet();
  //int n_vertices_per_corner = mesh->verticesPerCorner();
  int n_nodes_per_corner    = mesh->nodesPerCorner();


  int* nodes  = new int[n_nodes_per_facet];      // facet nodes
  //int* vtcs   = new int[n_vertices_per_facet];
  int* bnodes = new int[n_nodes_per_corner];     // corner nodes
  //int* bvtcs  = new int[n_vertices_per_corner];
  int facet_id;
  int corner_id;

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
    else if (elm_dim == cell_dim-1) // facets
    {
      for (int i=0; i<n_nodes_per_facet; ++i)
      {
        fscanf(file_ptr, "%d", &nodes[i]);
        --nodes[i];
        if (mesh->getNodePtr(nodes[i])->getTag() == 0)
          mesh->getNodePtr(nodes[i])->setTag(physical);
      }
      //std::copy( nodes, nodes + n_vertices_per_facet, vtcs );
      if (cell_dim > 1)
      {
        if (mesh->getFacetIdFromVertices(nodes, facet_id))
          mesh->getFacetPtr(abs(facet_id))->setTag(physical); //std::cout << (++TESTE) << std::endl;
        else
        {
          printf("WARNING: INVALID FACET IN INPUT MESH! vtcs: ");
          for (int zz = 0; zz < n_nodes_per_facet; ++zz)
            printf("%d ", nodes[zz]);
          printf("\n");
        }
      }
    }
    else if (elm_dim == cell_dim-2) // corners
    {
      for (int i=0; i<n_nodes_per_corner; ++i)
      {
        fscanf(file_ptr, "%d", &bnodes[i]);
        --bnodes[i];
        if (mesh->getNodePtr(bnodes[i])->getTag() == 0)
          mesh->getNodePtr(bnodes[i])->setTag(physical);
      }
      //std::copy( bnodes, bnodes + n_vertices_per_corner, bvtcs );
      if (cell_dim>2)
      {
        if (mesh->getCornerIdFromVertices(bnodes, corner_id))
        {
          mesh->getCornerPtr(abs(corner_id))->setTag(physical); //std::cout << (++TESTE) << std::endl;
        }
        else if (mesh->isVertex(mesh->getNodePtr(bnodes[0])) ) // if is vertex
            printf("WARNING: INVALID CORNER IN INPUT MESH!\n");
      }
    }
    else
    {
      for (int i=0; i<n_nodes; ++i)
      {
        fscanf(file_ptr, "%d", &id_aux);
        --id_aux;

        if ((mesh->getNodePtr(id_aux)->getTag()) == 0)
          mesh->getNodePtr(id_aux)->setTag(physical);
      }
    }


  }

  this->timer.elapsed("readFileMsh(): search for boundary elements");

  if (mesh->cellDim()>2)
  {
    const int n_corners_per_facet = mesh->numCornersPerFacet();
    // int facetm_facets[n_corners_per_facet];
    int *facetm_facets = new int [n_corners_per_facet];
    Facet const* facet;
    Corner* corner;
    for (int i = 0; i < mesh->numFacetsTotal(); ++i)
    {
      facet = mesh->getFacetPtr(i);
      if (facet->isDisabled())
        continue;
      mesh->getCellPtr(facet->getIncidCell())->getFacetCornersId(facet->getPosition(), facetm_facets);

      for (int j = 0; j < n_corners_per_facet; ++j)
      {
        corner = mesh->getCornerPtr(facetm_facets[j]);
        if (corner->getTag() == 0)
          corner->setTag(facet->getTag());
      }
    }
    
    delete [] facetm_facets;
    facetm_facets = NULL;
  }

  if (mesh->cellDim()>1)
  {
    const int n_cells_total = mesh->numCellsTotal();
    Cell * cell;
    Facet * facet;
    for (int i = 0; i < n_cells_total; ++i)
    {
      cell = mesh->getCellPtr(i);
      if (cell->isDisabled())
        continue;
      for (int j = 0; j < mesh->numFacetsPerCell(); ++j)
      {
        facet = mesh->getFacetPtr(cell->getFacetId(j));
        
        if (facet->getTag() == 0)
          facet->setTag(cell->getTag());
      }
    } // end loop
  } // endif

  fclose(file_ptr);
  //File.close();

  delete [] nodes;
  //delete [] vtcs;
  delete [] bnodes;
  //delete [] bvtcs;

  mesh->timer.addItems(this->timer);
}


#endif


template class MeshIoMsh<EDGE>       ;
template class MeshIoMsh<TRIANGLE>   ;
template class MeshIoMsh<QUADRANGLE> ;
template class MeshIoMsh<TETRAHEDRON>;
template class MeshIoMsh<HEXAHEDRON> ;


} // end namespace alelib

