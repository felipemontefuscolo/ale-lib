#include "vtk_io.hpp"
//#include <tr1/array>
#include "../util/misc2.hpp"
#include "../shape_functions/parametric_pts.hpp"
#include "../shape_functions/default_map.hpp"
#include "../dof_mapper/reorder.hpp"

namespace alelib
{



#if 0

void MeshIoVtk::addNodalVectorField(const char* nome_var, DefaultGetDataVtk const& data)
{

  std::string ss = this->paddedName(this->m_filenum-1, ".vtk");

  FILE * file_ptr = fopen(ss.c_str(), "a");

  ALELIB_ASSERT(file_ptr != NULL, "could not open the file", std::runtime_error);

  const int num_pts_total = m_mesh->numNodesTotal();
  const int num_pts       = m_mesh->numNodes();
  if (m_add_node_scalar_n_calls==0)
  {
    fprintf(file_ptr,"POINT_DATA %d\n", num_pts);
  }
  m_add_node_scalar_n_calls++;

  fprintf(file_ptr,"VECTORS %s double\n", nome_var);

  double vec[3]={0,0,0};
  int vec_size = data.vec_ncomps();
  for (int i=0; i<num_pts_total; ++i)
  {
    if (!(m_mesh->getNodePtr(i)->isDisabled()))
    {
      data.get_vec(i, vec);
      ALELIB_CHECK(vec_size>=0 && vec_size<4, "invalid vtk vector", std::invalid_argument);

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


void MeshIoVtk::addCellScalarField(const char* nome_var, DefaultGetDataVtk const& data)
{

  std::string ss = this->paddedName(this->m_filenum-1, ".vtk");

  FILE * file_ptr = fopen(ss.c_str(), "a");

  ALELIB_ASSERT(file_ptr != NULL, "could not open the file", std::runtime_error);

  int  n_cd   = getNumDivisions(m_mesh->cellType()); // num divisions
  const int num_cells_total = m_mesh->numCellsTotal();
  const int num_cells = m_mesh->numCells();
  if (m_add_cell_scalar_n_calls==0)
  {
    fprintf(file_ptr,"CELL_DATA %d\n", num_cells*n_cd);
  }
  m_add_cell_scalar_n_calls++;

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

  std::string ss = this->paddedName(this->m_filenum-1, ".vtk");

  FILE * file_ptr = fopen(ss.c_str(), "a");

  ALELIB_ASSERT(file_ptr != NULL, "could not open the file", std::runtime_error);

  const int num_pts_total = m_mesh->numNodesTotal();
  const int num_pts = m_mesh->numNodes();

  if (m_add_node_scalar_n_calls==0)
  {
    fprintf(file_ptr,"POINT_DATA %d\n\n", num_pts);
  }
  m_add_node_scalar_n_calls++;

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

  std::string ss = this->paddedName(this->m_filenum-1, ".vtk");

  FILE * file_ptr = fopen(ss.c_str(), "a");

  ALELIB_ASSERT(file_ptr != NULL, "could not open the file", std::runtime_error);

  int  n_cd   = getNumDivisions(m_mesh->cellType()); // num divisions
  const int num_cells_total = m_mesh->numCellsTotal();
  const int num_cells = m_mesh->numCells();

  if (m_add_cell_scalar_n_calls==0)
  {
    fprintf(file_ptr,"CELL_DATA %d\n\n", num_cells*n_cd);
  }
  m_add_cell_scalar_n_calls++;

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

  std::string ss = this->paddedName(this->m_filenum-1, ".vtk");

  FILE * file_ptr = fopen(ss.c_str(), "a");

  ALELIB_ASSERT(file_ptr != NULL, "could not open the file", std::runtime_error);

  const int num_pts = m_mesh->numNodes();
  if (m_add_node_scalar_n_calls==0)
  {
    fprintf(file_ptr,"POINT_DATA %d\n\n", num_pts);
  }
  m_add_node_scalar_n_calls++;

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

  std::string ss = this->paddedName(this->m_filenum-1, ".vtk");

  FILE * file_ptr = fopen(ss.c_str(), "a");

  ALELIB_ASSERT(file_ptr != NULL, "could not open the file", std::runtime_error);

  const int num_pts = m_mesh->numNodes();
  if (m_add_node_scalar_n_calls==0)
  {
    fprintf(file_ptr,"POINT_DATA %d\n\n", num_pts);
  }
  m_add_node_scalar_n_calls++;

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

  std::string ss = this->paddedName(this->m_filenum-1, ".vtk");

  FILE * file_ptr = fopen(ss.c_str(), "a");

  ALELIB_ASSERT(file_ptr != NULL, "could not open the file", std::runtime_error);

  const int num_pts = m_mesh->numNodes();
  if (m_add_node_scalar_n_calls==0)
  {
    fprintf(file_ptr,"POINT_DATA %d\n\n", num_pts);
  }
  m_add_node_scalar_n_calls++;

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

  std::string ss = this->paddedName(this->m_filenum-1, ".vtk");

  FILE * file_ptr = fopen(ss.c_str(), "a");

  ALELIB_ASSERT(file_ptr != NULL, "could not open the file", std::runtime_error);

  int  n_cd   = getNumDivisions(m_mesh->cellType()); // num divisions
  const int num_cells_total = m_mesh->numCellsTotal();
  const int num_cells = m_mesh->numCells();

  if (m_add_cell_scalar_n_calls==0)
  {
    fprintf(file_ptr,"CELL_DATA %d\n\n", num_cells*n_cd);
  }
  m_add_cell_scalar_n_calls++;

  fprintf(file_ptr,"SCALARS %s int\nLOOKUP_TABLE default\n", nome_var);

  for (int i=0; i<num_cells_total; ++i)
  {
    Cell const * cell = m_mesh->getCellPtr(i);
    ALELIB_CHECK(cell != NULL, "a bug here ...", std::runtime_error);
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


#endif




}





