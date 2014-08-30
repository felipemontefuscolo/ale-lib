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
#include <tr1/array>
#include "vtk_io.hpp"
#include "path_handle.hpp"
#include "Alelib/src/util/assert.hpp"
#include "Alelib/src/util/misc2.hpp"
#include "Alelib/src/mesh/mesh.hpp"
#include "Alelib/src/mesh/enums.hpp"
#include "Alelib/src/shape_functions/parametric_pts.hpp"
#include "Alelib/src/shape_functions/default_map.hpp"
#include "Alelib/src/dof_mapper/reorder.hpp"


namespace alelib
{

namespace internal
{
  template<ECellType CT>
  struct VtkTraits {};

  template<> struct VtkTraits<EDGE>        {static int const tag =  3;};
  template<> struct VtkTraits<TRIANGLE>    {static int const tag =  5;};
  template<> struct VtkTraits<QUADRANGLE>  {static int const tag =  9;};
  template<> struct VtkTraits<TETRAHEDRON> {static int const tag = 10;};
  template<> struct VtkTraits<HEXAHEDRON>  {static int const tag = 12;};  
}



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

  /// This function is called when printing higher-order meshes.
  /// The default behaviour is to printing zero at high-order nodes.
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


template<typename Mesh_t>
class MeshIoVtk : public iPathHandle
{

protected:
  bool m_is_binary;
  int  m_filenum;
  int  m_add_node_scalar_n_calls;
  int  m_add_cell_scalar_n_calls;
  int  m_spacedim;
  Mesh_t const* m_mesh;
  int m_subdivs_lvl;  // < 2 means do nothing, otherwise means the number of parts that an edge is divided
  std::vector<int> m_subcells;
  std::vector<Real> m_ref_cpts;
  std::vector<Real> m_ref_fpts;

public:

  typedef Mesh_t MeshT;
  typedef typename MeshT::VertexH VertexH;
  typedef typename MeshT::RidgeH RidgeH;
  typedef typename MeshT::FacetH FacetH;
  typedef typename MeshT::CellH CellH;
  
  
  static const ECellType CellType = MeshT::CellType;

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


private:
  template<typename T>
  static T reverseEndian( T aa)
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

  void fi_printPointVtk(double x[], FILE *fp) const
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

  void fi_printCellVtk(index_t const* ids, FILE *fp) const
  {
    //int const n = m_subdivs_lvl;
    int const n_nds   = MeshT::verts_per_cell; // per sub-cell
    int const n_subcs = CTypeTraits<CellType>::n_sub_cells(m_subdivs_lvl);

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

        //// std::cout AQUI IMPRIMIR
        //printf("%d", n_nds);
        //for (int j = 0; j < n_nds; ++j)
        //  printf(" %d", ids[m_subcells[i*n_nds + j]]);
        //printf("\n");
      }
    }
  }

  void fi_printNodeData(double tmp[], int ncomps, FILE *file_ptr) const
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

public:

  void splitMeshEdges(unsigned n_parts)
  {
    static ECellType const FT = CTypeTraits<CellType>::EFacetType;

    if (n_parts < 1)
      return;

    m_subdivs_lvl = n_parts;

    // divide the master triangle
    subCells<CellType>(n_parts, m_subcells);
    genParametricPts<CellType>(n_parts, m_ref_cpts);
    genParametricPts<FT>(n_parts, m_ref_fpts);
  }




/// Print mesh in vtk file format. In this version, if cell subdivisions are requested, it is used linear interpolation
/// to compute high-order nodes.
/// @param time if it's not null, print the content of (*time) in the file in a way that VisIt can recognize it.
///
void writeMesh(double* time = NULL)
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

  index_t n_pts = m_mesh->numVertices() + CTypeTraits<CellType>::np_in_ridge(m_subdivs_lvl) * m_mesh->numRidges() +
                                          CTypeTraits<CellType>::np_in_facet(m_subdivs_lvl) * m_mesh->numFacets() +
                                          CTypeTraits<CellType>::np_in_cell (m_subdivs_lvl) * m_mesh->numCells();

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
    int const nvir = CTypeTraits<CellType>::np_in_ridge(m_subdivs_lvl); // number of pts that will be created
    if (MeshT::cell_dim > 2 && nvir > 0)
    {
      Point<3> p[2];
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
    int const n_new_pts = CTypeTraits<CellType>::np_in_facet(m_subdivs_lvl); // number of pts that will be created
    if (MeshT::cell_dim > 1 && n_new_pts > 0)
    {
      static const ECellType FT = CTypeTraits<CellType>::EFacetType;

      Point<3> vts[4]; // vertices of the facet

      std::vector<Point<3> > x(n_new_pts); // new points

      FacetH f = m_mesh->facetBegin();
      FacetH fend = m_mesh->facetEnd();
      for (; f != fend; ++f)
      {
        if (f.isDisabled(m_mesh))
          continue;

        f.points(m_mesh, vts);

        int const first_index = (MeshT::cell_dim-1) *
                                (  MeshT::verts_per_facet +
                                   (MeshT::cell_dim <= 2 ? 0 : MeshT::ridges_per_facet * CTypeTraits<CellType>::np_in_ridge(m_subdivs_lvl)) );

        CTypeTraits<FT>::master_to_real(n_new_pts, m_ref_fpts.data() + first_index, vts, x.data());

        for (int i = 0; i < n_new_pts; ++i)
          fi_printPointVtk(x[i].coord(), file_ptr);
      }
    }
  } // Points inside facets

  // Points inside cells
  {
    int const n_new_pts = CTypeTraits<CellType>::np_in_cell(m_subdivs_lvl); // number of pts that will be created
    if (n_new_pts > 0)
    {
      Point<3> vts[8]; // vertices of the cell

      std::vector<Point<3> > x(n_new_pts); // new points

      CellH c = m_mesh->cellBegin();
      CellH cend = m_mesh->cellEnd();
      for (; c != cend; ++c)
      {
        if (c.isDisabled(m_mesh))
          continue;

        c.points(m_mesh, vts);

        int const first_index =  MeshT::cell_dim *
                                  (  MeshT::verts_per_cell +
                                      (MeshT::cell_dim <= 2 ? 0 : MeshT::ridges_per_cell * CTypeTraits<CellType>::np_in_ridge(m_subdivs_lvl)) +
                                      (MeshT::cell_dim <= 1 ? 0 : MeshT::facets_per_cell * CTypeTraits<CellType>::np_in_facet(m_subdivs_lvl)) );

        CTypeTraits<CellType>::master_to_real(n_new_pts, m_ref_cpts.data() + first_index, vts, x.data());

        for (int i = 0; i < n_new_pts; ++i)
          fi_printPointVtk(x[i].coord(), file_ptr);
      }
    }
  } // Points inside cells



  //
  //  PRINT CELLS
  //

  int const n_cd   = CTypeTraits<CellType>::n_sub_cells(m_subdivs_lvl);    // number of subcells
  int const n_pseudo_cells = m_mesh->numCells() * n_cd;
  //int const n_cells_total  = m_mesh->numCellsTotal();
  int const np_in_ridge = CTypeTraits<CellType>::np_in_ridge(m_subdivs_lvl);
  int const np_in_facet = CTypeTraits<CellType>::np_in_facet(m_subdivs_lvl);
  int const np_in_cell  = CTypeTraits<CellType>::np_in_cell(m_subdivs_lvl);
  int const np_per_cell = CTypeTraits<CellType>::np_per_cell(m_subdivs_lvl);
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
    reorderDofsLagrange<MeshT,index_t>(m_mesh, c, m_subdivs_lvl, 1, dofs.data());

    fi_printCellVtk(dofs.data(), file_ptr);

    dofs.clear();
  }

  // printing types
  int type = (int) internal::VtkTraits<CellType>::tag;
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
void writeMesh(DefaultGetDataVtk const& data, double* time = NULL)
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

  index_t n_pts = m_mesh->numVertices() + CTypeTraits<CellType>::np_in_ridge(m_subdivs_lvl) * m_mesh->numRidges() +
                                          CTypeTraits<CellType>::np_in_facet(m_subdivs_lvl) * m_mesh->numFacets() +
                                          CTypeTraits<CellType>::np_in_cell (m_subdivs_lvl) * m_mesh->numCells();

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
    int const nvir = CTypeTraits<CellType>::np_in_ridge(n); // number of pts that will be created
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
    int const n_new_pts = CTypeTraits<CellType>::np_in_facet(n); // number of pts that will be created
    if (MeshT::cell_dim > 1 && n_new_pts > 0)
    {
  //    static const ECellType FT = CTypeTraits<CellType>::EFacetType;

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
    int const np_in_facet = CTypeTraits<CellType>::np_in_facet(n); // number of pts that will be created
    int const n_new_pts = CTypeTraits<CellType>::np_in_cell(n); // number of pts that will be created
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

  int const n_cd   = CTypeTraits<CellType>::n_sub_cells(m_subdivs_lvl);    // number of subcells
  int const n_pseudo_cells = m_mesh->numCells() * n_cd;
  //int const n_cells_total  = m_mesh->numCellsTotal();
  int const np_in_ridge = CTypeTraits<CellType>::np_in_ridge(m_subdivs_lvl);
  int const np_in_facet = CTypeTraits<CellType>::np_in_facet(m_subdivs_lvl);
  int const np_in_cell  = CTypeTraits<CellType>::np_in_cell(m_subdivs_lvl);
  int const np_per_cell = CTypeTraits<CellType>::np_per_cell(m_subdivs_lvl);
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
    reorderDofsLagrange<MeshT,index_t>(m_mesh, c, m_subdivs_lvl, 1, dofs.data());

    fi_printCellVtk(dofs.data(), file_ptr);
    //for (int i = 0; i < (int)dofs.size(); ++i)
    //  std::cout << dofs[i] << " ";
    //std::cout << std::endl;

    dofs.clear();
  }

  // printing types
  int type = (int) internal::VtkTraits<CellType>::tag;
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

void addNodalScalarField(const char* nome_var, DefaultGetDataVtk const& data)
{

  std::string ss = this->paddedName(this->m_filenum-1, ".vtk", m_name_padd);

  FILE * file_ptr = fopen(ss.c_str(), "a");

  ALELIB_ASSERT(file_ptr != NULL, "could not open the file", std::runtime_error);
  ALELIB_ASSERT(data.numComps() > 0, "invalid number of components", std::invalid_argument);


  index_t n_pts = m_mesh->numVertices() + CTypeTraits<CellType>::np_in_ridge(m_subdivs_lvl) * m_mesh->numRidges() +
                                          CTypeTraits<CellType>::np_in_facet(m_subdivs_lvl) * m_mesh->numFacets() +
                                          CTypeTraits<CellType>::np_in_cell (m_subdivs_lvl) * m_mesh->numCells();
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
    int const nvir = CTypeTraits<CellType>::np_in_ridge(n); // number of pts that will be created
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
    int const n_new_pts = CTypeTraits<CellType>::np_in_facet(n); // number of pts that will be created
    if (MeshT::cell_dim > 1 && n_new_pts > 0)
    {
  //    static const ECellType FT = CTypeTraits<CellType>::EFacetType;

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
    int const np_in_facet = CTypeTraits<CellType>::np_in_facet(n); // number of pts that will be created
    int const n_new_pts = CTypeTraits<CellType>::np_in_cell(n); // number of pts that will be created
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

  
  void addNodalVectorField(const char* nome_var, DefaultGetDataVtk const& data);
  void addCellScalarField(const char* nome_var, DefaultGetDataVtk const& data);
  void addNodeIntVtk(const char* nome_var, DefaultGetDataVtk const& data);
  void addCellIntVtk(const char* nome_var, DefaultGetDataVtk const& data);
  void printPointTagVtk(const char* nome_var="node_labels"); //  debug
  void printPointIcellVtk(const char* nome_var="pt_icell"); //  debug
  void printPointPositionVtk(const char* nome_var="pt_ic_pos"); //  debug
  void printCellIdVtk(const char* nome_var="cell_id");

protected:

  //void fi_printPointVtk(double x[], FILE *fp) const;
  //void fi_printCellVtk(index_t const* ids, FILE *fp) const;
  //void fi_printNodeData(double x[], int ncomps, FILE *fp) const;
  //
  //static int getVtkTag(ECellType type);


};

}


#endif
