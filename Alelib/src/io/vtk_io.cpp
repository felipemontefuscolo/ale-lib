#include "meshiovtk.hpp"
#include <tr1/array>
#include "../../util/misc2.hpp"

template<typename T>
T reverseEndian( T aa)
{
  T tt;
  char *cc = ( char* ) & aa;
  char *dd = ( char* ) & tt;

  // swap the bytes into a temporary buffer
  int const n = sizeof(T);

  for (int i = 0; i < n; ++i)
    dd[i] = cc[n-i-1];

  return tt;
}


void MeshIoVtk::attachMesh(Mesh const* mesh)
{
  FEPIC_ASSERT(mesh != NULL, "invalid mesh", std::invalid_argument);
  m_mesh = mesh;
  m_spacedim = m_mesh->spaceDim();

  ECellType s = m_mesh->cellType();
  
  switch (s)
  {
    case EDGE2:        m_c_printer = &MeshIoVtk::fi_printCellVtk_Edge2;         break;
    case EDGE3:        m_c_printer = &MeshIoVtk::fi_printCellVtk_Edge3;         break;
    case TRIANGLE3:    m_c_printer = &MeshIoVtk::fi_printCellVtk_Triangle3;     break;
    case TRIANGLE6:    m_c_printer = &MeshIoVtk::fi_printCellVtk_Triangle6;     break;
    case QUADRANGLE4:  m_c_printer = &MeshIoVtk::fi_printCellVtk_Quadrangle4;   break;
    case QUADRANGLE8:  m_c_printer = &MeshIoVtk::fi_printCellVtk_Quadrangle8;   break;
    case QUADRANGLE9:  m_c_printer = &MeshIoVtk::fi_printCellVtk_Quadrangle9;   break;
    case TETRAHEDRON4: m_c_printer = &MeshIoVtk::fi_printCellVtk_Tetrahedron4;  break;
    case TETRAHEDRON10:m_c_printer = &MeshIoVtk::fi_printCellVtk_Tetrahedron10; break;
    case HEXAHEDRON8:  m_c_printer = &MeshIoVtk::fi_printCellVtk_Hexahedron8;   break;
    case HEXAHEDRON20: m_c_printer = &MeshIoVtk::fi_printCellVtk_Hexahedron20;  break;
    case HEXAHEDRON27: m_c_printer = &MeshIoVtk::fi_printCellVtk_Hexahedron27;  break;
    default:
      FEPIC_CHECK(false, "invalid mesh cell type", std::invalid_argument);
  }

  
}


void MeshIoVtk::fi_printPointVtk(Point const* p, FILE *fp) const
{
  double x[3] = { p->getCoord(0),
                  m_spacedim>1 ? p->getCoord(1): 0.0,
                  m_spacedim>2 ? p->getCoord(2): 0.0};
  if (m_is_binary)
  {
    x[0] = reverseEndian(x[0]);
    x[1] = reverseEndian(x[1]);
    x[2] = reverseEndian(x[2]);
    fwrite(x, sizeof(double), 3, fp);
  }
  else
    fprintf(fp, "%f %f %f\n", x[0], x[1], x[2]);
}



void MeshIoVtk::fi_printCellVtk_Edge2(int const* ids, FILE *fp) const
{
  if (m_is_binary)
  {
    int const n_nds   = 2;
    int const n_parts = 1;
    int ods[n_parts][n_nds + 1] = {{ n_nds, ids[0], ids[1] }};

    for (int i = 0; i < n_parts; ++i)
      for (int j = 0; j < n_nds + 1; ++j)
        ods[i][j] = reverseEndian(ods[i][j]);
    for (int i = 0; i < n_parts; ++i)
      fwrite(ods[i], sizeof(int), n_nds+1, fp);
  }
  else
    fprintf(fp,"2 %d %d\n", ids[0], ids[1]);
}

void MeshIoVtk::fi_printCellVtk_Edge3(int const* ids, FILE *fp) const
{
  if (m_is_binary)
  {
    int const n_nds   = 2;
    int const n_parts = 2;
    int ods[n_parts][n_nds + 1] = {{ n_nds, ids[0], ids[2] },
                                   { n_nds, ids[2], ids[1] }};
    
    for (int i = 0; i < n_parts; ++i)
      for (int j = 0; j < n_nds + 1; ++j)
        ods[i][j] = reverseEndian(ods[i][j]);
    for (int i = 0; i < n_parts; ++i)
      fwrite(ods[i], sizeof(int), n_nds+1, fp);
  }
  else
  {
    fprintf(fp,"2 %d %d\n", ids[0], ids[2]);
    fprintf(fp,"2 %d %d\n", ids[2], ids[1]);
  }
}

void MeshIoVtk::fi_printCellVtk_Triangle3(int const* ids, FILE *fp) const
{
  if (m_is_binary)
  {
    int const n_nds   = 3;
    int const n_parts = 1;
    int ods[n_parts][n_nds + 1] = {{ n_nds, ids[0], ids[1], ids[2] } };
    
    for (int i = 0; i < n_parts; ++i)
      for (int j = 0; j < n_nds + 1; ++j)
        ods[i][j] = reverseEndian(ods[i][j]);
    for (int i = 0; i < n_parts; ++i)
      fwrite(ods[i], sizeof(int), n_nds+1, fp);
  }
  else
  {
    fprintf(fp,"3 %d %d %d\n", *ids, ids[1], ids[2]);
  }
}

void MeshIoVtk::fi_printCellVtk_Triangle6(int const* ids, FILE *fp) const
{
  if (m_is_binary)
  {
    int const n_nds   = 3;
    int const n_parts = 4;
    int ods[n_parts][n_nds + 1] = {{ n_nds, ids[3], ids[4], ids[5] },
                                   { n_nds, ids[3], ids[1], ids[4] },
                                   { n_nds, ids[3], ids[5], ids[0] },
                                   { n_nds, ids[4], ids[2], ids[5] }};
    
    for (int i = 0; i < n_parts; ++i)
      for (int j = 0; j < n_nds + 1; ++j)
        ods[i][j] = reverseEndian(ods[i][j]);
    for (int i = 0; i < n_parts; ++i)
      fwrite(ods[i], sizeof(int), n_nds+1, fp);
  }
  else
  {
    fprintf(fp,"3 %d %d %d\n", ids[3], ids[4], ids[5]);
    fprintf(fp,"3 %d %d %d\n", ids[3], ids[1], ids[4]);
    fprintf(fp,"3 %d %d %d\n", ids[3], ids[5], ids[0]);
    fprintf(fp,"3 %d %d %d\n", ids[4], ids[2], ids[5]);
  }
}

void MeshIoVtk::fi_printCellVtk_Quadrangle4(int const* ids, FILE *fp) const
{
  if (m_is_binary)
  {
    int const n_nds   = 4;
    int const n_parts = 1;
    int ods[n_parts][n_nds + 1] = {{ n_nds, ids[0], ids[1], ids[2], ids[3] } };
    
    for (int i = 0; i < n_parts; ++i)
      for (int j = 0; j < n_nds + 1; ++j)
        ods[i][j] = reverseEndian(ods[i][j]);
    for (int i = 0; i < n_parts; ++i)
      fwrite(ods[i], sizeof(int), n_nds+1, fp);
  }
  else
  {
    fprintf(fp,"4 %d %d %d %d\n", *ids, ids[1], ids[2], ids[3]);
  }
}

void MeshIoVtk::fi_printCellVtk_Quadrangle8(int const* ids, FILE *fp) const
{
  if (m_is_binary)
  {
    int const n_nds   = 8;
    int const n_parts = 1;
    int ods[n_parts][n_nds + 1] = {{ n_nds, ids[0], ids[1], ids[2], ids[3], ids[4], ids[5], ids[6], ids[7] } };
    
    for (int i = 0; i < n_parts; ++i)
      for (int j = 0; j < n_nds + 1; ++j)
        ods[i][j] = reverseEndian(ods[i][j]);
    for (int i = 0; i < n_parts; ++i)
      fwrite(ods[i], sizeof(int), n_nds+1, fp);
  }
  else
  {
    fprintf(fp,"8 %d %d %d %d %d %d %d %d\n", *ids, ids[1], ids[2], ids[3], ids[4], ids[5], ids[6], ids[7]);
  }
}

void MeshIoVtk::fi_printCellVtk_Quadrangle9(int const* ids, FILE *fp) const
{
  if (m_is_binary)
  {
    int const n_nds   = 4;
    int const n_parts = 4;
    int ods[n_parts][n_nds + 1] = {{ n_nds, ids[8], ids[7], ids[0], ids[4] },
                                   { n_nds, ids[8], ids[4], ids[1], ids[5] },
                                   { n_nds, ids[8], ids[5], ids[2], ids[6] },
                                   { n_nds, ids[8], ids[6], ids[3], ids[7] }};
    
    for (int i = 0; i < n_parts; ++i)
      for (int j = 0; j < n_nds + 1; ++j)
        ods[i][j] = reverseEndian(ods[i][j]);
    for (int i = 0; i < n_parts; ++i)
      fwrite(ods[i], sizeof(int), n_nds+1, fp);
  }
  else
  {
    fprintf(fp,"4 %d %d %d %d\n", ids[8], ids[7], ids[0], ids[4]);
    fprintf(fp,"4 %d %d %d %d\n", ids[8], ids[4], ids[1], ids[5]);
    fprintf(fp,"4 %d %d %d %d\n", ids[8], ids[5], ids[2], ids[6]);
    fprintf(fp,"4 %d %d %d %d\n", ids[8], ids[6], ids[3], ids[7]);
  }
}

void MeshIoVtk::fi_printCellVtk_Tetrahedron4(int const* ids, FILE *fp) const
{
  if (m_is_binary)
  {
    int const n_nds   = 4;
    int const n_parts = 1;
    int ods[n_parts][n_nds + 1] = {{ n_nds, ids[0], ids[1], ids[2], ids[3] } };
    
    for (int i = 0; i < n_parts; ++i)
      for (int j = 0; j < n_nds + 1; ++j)
        ods[i][j] = reverseEndian(ods[i][j]);
    for (int i = 0; i < n_parts; ++i)
      fwrite(ods[i], sizeof(int), n_nds+1, fp);
  }
  else
  {
    fprintf(fp,"4 %d %d %d %d\n", ids[0], ids[1], ids[2], ids[3]);
  }
}

void MeshIoVtk::fi_printCellVtk_Tetrahedron10(int const* ids, FILE *fp) const
{
  if (m_is_binary)
  {
    int const n_nds   = 4;
    int const n_parts = 8;
    int ods[n_parts][n_nds + 1] = {{ n_nds, ids[0], ids[4], ids[7], ids[6] },
                                   { n_nds, ids[1], ids[4], ids[5], ids[9] },
                                   { n_nds, ids[2], ids[5], ids[6], ids[8] },
                                   { n_nds, ids[3], ids[7], ids[9], ids[8] },
                                   { n_nds, ids[5], ids[8], ids[7], ids[9] },
                                   { n_nds, ids[5], ids[7], ids[4], ids[9] },
                                   { n_nds, ids[7], ids[8], ids[5], ids[6] },
                                   { n_nds, ids[4], ids[7], ids[5], ids[6] }};
    
    for (int i = 0; i < n_parts; ++i)
      for (int j = 0; j < n_nds + 1; ++j)
        ods[i][j] = reverseEndian(ods[i][j]);
    for (int i = 0; i < n_parts; ++i)
      fwrite(ods[i], sizeof(int), n_nds+1, fp);
  }
  else
  {
    fprintf(fp,"4 %d %d %d %d\n", ids[0], ids[4], ids[7], ids[6]);
    fprintf(fp,"4 %d %d %d %d\n", ids[1], ids[4], ids[5], ids[9]);
    fprintf(fp,"4 %d %d %d %d\n", ids[2], ids[5], ids[6], ids[8]);
    fprintf(fp,"4 %d %d %d %d\n", ids[3], ids[7], ids[9], ids[8]);
    fprintf(fp,"4 %d %d %d %d\n", ids[5], ids[8], ids[7], ids[9]);
    fprintf(fp,"4 %d %d %d %d\n", ids[5], ids[7], ids[4], ids[9]);
    fprintf(fp,"4 %d %d %d %d\n", ids[7], ids[8], ids[5], ids[6]);
    fprintf(fp,"4 %d %d %d %d\n", ids[4], ids[7], ids[5], ids[6]);
  }
}

void MeshIoVtk::fi_printCellVtk_Hexahedron8(int const* ids, FILE *fp) const
{
  if (m_is_binary)
  {
    int const n_nds   = 8;
    int const n_parts = 1;
    int ods[n_parts][n_nds + 1] = {{ n_nds, ids[0],ids[1],ids[2],ids[3],ids[4],ids[5],ids[6],ids[7] }};
    
    for (int i = 0; i < n_parts; ++i)
      for (int j = 0; j < n_nds + 1; ++j)
        ods[i][j] = reverseEndian(ods[i][j]);
    for (int i = 0; i < n_parts; ++i)
      fwrite(ods[i], sizeof(int), n_nds+1, fp);
  }
  else
  {
    fprintf(fp,"8 %d %d %d %d %d %d %d %d\n", ids[0],ids[1],ids[2],ids[3],ids[4],ids[5],ids[6],ids[7]);
  }
}

void MeshIoVtk::fi_printCellVtk_Hexahedron20(int const* ids, FILE *fp) const
{
  if (m_is_binary)
  {
    int const n_nds   = 20;
    int const n_parts = 1;
    int ods[n_parts][n_nds + 1] = {{ n_nds, ids[0], ids[1], ids[2], ids[3], ids[4], ids[5], ids[6], ids[7], ids[8], ids[11],
                                      ids[13], ids[9], ids[16], ids[18], ids[19], ids[17], ids[10], ids[12], ids[14], ids[15] }};
    
    for (int i = 0; i < n_parts; ++i)
      for (int j = 0; j < n_nds + 1; ++j)
        ods[i][j] = reverseEndian(ods[i][j]);
    for (int i = 0; i < n_parts; ++i)
      fwrite(ods[i], sizeof(int), n_nds+1, fp);
  }
  else
  {
    fprintf(fp,"20 %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
           ids[0], ids[1], ids[2], ids[3], ids[4], ids[5], ids[6], ids[7], ids[8], ids[11],
           ids[13], ids[9], ids[16], ids[18], ids[19], ids[17], ids[10], ids[12], ids[14], ids[15]);
  }
}

void MeshIoVtk::fi_printCellVtk_Hexahedron27(int const* ids, FILE *fp) const
{
  if (m_is_binary)
  {
    int const n_nds   = 8;
    int const n_parts = 8;
    int ods[n_parts][n_nds + 1] = {{ n_nds, ids[ 0],ids[ 8],ids[20],ids[ 9],ids[10],ids[21],ids[26],ids[22] },
                                   { n_nds, ids[10],ids[21],ids[26],ids[22],ids[ 4],ids[16],ids[25],ids[17] },
                                   { n_nds, ids[ 8],ids[ 1],ids[11],ids[20],ids[21],ids[12],ids[23],ids[26] },
                                   { n_nds, ids[21],ids[12],ids[23],ids[26],ids[16],ids[ 5],ids[18],ids[25] },
                                   { n_nds, ids[ 9],ids[20],ids[13],ids[ 3],ids[22],ids[26],ids[24],ids[15] },
                                   { n_nds, ids[22],ids[26],ids[24],ids[15],ids[17],ids[25],ids[19],ids[ 7] },
                                   { n_nds, ids[20],ids[11],ids[ 2],ids[13],ids[26],ids[23],ids[14],ids[24] },
                                   { n_nds, ids[26],ids[23],ids[14],ids[24],ids[25],ids[18],ids[ 6],ids[19] }};
    
    for (int i = 0; i < n_parts; ++i)
      for (int j = 0; j < n_nds + 1; ++j)
        ods[i][j] = reverseEndian(ods[i][j]);
    for (int i = 0; i < n_parts; ++i)
      fwrite(ods[i], sizeof(int), n_nds+1, fp);
  }
  else
  {
    fprintf(fp,"8 %d %d %d %d %d %d %d %d\n", ids[ 0],ids[ 8],ids[20],ids[ 9],ids[10],ids[21],ids[26],ids[22]);
    fprintf(fp,"8 %d %d %d %d %d %d %d %d\n", ids[10],ids[21],ids[26],ids[22],ids[ 4],ids[16],ids[25],ids[17]);
    fprintf(fp,"8 %d %d %d %d %d %d %d %d\n", ids[ 8],ids[ 1],ids[11],ids[20],ids[21],ids[12],ids[23],ids[26]);
    fprintf(fp,"8 %d %d %d %d %d %d %d %d\n", ids[21],ids[12],ids[23],ids[26],ids[16],ids[ 5],ids[18],ids[25]);
    fprintf(fp,"8 %d %d %d %d %d %d %d %d\n", ids[ 9],ids[20],ids[13],ids[ 3],ids[22],ids[26],ids[24],ids[15]);
    fprintf(fp,"8 %d %d %d %d %d %d %d %d\n", ids[22],ids[26],ids[24],ids[15],ids[17],ids[25],ids[19],ids[ 7]);
    fprintf(fp,"8 %d %d %d %d %d %d %d %d\n", ids[20],ids[11],ids[ 2],ids[13],ids[26],ids[23],ids[14],ids[24]);
    fprintf(fp,"8 %d %d %d %d %d %d %d %d\n", ids[26],ids[23],ids[14],ids[24],ids[25],ids[18],ids[ 6],ids[19]);
  }
}


namespace fi_VtkTagsInitializers
{
  std::tr1::array<int , N_CELL_TYPES> m_tags()
  {
    std::tr1::array<int , N_CELL_TYPES> tab;
    
    tab[log2_i32(EDGE2        ) ] = 3 ;
    tab[log2_i32(EDGE3        ) ] = 3 ;
    tab[log2_i32(TRIANGLE3    ) ] = 5 ;
    tab[log2_i32(TRIANGLE6    ) ] = 5 ;
    tab[log2_i32(QUADRANGLE4  ) ] = 9 ;
    tab[log2_i32(QUADRANGLE8  ) ] = 23;
    tab[log2_i32(QUADRANGLE9  ) ] = 9 ;
    tab[log2_i32(TETRAHEDRON4 ) ] = 10;
    tab[log2_i32(TETRAHEDRON10) ] = 10;
    tab[log2_i32(HEXAHEDRON8  ) ] = 12;
    tab[log2_i32(HEXAHEDRON20 ) ] = 25;
    tab[log2_i32(HEXAHEDRON27 ) ] = 12;
    
    return tab;
  }
  
  std::tr1::array<int , N_CELL_TYPES> fi_n_divs()
  {
    std::tr1::array<int , N_CELL_TYPES> tab;
    
    tab[log2_i32(EDGE2        ) ] = 1;
    tab[log2_i32(EDGE3        ) ] = 2;
    tab[log2_i32(TRIANGLE3    ) ] = 1;
    tab[log2_i32(TRIANGLE6    ) ] = 4;
    tab[log2_i32(QUADRANGLE4  ) ] = 1;
    tab[log2_i32(QUADRANGLE8  ) ] = 1; // FIXME: can't split this element
    tab[log2_i32(QUADRANGLE9  ) ] = 4;
    tab[log2_i32(TETRAHEDRON4 ) ] = 1;
    tab[log2_i32(TETRAHEDRON10) ] = 8;
    tab[log2_i32(HEXAHEDRON8  ) ] = 1;
    tab[log2_i32(HEXAHEDRON20 ) ] = 1; // FIXME: can't split this element
    tab[log2_i32(HEXAHEDRON27 ) ] = 8;
    
    return tab;
  }
  
}


int MeshIoVtk::getVtkTag(ECellType type)
{
  std::tr1::array<int , N_CELL_TYPES> vtk_tag = fi_VtkTagsInitializers::m_tags();
      
  unsigned idx = log2_i32(type);
    
  FEPIC_CHECK(idx < N_CELL_TYPES, "invalid or not supported cell type", std::invalid_argument);
  
  return vtk_tag[idx];
}

int MeshIoVtk::getNumDivisions(ECellType type)
{
  std::tr1::array<int , N_CELL_TYPES> divs = fi_VtkTagsInitializers::fi_n_divs();
      
  unsigned idx = log2_i32(type);
    
  FEPIC_CHECK(idx < N_CELL_TYPES, "invalid or not supported cell type", std::invalid_argument);
  
  return divs[idx];
}


/** Print mesh in vtk file format.
 * @param flinear if the mesh has higher order elements and this flag is false, than
 * the mesh print each higher order cell as a compound of linear cells, otherwise the
 * cells are printed as if lienar cells, disregarding high order nodes..
 */
void MeshIoVtk::writeVtk(std::string outname)
{
  /*
  *  DISABLE entities are not printed.
  */

  this->m_add_node_scalar_vtk_n_calls=0;
  this->m_add_cell_scalar_vtk_n_calls=0;

  if (!(this->m_sofn_already_called))
    this->setOutputFileName(outname.c_str());

  if (!strcmp(outname.c_str(), "")) // if equal
    outname = this->fi_popNextName(this->m_filenumVtk, ".vtk");
  
  ++m_filenumVtk;
  
  FILE *file_ptr = fopen(outname.c_str(), m_is_binary ? "wb" : "w");

  fprintf(file_ptr, "# vtk DataFile Version 2.0\n"
                    "Created by fepic++\n");
  if (m_is_binary)
    fprintf(file_ptr, "BINARY\n");
  else
    fprintf(file_ptr, "ASCII\n");
  fprintf(file_ptr, "DATASET UNSTRUCTURED_GRID\n");
  

  fprintf(file_ptr, "POINTS %d double\n", m_mesh->numNodes());

  Point const* point;
  int nm_points_total = m_mesh->numNodesTotal();
  
  /* printing the points */
  for (int i = 0; i < nm_points_total; ++i)
  {
    point = m_mesh->getNodePtr(i);
    if (point->isDisabled())
      continue;
    fi_printPointVtk(point, file_ptr);
  }
  
  
  int  n_cd   = getNumDivisions(m_mesh->cellType());    // number os cell's division
  int  n_pseudo_cells = m_mesh->numCells() * n_cd;
  int  n_cells_total  = m_mesh->numCellsTotal();
  int  nodes[FEPIC_MAX_N_NODES_P_CELL];
  Cell const *cell;
  
  /* imprimindo células */
  fprintf(file_ptr,"\nCELLS %d %d\n", n_pseudo_cells, n_cd>1 ? n_pseudo_cells*(1+m_mesh->verticesPerCell()) :
                                                               n_pseudo_cells*(1+m_mesh->nodesPerCell()));
                                                               
  //printf("DEBUG n_cells_total = %d\n", n_cells_total);
  for (int k = 0; k < n_cells_total; ++k)
  {
    cell = m_mesh->getCellPtr(k);
    if (cell->isDisabled())
      continue;
    m_mesh->getCellNodesContigId(cell,nodes);
    CALL_MEMBER_FN(*this, m_c_printer)(nodes, file_ptr);
  }
  
  // printing types
  int type = MeshIoVtk::getVtkTag(m_mesh->cellType());
  fprintf(file_ptr,"\nCELL_TYPES %d\n", n_pseudo_cells);
  for (int k = 0; k < n_cells_total; ++k)
  {
    cell = m_mesh->getCellPtr(k);
    if (cell->isDisabled())
      continue;
    for (int i = 0; i < n_cd; ++i)
    {
      if (m_is_binary)
      {
        int tmp = reverseEndian(type);
        fwrite(&tmp, sizeof(int), 1, file_ptr);
      }
      else
        fprintf(file_ptr, "%d ", type);
    }
  }
  fprintf(file_ptr,"\n");

  fclose(file_ptr);
}

void MeshIoVtk::addNodeScalarVtk(const char* nome_var, DefaultGetDataVtk const& data)
{

  std::string ss = this->fi_popNextName(this->m_filenumVtk-1, ".vtk");

  FILE * file_ptr = fopen(ss.c_str(), "a");
  
  FEPIC_ASSERT(file_ptr != NULL, "could not open the file", std::runtime_error);
  
  const int num_pts_total = m_mesh->numNodesTotal();
  const int num_pts       = m_mesh->numNodes();
  if (m_add_node_scalar_vtk_n_calls==0)
  {
    fprintf(file_ptr,"POINT_DATA %d\n", num_pts);
  }
  m_add_node_scalar_vtk_n_calls++;

  fprintf(file_ptr,"SCALARS %s double\nLOOKUP_TABLE default\n", nome_var);

  for (int i=0; i<num_pts_total; ++i)
    if (!(m_mesh->getNodePtr(i)->isDisabled()))
    {
      if (m_is_binary)
      {
        double tmp = reverseEndian(data.get_data_r(i));
        fwrite(&tmp, sizeof(double), 1, file_ptr);
      }
      else
        fprintf(file_ptr,"%.14e\n", data.get_data_r(i));
    }
  fprintf(file_ptr,"\n\n");

  fclose(file_ptr);
}

void MeshIoVtk::addNodeVectorVtk(const char* nome_var, DefaultGetDataVtk const& data)
{

  std::string ss = this->fi_popNextName(this->m_filenumVtk-1, ".vtk");

  FILE * file_ptr = fopen(ss.c_str(), "a");
  
  FEPIC_ASSERT(file_ptr != NULL, "could not open the file", std::runtime_error);
  
  const int num_pts_total = m_mesh->numNodesTotal();
  const int num_pts       = m_mesh->numNodes();
  if (m_add_node_scalar_vtk_n_calls==0)
  {
    fprintf(file_ptr,"POINT_DATA %d\n", num_pts);
  }
  m_add_node_scalar_vtk_n_calls++;

  fprintf(file_ptr,"VECTORS %s double\n", nome_var);

  double vec[3]={0,0,0};
  int vec_size = data.vec_ncomps();
  for (int i=0; i<num_pts_total; ++i)
  {
    if (!(m_mesh->getNodePtr(i)->isDisabled()))
    {
      data.get_vec(i, vec);
      FEPIC_CHECK(vec_size>=0 && vec_size<4, "invalid vtk vector", std::invalid_argument);
      
      if (m_is_binary)
      {
        for (int j = 0; j < vec_size; ++j)
        {
          vec[j] = reverseEndian(vec[j]);
          fwrite(&vec[j], sizeof(double), 1, file_ptr);
        }
        for (int j = vec_size; j < 3; ++j)
        {
          double tmp = reverseEndian(0);
          fwrite(&tmp, sizeof(double), 1, file_ptr);
        }
      }
      else
      {
        for (int j = 0; j < vec_size; ++j)
          fprintf(file_ptr,"%.14e ", vec[j]);
        for (int j = vec_size; j < 3; ++j)
          fprintf(file_ptr,"0.0 ");
        fprintf(file_ptr,"\n");
      }
    }
  }
  fprintf(file_ptr,"\n\n");

  fclose(file_ptr);
}


void MeshIoVtk::addCellScalarVtk(const char* nome_var, DefaultGetDataVtk const& data)
{

  std::string ss = this->fi_popNextName(this->m_filenumVtk-1, ".vtk");

  FILE * file_ptr = fopen(ss.c_str(), "a");
  
  FEPIC_ASSERT(file_ptr != NULL, "could not open the file", std::runtime_error);
  
  int  n_cd   = getNumDivisions(m_mesh->cellType()); // num divisions
  const int num_cells_total = m_mesh->numCellsTotal();
  const int num_cells = m_mesh->numCells();
  if (m_add_cell_scalar_vtk_n_calls==0)
  {
    fprintf(file_ptr,"CELL_DATA %d\n", num_cells*n_cd);
  }
  m_add_cell_scalar_vtk_n_calls++;

  fprintf(file_ptr,"SCALARS %s double\nLOOKUP_TABLE default\n", nome_var);

  for (int i=0; i<num_cells_total; ++i)
  {
    if (m_mesh->getCellPtr(i)->isDisabled())
      continue;
    for (int j = 0; j < n_cd; ++j)
    {
      if (m_is_binary)
      {
        double tmp = data.get_data_r(i);
        fwrite(&tmp, sizeof(double), 1, file_ptr);
      }
      else
        fprintf(file_ptr,"%f\n", data.get_data_r(i));
    }
  }

  fprintf(file_ptr,"\n\n");

  fclose(file_ptr);
}




// ======================= Interger version =====================================

void MeshIoVtk::addNodeIntVtk(const char* nome_var, DefaultGetDataVtk const& data)
{

  std::string ss = this->fi_popNextName(this->m_filenumVtk-1, ".vtk");

  FILE * file_ptr = fopen(ss.c_str(), "a");
  
  FEPIC_ASSERT(file_ptr != NULL, "could not open the file", std::runtime_error);
  
  const int num_pts_total = m_mesh->numNodesTotal();
  const int num_pts = m_mesh->numNodes();
  
  if (m_add_node_scalar_vtk_n_calls==0)
  {
    fprintf(file_ptr,"POINT_DATA %d\n\n", num_pts);
  }
  m_add_node_scalar_vtk_n_calls++;

  fprintf(file_ptr,"SCALARS %s int\nLOOKUP_TABLE default\n", nome_var);

  
  for (int i=0; i<num_pts_total; ++i)
    if (!(m_mesh->getNodePtr(i)->isDisabled()))
    {
      if (m_is_binary)
      {
        int tmp = reverseEndian(data.get_data_i(i));
        fwrite(&tmp, sizeof(int), 1, file_ptr);
      }
      else       
        fprintf(file_ptr,"%d\n", data.get_data_i(i));
    }

  fprintf(file_ptr,"\n");

  fclose(file_ptr);
}


void MeshIoVtk::addCellIntVtk(const char* nome_var, DefaultGetDataVtk const& data)
{

  std::string ss = this->fi_popNextName(this->m_filenumVtk-1, ".vtk");

  FILE * file_ptr = fopen(ss.c_str(), "a");
  
  FEPIC_ASSERT(file_ptr != NULL, "could not open the file", std::runtime_error);
  
  int  n_cd   = getNumDivisions(m_mesh->cellType()); // num divisions
  const int num_cells_total = m_mesh->numCellsTotal();
  const int num_cells = m_mesh->numCells();
  
  if (m_add_cell_scalar_vtk_n_calls==0)
  {
    fprintf(file_ptr,"CELL_DATA %d\n\n", num_cells*n_cd);
  }
  m_add_cell_scalar_vtk_n_calls++;

  fprintf(file_ptr,"SCALARS %s int\nLOOKUP_TABLE default\n", nome_var);

  for (int i=0; i<num_cells_total; ++i)
  {
    if (m_mesh->getCellPtr(i)->isDisabled())
      continue;
    for (int j = 0; j < n_cd; ++j)
    {
      if (m_is_binary)
      {
        int tmp = reverseEndian(data.get_data_i(i));
        fwrite(&tmp, sizeof(int), 1, file_ptr);
      }
      else       
        fprintf(file_ptr,"%d\n", data.get_data_i(i));
    }
  }

  fprintf(file_ptr,"\n");

  fclose(file_ptr);
}



void MeshIoVtk::printPointTagVtk(const char* nome_var)
{

  std::string ss = this->fi_popNextName(this->m_filenumVtk-1, ".vtk");

  FILE * file_ptr = fopen(ss.c_str(), "a");
  
  FEPIC_ASSERT(file_ptr != NULL, "could not open the file", std::runtime_error);
  
  const int num_pts = m_mesh->numNodes();
  if (m_add_node_scalar_vtk_n_calls==0)
  {
    fprintf(file_ptr,"POINT_DATA %d\n\n", num_pts);
  }
  m_add_node_scalar_vtk_n_calls++;

  fprintf(file_ptr,"SCALARS %s int\nLOOKUP_TABLE default\n", nome_var);

  int const num_pts_total = m_mesh->numNodesTotal();
  Point const* point;
  for (int i=0; i<num_pts_total; ++i)
  {
    point = m_mesh->getNodePtr(i);
    if (point->isDisabled())
      continue;
    if (m_is_binary)
    {
      int tmp = reverseEndian(point->getTag());
      fwrite(&tmp, sizeof(int), 1, file_ptr);
    }
    else
      fprintf(file_ptr,"%d\n", point->getTag());
  }

  fprintf(file_ptr,"\n");

  fclose(file_ptr);
}


void MeshIoVtk::printPointIcellVtk(const char* nome_var)
{

  std::string ss = this->fi_popNextName(this->m_filenumVtk-1, ".vtk");

  FILE * file_ptr = fopen(ss.c_str(), "a");
  
  FEPIC_ASSERT(file_ptr != NULL, "could not open the file", std::runtime_error);
  
  const int num_pts = m_mesh->numNodes();
  if (m_add_node_scalar_vtk_n_calls==0)
  {
    fprintf(file_ptr,"POINT_DATA %d\n\n", num_pts);
  }
  m_add_node_scalar_vtk_n_calls++;

  fprintf(file_ptr,"SCALARS %s int\nLOOKUP_TABLE default\n", nome_var);

  int const num_pts_total = m_mesh->numNodesTotal();
  Point const* point;
  for (int i=0; i<num_pts_total; ++i)
  {
    point = m_mesh->getNodePtr(i);
    if (point->isDisabled())
      continue;
    if (m_is_binary)
    {
      int tmp = reverseEndian(m_mesh->getCellContigId(point->getIncidCell()));
      fwrite(&tmp, sizeof(int), 1, file_ptr);
    }
    else    
      fprintf(file_ptr,"%d\n", m_mesh->getCellContigId(point->getIncidCell()));
  }

  fprintf(file_ptr,"\n");

  fclose(file_ptr);
}


void MeshIoVtk::printPointPositionVtk(const char* nome_var)
{

  std::string ss = this->fi_popNextName(this->m_filenumVtk-1, ".vtk");

  FILE * file_ptr = fopen(ss.c_str(), "a");
  
  FEPIC_ASSERT(file_ptr != NULL, "could not open the file", std::runtime_error);
  
  const int num_pts = m_mesh->numNodes();
  if (m_add_node_scalar_vtk_n_calls==0)
  {
    fprintf(file_ptr,"POINT_DATA %d\n\n", num_pts);
  }
  m_add_node_scalar_vtk_n_calls++;

  fprintf(file_ptr,"SCALARS %s int\nLOOKUP_TABLE default\n", nome_var);

  int const num_pts_total = m_mesh->numNodesTotal();
  Point const* point;
  for (int i=0; i<num_pts_total; ++i)
  {
    point = m_mesh->getNodePtr(i);
    if (point->isDisabled())
      continue;
    if (m_is_binary)
    {
      int tmp = reverseEndian(point->getPosition());
      fwrite(&tmp, sizeof(int), 1, file_ptr);
    }
    else      
      fprintf(file_ptr,"%d\n", point->getPosition());
  }

  fprintf(file_ptr,"\n");

  fclose(file_ptr);
}

void MeshIoVtk::printCellIdVtk(const char* nome_var)
{

  std::string ss = this->fi_popNextName(this->m_filenumVtk-1, ".vtk");

  FILE * file_ptr = fopen(ss.c_str(), "a");
  
  FEPIC_ASSERT(file_ptr != NULL, "could not open the file", std::runtime_error);
  
  int  n_cd   = getNumDivisions(m_mesh->cellType()); // num divisions
  const int num_cells_total = m_mesh->numCellsTotal();
  const int num_cells = m_mesh->numCells();
  
  if (m_add_cell_scalar_vtk_n_calls==0)
  {
    fprintf(file_ptr,"CELL_DATA %d\n\n", num_cells*n_cd);
  }
  m_add_cell_scalar_vtk_n_calls++;

  fprintf(file_ptr,"SCALARS %s int\nLOOKUP_TABLE default\n", nome_var);

  for (int i=0; i<num_cells_total; ++i)
  {
    Cell const * cell = m_mesh->getCellPtr(i);
    FEPIC_CHECK(cell != NULL, "a bug here ...", std::runtime_error);
    if (cell->isDisabled())
      continue;
    for (int j = 0; j < n_cd; ++j)
    {
      if (m_is_binary)
      {
        int tmp = reverseEndian(m_mesh->getCellId(cell) );
        fwrite(&tmp, sizeof(int), 1, file_ptr);
      }
      else       
        fprintf(file_ptr,"%d\n", m_mesh->getCellId(cell) );
    }
  }

  fprintf(file_ptr,"\n");

  fclose(file_ptr);
}


