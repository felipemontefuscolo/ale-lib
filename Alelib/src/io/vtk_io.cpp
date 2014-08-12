#include "vtk_io.hpp"
#include <tr1/array>
#include "../util/misc2.hpp"
#include "../shape_functions/parametric_pts.hpp"
#include "../shape_functions/default_map.hpp"
#include "../dof_mapper/reorder.hpp"

namespace alelib
{

template<ECellType CT>
struct VtkTraits {};

template<> struct VtkTraits<EDGE>        {static int const tag =  3;};
template<> struct VtkTraits<TRIANGLE>    {static int const tag =  5;};
template<> struct VtkTraits<QUADRANGLE>  {static int const tag =  9;};
template<> struct VtkTraits<TETRAHEDRON> {static int const tag = 10;};
template<> struct VtkTraits<HEXAHEDRON>  {static int const tag = 12;};



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

template<ECellType CT>
void MeshIoVtk<CT>::splitMeshEdges(unsigned n_parts)
{
  static ECellType const FT = CTypeTraits<CT>::EFacetType;

  if (n_parts < 1)
    return;

  m_subdivs_lvl = n_parts;

  // divide the master triangle
  subCells<CT>(n_parts, m_subcells);
  genParametricPts<CT>(n_parts, m_ref_cpts);
  genParametricPts<FT>(n_parts, m_ref_fpts);
}


template<ECellType ECT>
void MeshIoVtk<ECT>::fi_printPointVtk(double x[], FILE *fp) const
{
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

template<ECellType ECT>
void MeshIoVtk<ECT>::fi_printCellVtk(index_t const* ids, FILE *fp) const
{
  //int const n = m_subdivs_lvl;
  int const n_nds   = MeshT::verts_per_cell; // per sub-cell
  int const n_subcs = CTypeTraits<ECT>::n_sub_cells(m_subdivs_lvl);

  if (m_is_binary)
  {
    int const tmp = reverseEndian(n_nds);
    for (int i = 0; i < n_subcs; ++i)
    {
      fwrite(&tmp, sizeof(index_t), 1, fp);
      for (int j = 0; j < n_nds; ++j)
      {
        index_t const tmp = reverseEndian(ids[m_subcells[i*n_nds + j]]);
        fwrite(&tmp, sizeof(index_t), 1, fp);
      }
    }
  }
  else
  {
    for (int i = 0; i < n_subcs; ++i)
    {
      fprintf(fp, "%d", n_nds);
      for (int j = 0; j < n_nds; ++j)
        fprintf(fp," %d", ids[m_subcells[i*n_nds + j]]);
      fprintf(fp,"\n");
    }
  }
}

template<ECellType ECT>
void MeshIoVtk<ECT>::fi_printNodeData(double tmp[], int ncomps, FILE *file_ptr) const
{
  if (m_is_binary)
  {
    for (int k = 0; k < ncomps; ++k)
      tmp[k] = reverseEndian(tmp[k]);
    fwrite(tmp, sizeof(Real), ncomps, file_ptr);
  }
  else
  {
    for (int k = 0; k < ncomps-1; ++k)
      fprintf(file_ptr,"%.14e ", tmp[k]);
    fprintf(file_ptr,"%.14e\n", tmp[ncomps-1]);
  }
}


/// Print mesh in vtk file format. In this version, if cell subdivisions are requested, it is used linear interpolation
/// to compute high-order nodes.
/// @param time if it's not null, print the content of (*time) in the file in a way that VisIt can recognize it.
///
template<ECellType CT>
void MeshIoVtk<CT>::writeMesh(double* time)
{
  ///
  /// DISABLE entities are not printed.
  ///

  this->m_add_node_scalar_n_calls=0;
  this->m_add_cell_scalar_n_calls=0;

  std::string outname = this->paddedName(this->m_filenum, ".vtk", m_name_padd);
  ++m_filenum;

  FILE *file_ptr = fopen(outname.c_str(), m_is_binary ? "wb" : "w");

  fprintf(file_ptr, "# vtk DataFile Version 2.0\n"
                    "Created by Alelib\n");
  if (m_is_binary)
    fprintf(file_ptr, "BINARY\n");
  else
    fprintf(file_ptr, "ASCII\n");
  fprintf(file_ptr, "DATASET UNSTRUCTURED_GRID\n");

  if (time)
  {
    fprintf(file_ptr, "FIELD FieldData 1\n");
    fprintf(file_ptr, "TIME 1 1 double\n");
    if (m_is_binary)
    {
      double const aux = reverseEndian(*time);
      fwrite(&aux, sizeof(double), 1, file_ptr);
    }
    else
      fprintf(file_ptr, "%f\n", *time);
  }

  index_t n_pts = m_mesh->numVertices() + CTypeTraits<CT>::np_in_ridge(m_subdivs_lvl) * m_mesh->numRidges() +
                                          CTypeTraits<CT>::np_in_facet(m_subdivs_lvl) * m_mesh->numFacets() +
                                          CTypeTraits<CT>::np_in_cell (m_subdivs_lvl) * m_mesh->numCells();

  //
  //  PRINT POINTS
  //


  fprintf(file_ptr, "POINTS %u double\n", n_pts);

  // Points on vertices
  {
    VertexH v = m_mesh->vertexBegin();
    VertexH vend = m_mesh->vertexEnd();

    for (; v!=vend ; ++v)
    {
      if (v.isDisabled(m_mesh))
        continue;
      double x[3] = {0.,0.,0.};
      v.coord(m_mesh, x);
      fi_printPointVtk(x, file_ptr);
    }
  }

  // Points on ridges (3d cells)
  {
    int const nvir = CTypeTraits<CT>::np_in_ridge(m_subdivs_lvl); // number of pts that will be created
    if (MeshT::cell_dim > 2 && nvir > 0)
    {
      Point p[2];
      Real x[3];
      RidgeH r = m_mesh->ridgeBegin();
      RidgeH rend = m_mesh->ridgeEnd();
      for (; r != rend; ++r)
      {
        if (r.isDisabled(m_mesh))
          continue;
        r.points(m_mesh,p);
        for (Real i = 0; i < nvir; ++i)
        {
          Real s = (i+1.)/(nvir+1);
          x[0] = (1.-s)*p[0][0] + s*p[1][0] ;
          x[1] = (1.-s)*p[0][1] + s*p[1][1] ;
          x[2] = (1.-s)*p[0][2] + s*p[1][2] ;

          fi_printPointVtk(x, file_ptr);
        }
      }
    }
  }

  // Points inside facets
  {
    int const n_new_pts = CTypeTraits<CT>::np_in_facet(m_subdivs_lvl); // number of pts that will be created
    if (MeshT::cell_dim > 1 && n_new_pts > 0)
    {
      static const ECellType FT = CTypeTraits<CT>::EFacetType;

      Point vts[4]; // vertices of the facet

      std::vector<Point> x(n_new_pts); // new points

      FacetH f = m_mesh->facetBegin();
      FacetH fend = m_mesh->facetEnd();
      for (; f != fend; ++f)
      {
        if (f.isDisabled(m_mesh))
          continue;

        f.points(m_mesh, vts);

        int const first_index = (MeshT::cell_dim-1) *
                                (  MeshT::verts_per_facet +
                                   (MeshT::cell_dim <= 2 ? 0 : MeshT::ridges_per_facet * CTypeTraits<CT>::np_in_ridge(m_subdivs_lvl)) );

        CTypeTraits<FT>::master_to_real(n_new_pts, m_ref_fpts.data() + first_index, vts, x.data());

        for (int i = 0; i < n_new_pts; ++i)
          fi_printPointVtk(x[i].coord(), file_ptr);
      }
    }
  } // Points inside facets

  // Points inside cells
  {
    int const n_new_pts = CTypeTraits<CT>::np_in_cell(m_subdivs_lvl); // number of pts that will be created
    if (n_new_pts > 0)
    {
      Point vts[8]; // vertices of the cell

      std::vector<Point> x(n_new_pts); // new points

      CellH c = m_mesh->cellBegin();
      CellH cend = m_mesh->cellEnd();
      for (; c != cend; ++c)
      {
        if (c.isDisabled(m_mesh))
          continue;

        c.points(m_mesh, vts);

        int const first_index =  MeshT::cell_dim *
                                  (  MeshT::verts_per_cell +
                                      (MeshT::cell_dim <= 2 ? 0 : MeshT::ridges_per_cell * CTypeTraits<CT>::np_in_ridge(m_subdivs_lvl)) +
                                      (MeshT::cell_dim <= 1 ? 0 : MeshT::facets_per_cell * CTypeTraits<CT>::np_in_facet(m_subdivs_lvl)) );

        CTypeTraits<CT>::master_to_real(n_new_pts, m_ref_cpts.data() + first_index, vts, x.data());

        for (int i = 0; i < n_new_pts; ++i)
          fi_printPointVtk(x[i].coord(), file_ptr);
      }
    }
  } // Points inside cells



  //
  //  PRINT CELLS
  //

  int const n_cd   = CTypeTraits<CT>::n_sub_cells(m_subdivs_lvl);    // number of subcells
  int const n_pseudo_cells = m_mesh->numCells() * n_cd;
  //int const n_cells_total  = m_mesh->numCellsTotal();
  int const np_in_ridge = CTypeTraits<CT>::np_in_ridge(m_subdivs_lvl);
  int const np_in_facet = CTypeTraits<CT>::np_in_facet(m_subdivs_lvl);
  int const np_in_cell  = CTypeTraits<CT>::np_in_cell(m_subdivs_lvl);
  int const np_per_cell = CTypeTraits<CT>::np_per_cell(m_subdivs_lvl);
  int const n_vertices = m_mesh->numVertices();
  int const n_ridges   = m_mesh->numRidges();
  int const n_facets   = m_mesh->numFacets();
  //int const n_cells    = m_mesh->numCells();
  std::vector<index_t> dofs;
  dofs.reserve(np_per_cell);


  /// print cells
  fprintf(file_ptr,"\nCELLS %d %d\n", n_pseudo_cells, (MeshT::verts_per_cell + 1)*n_pseudo_cells);

  //printf("DEBUG n_cells_total = %d\n", n_cells_total);
  CellH c = m_mesh->cellBegin();
  CellH cend = m_mesh->cellEnd();
  for (; c != cend; ++c)
  {
    if (c.isDisabled(m_mesh))
      continue;

    // getting "dofs"
    index_t vts[MeshT::verts_per_cell+1];  // +1 to avoid compiler warning about zero-size vectors
    index_t rds[MeshT::ridges_per_cell+1];
    index_t fts[MeshT::facets_per_cell+1];

    // vertices dofs
    c.verticesContigId(m_mesh, vts);
    for (int i = 0; i < MeshT::verts_per_cell; ++i)
      dofs.push_back(vts[i]);

    // ridges dofs
    if (MeshT::cell_dim > 2 && np_in_ridge > 0)
    {
      index_t const offset = n_vertices;
      c.ridgesContigId(m_mesh, rds);
      for (int i = 0; i < MeshT::ridges_per_cell; ++i)
        for (int j = 0; j < np_in_ridge; ++j)
          dofs.push_back(offset + np_in_ridge*rds[i] + j);
    }

    // facets dofs
    if (MeshT::cell_dim > 1 && np_in_facet > 0)
    {
      index_t const offset = n_vertices + n_ridges*np_in_ridge;
      c.facetsContigId(m_mesh, fts);
      for (int i = 0; i < MeshT::facets_per_cell; ++i)
        for (int j = 0; j < np_in_facet; ++j)
          dofs.push_back(offset + np_in_facet*fts[i] + j);
    }

    // cell dofs
    if (np_in_cell > 0)
    {
      index_t const offset = n_vertices + n_ridges*np_in_ridge + n_facets*np_in_facet;
      index_t const id = c.contiguousId(m_mesh);
      for (int j = 0; j < np_in_cell; ++j)
        dofs.push_back(offset + np_in_cell*id + j);
    }

    // AQUIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
    reorderDofsLagrange<CT,index_t>(m_mesh, c, m_subdivs_lvl, 1, dofs.data());

    fi_printCellVtk(dofs.data(), file_ptr);

    dofs.clear();
  }

  // printing types
  int type = (int) VtkTraits<CT>::tag;
  fprintf(file_ptr,"\nCELL_TYPES %d\n", n_pseudo_cells);
  unsigned long counter = 0;
  c = m_mesh->cellBegin();
  cend = m_mesh->cellEnd();
  for (; c != cend; ++c)
  {
    if (c.isDisabled(m_mesh))
      continue;

    for (int i = 0; i < n_cd; ++i)
    {
      if (m_is_binary)
      {
        int tmp = reverseEndian(type);
        fwrite(&tmp, sizeof(int), 1, file_ptr);
      }
      else
      {
        if (((++counter)%50) == 0)
          fprintf(file_ptr, "%d\n", type);
        else
          fprintf(file_ptr, "%d ", type);
      }
    }
  }
  fprintf(file_ptr,"\n\n");

  fclose(file_ptr);
}


/// Print mesh in vtk file format. In this version, the used pass the mesh coordinates (including high-order nodes)
/// as function argument in the same way as variables in addNodalScalarField function.
/// @param time if it's not null, print the content of (*time) in the file in a way that VisIt can recognize it.
///
template<ECellType CT>
void MeshIoVtk<CT>::writeMesh(DefaultGetDataVtk const& data, double* time)
{
  ///
  /// DISABLE entities are not printed.
  ///

  this->m_add_node_scalar_n_calls=0;
  this->m_add_cell_scalar_n_calls=0;

  std::string outname = this->paddedName(this->m_filenum, ".vtk", m_name_padd);
  ++m_filenum;

  FILE *file_ptr = fopen(outname.c_str(), m_is_binary ? "wb" : "w");

  fprintf(file_ptr, "# vtk DataFile Version 2.0\n"
                    "Created by Alelib\n");
  if (m_is_binary)
    fprintf(file_ptr, "BINARY\n");
  else
    fprintf(file_ptr, "ASCII\n");
  fprintf(file_ptr, "DATASET UNSTRUCTURED_GRID\n");

  if (time)
  {
    fprintf(file_ptr, "FIELD FieldData 1\n");
    fprintf(file_ptr, "TIME 1 1 double\n");
    if (m_is_binary)
    {
      double const aux = reverseEndian(*time);
      fwrite(&aux, sizeof(double), 1, file_ptr);
    }
    else
      fprintf(file_ptr, "%f\n", *time);
  }

  index_t n_pts = m_mesh->numVertices() + CTypeTraits<CT>::np_in_ridge(m_subdivs_lvl) * m_mesh->numRidges() +
                                          CTypeTraits<CT>::np_in_facet(m_subdivs_lvl) * m_mesh->numFacets() +
                                          CTypeTraits<CT>::np_in_cell (m_subdivs_lvl) * m_mesh->numCells();

  //
  //  PRINT POINTS
  //


  int const n_comps = data.numComps()==2 ? 3 : data.numComps(); // this is because VisIt does not support 2d variables :(
  ALELIB_ASSERT(n_comps>=1 && n_comps<4, "invalid custom coordinates", std::invalid_argument);
  double tmp[3] = {0.,0.,0.}; // space dimensional is always at most 3

  fprintf(file_ptr, "POINTS %u double\n", n_pts);

  // Points on vertices
  {
    VertexH v = m_mesh->vertexBegin();
    VertexH vend = m_mesh->vertexEnd();

    for (; v!=vend ; ++v)
    {
      if (v.isDisabled(m_mesh))
        continue;
      data.getData(v.id(m_mesh), tmp);
      fi_printNodeData(tmp, sizeof(tmp)/sizeof(double), file_ptr);
    }
  }

  // Points on ridges (3d cells)
  {
    int const n = m_subdivs_lvl;
    int const nvir = CTypeTraits<CT>::np_in_ridge(n); // number of pts that will be created
    if (MeshT::cell_dim > 2 && nvir > 0)
    {
      RidgeH r = m_mesh->ridgeBegin();
      RidgeH rend = m_mesh->ridgeEnd();
      for (; r != rend; ++r)
      {
        if (r.isDisabled(m_mesh))
          continue;

        int const pos = r.localId(m_mesh);
        int const first_dof = MeshT::verts_per_cell + (n-1)*pos;
        for (Real i = 0; i < nvir; ++i)
        {
          Real * x_ref = &m_ref_cpts[MeshT::cell_dim*(first_dof + i)];
          data.getData(x_ref, r.icell(m_mesh).id(m_mesh), first_dof + i, tmp);
          fi_printNodeData(tmp, sizeof(tmp)/sizeof(double), file_ptr);
        }
      }
    }
  }

  // Points inside facets
  {
    int const n = m_subdivs_lvl;
    int const n_new_pts = CTypeTraits<CT>::np_in_facet(n); // number of pts that will be created
    if (MeshT::cell_dim > 1 && n_new_pts > 0)
    {
  //    static const ECellType FT = CTypeTraits<CT>::EFacetType;

      FacetH f = m_mesh->facetBegin();
      FacetH fend = m_mesh->facetEnd();
      for (; f != fend; ++f)
      {
        if (f.isDisabled(m_mesh))
          continue;

        int const pos = f.localId(m_mesh);
        int const first_dof = MeshT::verts_per_cell + (n-1)*MeshT::ridges_per_cell + n_new_pts*pos;
        for (int i = 0; i < n_new_pts; ++i)
        {
          Real * x_ref = &m_ref_cpts[MeshT::cell_dim*(first_dof + i)];
          data.getData(x_ref, f.icellSide0(m_mesh).id(m_mesh), first_dof + i, tmp);
          fi_printNodeData(tmp, sizeof(tmp)/sizeof(double), file_ptr);
        }
      }
    }
  } // Points inside facets

  // Points inside cells
  {
    int const n = m_subdivs_lvl;
    int const np_in_facet = CTypeTraits<CT>::np_in_facet(n); // number of pts that will be created
    int const n_new_pts = CTypeTraits<CT>::np_in_cell(n); // number of pts that will be created
    if (n_new_pts > 0)
    {

      CellH c = m_mesh->cellBegin();
      CellH cend = m_mesh->cellEnd();
      for (; c != cend; ++c)
      {
        if (c.isDisabled(m_mesh))
          continue;

        int const first_dof = MeshT::verts_per_cell + (n-1)*MeshT::ridges_per_cell + np_in_facet*MeshT::facets_per_cell;
        for (int i = 0; i < n_new_pts; ++i)
        {
          Real * x_ref = &m_ref_cpts[MeshT::cell_dim*(first_dof + i)];
          data.getData(x_ref, c.id(m_mesh), first_dof + i, tmp);
          fi_printNodeData(tmp, sizeof(tmp)/sizeof(double), file_ptr);
        }
      }
    }
  } // Points inside cells




  //
  //  PRINT CELLS
  //

  int const n_cd   = CTypeTraits<CT>::n_sub_cells(m_subdivs_lvl);    // number of subcells
  int const n_pseudo_cells = m_mesh->numCells() * n_cd;
  //int const n_cells_total  = m_mesh->numCellsTotal();
  int const np_in_ridge = CTypeTraits<CT>::np_in_ridge(m_subdivs_lvl);
  int const np_in_facet = CTypeTraits<CT>::np_in_facet(m_subdivs_lvl);
  int const np_in_cell  = CTypeTraits<CT>::np_in_cell(m_subdivs_lvl);
  int const np_per_cell = CTypeTraits<CT>::np_per_cell(m_subdivs_lvl);
  int const n_vertices = m_mesh->numVertices();
  int const n_ridges   = m_mesh->numRidges();
  int const n_facets   = m_mesh->numFacets();
  //int const n_cells    = m_mesh->numCells();
  std::vector<index_t> dofs;
  dofs.reserve(np_per_cell);


  /// print cells
  fprintf(file_ptr,"\nCELLS %d %d\n", n_pseudo_cells, (MeshT::verts_per_cell + 1)*n_pseudo_cells);

  //printf("DEBUG n_cells_total = %d\n", n_cells_total);
  CellH c = m_mesh->cellBegin();
  CellH cend = m_mesh->cellEnd();
  for (; c != cend; ++c)
  {
    if (c.isDisabled(m_mesh))
      continue;

    // getting "dofs"
    index_t vts[MeshT::verts_per_cell+1];  // +1 to avoid compiler warning about zero-size vectors
    index_t rds[MeshT::ridges_per_cell+1];
    index_t fts[MeshT::facets_per_cell+1];

    // vertices dofs
    c.verticesContigId(m_mesh, vts);
    for (int i = 0; i < MeshT::verts_per_cell; ++i)
      dofs.push_back(vts[i]);

    // ridges dofs
    if (MeshT::cell_dim > 2 && np_in_ridge > 0)
    {
      index_t const offset = n_vertices;
      c.ridgesContigId(m_mesh, rds);
      for (int i = 0; i < MeshT::ridges_per_cell; ++i)
        for (int j = 0; j < np_in_ridge; ++j)
          dofs.push_back(offset + np_in_ridge*rds[i] + j);
    }

    // facets dofs
    if (MeshT::cell_dim > 1 && np_in_facet > 0)
    {
      index_t const offset = n_vertices + n_ridges*np_in_ridge;
      c.facetsContigId(m_mesh, fts);
      for (int i = 0; i < MeshT::facets_per_cell; ++i)
        for (int j = 0; j < np_in_facet; ++j)
          dofs.push_back(offset + np_in_facet*fts[i] + j);
    }

    // cell dofs
    if (np_in_cell > 0)
    {
      index_t const offset = n_vertices + n_ridges*np_in_ridge + n_facets*np_in_facet;
      index_t const id = c.contiguousId(m_mesh);
      for (int j = 0; j < np_in_cell; ++j)
        dofs.push_back(offset + np_in_cell*id + j);
    }

    // AQUIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
    reorderDofsLagrange<CT,index_t>(m_mesh, c, m_subdivs_lvl, 1, dofs.data());

    fi_printCellVtk(dofs.data(), file_ptr);

    dofs.clear();
  }

  // printing types
  int type = (int) VtkTraits<CT>::tag;
  fprintf(file_ptr,"\nCELL_TYPES %d\n", n_pseudo_cells);
  unsigned long counter = 0;
  c = m_mesh->cellBegin();
  cend = m_mesh->cellEnd();
  for (; c != cend; ++c)
  {
    if (c.isDisabled(m_mesh))
      continue;

    for (int i = 0; i < n_cd; ++i)
    {
      if (m_is_binary)
      {
        int tmp = reverseEndian(type);
        fwrite(&tmp, sizeof(int), 1, file_ptr);
      }
      else
      {
        if (((++counter)%50) == 0)
          fprintf(file_ptr, "%d\n", type);
        else
          fprintf(file_ptr, "%d ", type);
      }
    }
  }
  fprintf(file_ptr,"\n\n");

  fclose(file_ptr);
}


template<ECellType CT>
void MeshIoVtk<CT>::addNodalScalarField(const char* nome_var, DefaultGetDataVtk const& data)
{

  std::string ss = this->paddedName(this->m_filenum-1, ".vtk", m_name_padd);

  FILE * file_ptr = fopen(ss.c_str(), "a");

  ALELIB_ASSERT(file_ptr != NULL, "could not open the file", std::runtime_error);
  ALELIB_ASSERT(data.numComps() > 0, "invalid number of components", std::invalid_argument);


  index_t n_pts = m_mesh->numVertices() + CTypeTraits<CT>::np_in_ridge(m_subdivs_lvl) * m_mesh->numRidges() +
                                          CTypeTraits<CT>::np_in_facet(m_subdivs_lvl) * m_mesh->numFacets() +
                                          CTypeTraits<CT>::np_in_cell (m_subdivs_lvl) * m_mesh->numCells();
  if (m_add_node_scalar_n_calls==0)
  {
    fprintf(file_ptr,"POINT_DATA %d\n", n_pts);
  }
  m_add_node_scalar_n_calls++;

  int const n_comps = data.numComps()==2 ? 3 : data.numComps(); // this is because VisIt does not support 2d variables :(
  std::vector<double> tmp(n_comps, 0.);
  //tmp.back() = 0;

  fprintf(file_ptr,"SCALARS %s double %d\n", nome_var, n_comps);
  fprintf(file_ptr,"LOOKUP_TABLE default\n");

  // Points on vertices
  {
    VertexH v = m_mesh->vertexBegin();
    VertexH vend = m_mesh->vertexEnd();

    for (; v!=vend ; ++v)
    {
      if (v.isDisabled(m_mesh))
        continue;
      data.getData(v.id(m_mesh), tmp.data());
      fi_printNodeData(tmp.data(), tmp.size(), file_ptr);
    }
  }

  // Points on ridges (3d cells)
  {
    int const n = m_subdivs_lvl;
    int const nvir = CTypeTraits<CT>::np_in_ridge(n); // number of pts that will be created
    if (MeshT::cell_dim > 2 && nvir > 0)
    {
      RidgeH r = m_mesh->ridgeBegin();
      RidgeH rend = m_mesh->ridgeEnd();
      for (; r != rend; ++r)
      {
        if (r.isDisabled(m_mesh))
          continue;

        int const pos = r.localId(m_mesh);
        int const first_dof = MeshT::verts_per_cell + (n-1)*pos;
        for (Real i = 0; i < nvir; ++i)
        {
          Real * x_ref = &m_ref_cpts[MeshT::cell_dim*(first_dof + i)];
          data.getData(x_ref, r.icell(m_mesh).id(m_mesh), first_dof + i, tmp.data());
          fi_printNodeData(tmp.data(), tmp.size(), file_ptr);
        }
      }
    }
  }

  // Points inside facets
  {
    int const n = m_subdivs_lvl;
    int const n_new_pts = CTypeTraits<CT>::np_in_facet(n); // number of pts that will be created
    if (MeshT::cell_dim > 1 && n_new_pts > 0)
    {
  //    static const ECellType FT = CTypeTraits<CT>::EFacetType;

      FacetH f = m_mesh->facetBegin();
      FacetH fend = m_mesh->facetEnd();
      for (; f != fend; ++f)
      {
        if (f.isDisabled(m_mesh))
          continue;

        int const pos = f.localId(m_mesh);
        int const first_dof = MeshT::verts_per_cell + (n-1)*MeshT::ridges_per_cell + n_new_pts*pos;
        for (int i = 0; i < n_new_pts; ++i)
        {
          Real * x_ref = &m_ref_cpts[MeshT::cell_dim*(first_dof + i)];
          data.getData(x_ref, f.icellSide0(m_mesh).id(m_mesh), first_dof + i, tmp.data());
          fi_printNodeData(tmp.data(), tmp.size(), file_ptr);
        }
      }
    }
  } // Points inside facets

  // Points inside cells
  {
    int const n = m_subdivs_lvl;
    int const np_in_facet = CTypeTraits<CT>::np_in_facet(n); // number of pts that will be created
    int const n_new_pts = CTypeTraits<CT>::np_in_cell(n); // number of pts that will be created
    if (n_new_pts > 0)
    {

      CellH c = m_mesh->cellBegin();
      CellH cend = m_mesh->cellEnd();
      for (; c != cend; ++c)
      {
        if (c.isDisabled(m_mesh))
          continue;

        int const first_dof = MeshT::verts_per_cell + (n-1)*MeshT::ridges_per_cell + np_in_facet*MeshT::facets_per_cell;
        for (int i = 0; i < n_new_pts; ++i)
        {
          Real * x_ref = &m_ref_cpts[MeshT::cell_dim*(first_dof + i)];
          data.getData(x_ref, c.id(m_mesh), first_dof + i, tmp.data());
          fi_printNodeData(tmp.data(), tmp.size(), file_ptr);
        }
      }
    }
  } // Points inside cells


  fclose(file_ptr);
}


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



template class MeshIoVtk<TRIANGLE>   ;
template class MeshIoVtk<TETRAHEDRON>;

}





