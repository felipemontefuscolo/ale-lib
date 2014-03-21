#include "mesh.hpp"
#include "handler_cell.hpp"
#include "handler_vertex.hpp"
#include "handler_facet.hpp"
#include "handler_ridge.hpp"
#include "../util/misc2.hpp"
#include "../util/assert.hpp"
#include "../util/algorithm.hpp"

namespace alelib
{

//void dummy_instantiation() {
//
//  Mesh<EDGE>       ::create(1);
//  Mesh<TRIANGLE>   ::create(1);
//  Mesh<QUADRANGLE> ::create(1);
//  Mesh<TETRAHEDRON>::create(1);
//  Mesh<HEXAHEDRON> ::create(1);
//
//}


//~ //                                       POINT, EDGE , TRIANGLE, QUADRANGLE, TETRAHEDRON, HEXAHEDRON
//~ int const _Table_cell_dim[]            = { 0  ,  1   ,  2      ,  2        ,  3         , 3         };
//~ int const _Table_vertices_per_cell[]   = { 0  ,  2   ,  3      ,  4        ,  4         , 8         };
//~ int const _Table_vertices_per_facet[]  = { 0  ,  1   ,  2      ,  2        ,  3         , 4         };
//~ int const _Table_vertices_per_ridge[]  = { 0  ,  0   ,  1      ,  1        ,  2         , 2         };
//~ int const _Table_facets_per_cell[]     = { 0  ,  2   ,  3      ,  4        ,  4         , 6         };
//~ int const _Table_ridges_per_cell[]     = { 0  ,  0   ,  3      ,  4        ,  6         , 12        };
//~ int const _Table_ridges_per_facet[]    = { 0  ,  0   ,  2      ,  2        ,  3         , 4         };

template<ECellType fept>
marray::Array<int, 2> init_tables(int n)
{
  static marray::Array<int, 2> table_fC_x_vC;
  static marray::Array<int, 2> table_vC_x_fC;
  static marray::Array<int, 2> table_fC_x_bC;
  static marray::Array<int, 2> table_bC_x_vC;
  static marray::Array<int, 2> table_bC_x_fC;

  static int counter = 0;

  if (counter == 0)
  {
    switch (fept)
    {
      case POINT: {}
      break;

      case EDGE:
      {
        table_fC_x_vC.reshape(2,1); table_fC_x_vC <<  0, 1;
        table_bC_x_vC.reshape(1,1); table_bC_x_vC <<  -1;
        table_fC_x_bC.reshape(1,1); table_fC_x_bC <<  -1;
        table_bC_x_fC.reshape(1,1); table_bC_x_fC <<  -1;
        table_vC_x_fC.reshape(2,1); table_vC_x_fC <<  0, 1;
      }
      break;

      case TRIANGLE:
      {
        table_fC_x_vC.reshape(3,2); table_fC_x_vC << 0,1,  1,2,  2,0;
        table_vC_x_fC.reshape(3,2); table_vC_x_fC << 0,2,  1,0,  2,1;
        table_fC_x_bC.reshape(3,2); table_fC_x_bC << 0,1,  1,2,  2,0;
        table_bC_x_vC.reshape(3,1); table_bC_x_vC << 0,1,2;
        table_bC_x_fC.reshape(3,2); table_bC_x_fC << 0,2,  1,0,  2,1;

      }
      break;

      case QUADRANGLE:
      {
        table_fC_x_vC.reshape(4,2); table_fC_x_vC <<  0,1,  1,2,  2,3,  3,0;
        table_vC_x_fC.reshape(4,2); table_vC_x_fC <<  0,3,  1,0,  2,1,  3,2;
        table_fC_x_bC.reshape(4,2); table_fC_x_bC <<  0,1,  1,2,  2,3,  3,0;
        table_bC_x_vC.reshape(4,1); table_bC_x_vC <<  0, 1, 2, 3;
        table_bC_x_fC.reshape(4,2); table_bC_x_fC <<  0,3,  1,0,  2,1,  3,2;
      }
      break;

      case TETRAHEDRON:
      {
        table_vC_x_fC.reshape(4,6); table_vC_x_fC << 0, 1, 2,   1, 0, 2,
                                                     0, 1, 3,   0, 1, 2,
                                                     0, 2, 3,   2, 1, 0,
                                                     1, 2, 3,   2, 0, 1;
        table_fC_x_bC.reshape(4,3); table_fC_x_bC << 0, 2, 1,
                                                     0, 5, 3,
                                                     4, 2, 3,
                                                     4, 5, 1;

        table_fC_x_vC.reshape(4,6); table_fC_x_vC << 1,0,2,   0,0,0,
                                                     0,1,3,   1,1,0,
                                                     3,2,0,   1,1,2,
                                                     2,3,1,   2,2,2;

        table_bC_x_vC.reshape(6,2); table_bC_x_vC << 0,1,  1,2,  2,0,  3,0,  3,2,  3,1;

        table_bC_x_fC.reshape(6,4); table_bC_x_fC << 0, 1, 0, 0,
                                                     0, 3, 2, 2,
                                                     0, 2, 1, 1,
                                                     2, 1, 2, 2,
                                                     3, 2, 0, 0,
                                                     1, 3, 1, 1;
      }
      break;

      case HEXAHEDRON:
      {
        table_vC_x_fC.reshape(8,6); table_vC_x_fC << 0, 1, 2, 0, 0, 0,
                                                     0, 1, 3, 3, 1, 0,
                                                     0, 3, 4, 2, 1, 0,
                                                     0, 2, 4, 1, 3, 1,
                                                     1, 2, 5, 3, 1, 0,
                                                     1, 3, 5, 2, 3, 1,
                                                     3, 4, 5, 2, 3, 2,
                                                     2, 4, 5, 2, 2, 3;

        table_fC_x_bC.reshape(6,4); table_fC_x_bC << 1, 5 , 3 , 0,
                                                     0, 4 , 8 , 2,
                                                     2, 9 , 7 , 1,
                                                     3, 6 , 10, 4,
                                                     5, 7 , 11, 6,
                                                     8, 10, 11, 9;

        table_fC_x_vC.reshape(6,8); table_fC_x_vC << 0,3,2,1,   0,0,0,0,
                                                     0,1,5,4,   1,1,0,0,
                                                     0,4,7,3,   2,1,0,1,
                                                     1,2,6,5,   2,1,0,1,
                                                     2,3,7,6,   2,2,1,1,
                                                     4,5,6,7,   2,2,2,2;

        table_bC_x_vC.reshape(12,2); table_bC_x_vC << 0,1,0,3,0,4,1,2,1,5,2,3,2,6,3,7,4,5,4,7,5,6,6,7;

        table_bC_x_fC.reshape(12,4); table_bC_x_fC << 0, 1, 3, 0,
                                                      2, 0, 3, 0,
                                                      1, 2, 3, 0,
                                                      0, 3, 2, 0,
                                                      3, 1, 3, 1,
                                                      0, 4, 1, 0,
                                                      4, 3, 3, 1,
                                                      2, 4, 2, 1,
                                                      1, 5, 2, 0,
                                                      5, 2, 3, 1,
                                                      3, 5, 2, 1,
                                                      4, 5, 2, 2;
      }
      break;

      default: {
        std::cout << "Invalid cell type" << std::endl;
        throw;
      }
    };
  }

  switch (n)
  {
    case 0: return table_fC_x_vC;
    case 1: return table_vC_x_fC;
    case 2: return table_fC_x_bC;
    case 3: return table_bC_x_vC;
    case 4: return table_bC_x_fC;
    default: {
      std::cout << "invalid table" << std::endl;
      throw;
    }
  }

}

template<ECellType CT>
Mesh<CT>::Mesh(unsigned spacedim) : m_spacedim       (spacedim),
                                    m_table_fC_x_vC  (init_tables<CT>(0)),
                                    m_table_vC_x_fC  (init_tables<CT>(1)),
                                    m_table_fC_x_bC  (init_tables<CT>(2)),
                                    m_table_bC_x_vC  (init_tables<CT>(3)),
                                    m_table_bC_x_fC  (init_tables<CT>(4))
{ ALELIB_CHECK((spacedim-1)<=2, "Invalid space dimension", std::invalid_argument); }

template<ECellType CT>
Mesh<CT>* Mesh<CT>::create(unsigned spacedim)
{
  return new MeshT(spacedim);
}

template<ECellType CT>
typename Mesh<CT>::CellH
Mesh<CT>::addCell_2D(VertexH const verts[])
{
  //index_t verts[CellT::n_verts];
  //for (unsigned i = 0; i < CellT::n_verts; ++i)
  //  verts[i] = verts_[i].id(this);

  // Some checks
  for (unsigned i = 0; i < CellT::n_verts; ++i)
  {
    ALELIB_CHECK(verts[i].id(this) < (index_t)this->numVerticesTotal(), "Vertex index: out of range", std::invalid_argument);
    ALELIB_CHECK(!verts[i].isDisabled(this), "Can not use disabled vertex", std::invalid_argument);
  }

  #define nvpc (CellT::n_verts)
  #define nfpc (CellT::n_verts)

  index_t const new_cid = pushCell();
  CellT &new_c = m_cells[new_cid];
  index_t adj_id;
  index_t const* it;

  // FIRST, SET UP ITSELF and other cells
  for (unsigned i = 0; i < nvpc; ++i)
  {
    // loop on sides

    VertexH const f_vtcs[] = {verts[i], verts[(i+1)%nvpc]};
    //index_t const f_vtcs[] = {verts[i], verts[(i+1)%nvpc]};

    VertexT const &vi = m_verts[f_vtcs[0].id(this)];
    VertexT const &vj = m_verts[f_vtcs[1].id(this)];

    new_c.verts[i] = verts[i].id(this);

    it = set_1_intersection(vi.icells.begin(), vi.icells.end(),
                            vj.icells.begin(), vj.icells.end(), &adj_id);

    if (it == &adj_id) // i.ei, no adj cell
    {
      // the new facet will always point to the new cell
      new_c.facets[i] = pushFacet(FacetT(new_cid, i, NULL_IDX, NO_TAG, NO_FLAG, 1));
    }
    else // if there is an adj cell
    {
      int side;
      bool const is_facet = CellH(adj_id).isFacet(this, f_vtcs, &side, NULL);
      ALELIB_CHECK(is_facet, "maybe something is wrong with the function CellH::isFacet", std::runtime_error);

      CellT& adj_c = m_cells[adj_id];
      new_c.facets[i] = adj_c.facets[side];
      FacetT& f = m_facets[new_c.facets[i]];
      ++(f.valency);
      // take the chance to set up the adjcent cell
      f.opp_cell = new_cid;
    }
  }

  // Setting up vertices
  for (unsigned i = 0; i < nvpc; ++i)
  {
    VertexT& vtx = m_verts[verts[i].id(this)];
    vtx.icells.insert(new_cid);
  }


  #undef nvpc
  #undef nfpc

  return CellH(this, new_cid);
}


template<ECellType CT>
void Mesh<CT>::removeCell_2D(CellH ch, bool remove_unref_verts)
{
  // Some checks
  ALELIB_CHECK(!ch.isDisabled(this) && ch.isValid(this), "This cell can not be deleted", std::invalid_argument);

  #define nvpc (CellT::n_verts)
  #define nfpc (CellT::n_verts)

  index_t const cid = ch.id(this);
  CellT const& cell = this->m_cells[cid];


  // the facets, the cell and the neighbors
  for (unsigned i = 0; i < nvpc; ++i)
  {
    FacetH fh = ch.facet(this, i);
    FacetT &f = this->m_facets[fh.id(this)];

    // Update facets and remove unref facets

    if (f.valency == 1) // boundary, delete it
    {
      m_facets.disable(fh.id(this));
    }
    else if (f.valency == 2)
    {
      if(f.icell == ch.id(this))
      {
        CellH adj = fh.icellSide1(this);
        f.icell = adj.id(this);
        f.local_id = adj.facetLocalId(this, fh);
      }
      --(f.valency);
      f.opp_cell = NULL_IDX;
    }
    else // singular facet
    {
      CellH ics[3];
      fh.first3icells(this, ics);
      int I;
      if      (ics[0] == ch) I = 0;
      else if (ics[1] == ch) I = 1;
      else if (ics[2] == ch) I = 2;
      int J=(I+1)%3, K=(I+2)%3;

      --(f.valency);
      f.icell    = ics[J].id(this);
      f.opp_cell = ics[K].id(this);
      f.local_id = ics[J].facetLocalId(this, fh);
    }
  }

  // Remove this cell from the stars
  for (unsigned i = 0; i < nvpc; ++i)
  {
    VertexT& vtx = m_verts[cell.verts[i]];
    vtx.icells.erase(cid);
  }

  if (remove_unref_verts)
  {
    for (unsigned i = 0; i < nvpc; ++i)
    {
      this->removeUnrefVertex(VertexH(cell.verts[i]));
    }
  }

  m_cells.disable(ch.id(this));

  #undef nvpc
  #undef nfpc
}


template<ECellType CT>
typename Mesh<CT>::CellH
Mesh<CT>::addCell_3D(VertexH const verts[])
{
  //index_t verts[CellT::n_verts];
  //for (unsigned i = 0; i < CellT::n_verts; ++i)
  //  verts[i] = verts_[i].id(this);

  // Some checks
  for (unsigned i = 0; i < CellT::n_verts; ++i)
  {
    ALELIB_CHECK(verts[i].id(this) < (index_t)this->numVerticesTotal(), "Vertex index: out of range", std::invalid_argument);
    ALELIB_CHECK(!verts[i].isDisabled(this), "Can not use disabled vertex", std::invalid_argument);
  }

  #define nvpc (CellT::n_verts)
  #define nfpc (CellT::n_facets)

  index_t const new_cid = pushCell();
  CellT &new_c = m_cells[new_cid];
  index_t adj_id;
  index_t const* it;

  // FIRST, SET UP ITSELF and other cells
  for (unsigned i = 0; i < nfpc; ++i)
  {
    // loop on sides

    VertexH f_vtcs[CellT::n_verts_p_facet];
    for (int j = 0; j < (int)CellT::n_verts_p_facet; ++j)
      f_vtcs[j] = verts[m_table_fC_x_vC(i,j)];

    VertexT const &vi = m_verts[f_vtcs[0].id(this)];
    VertexT const &vj = m_verts[f_vtcs[1].id(this)];
    VertexT const &vk = m_verts[f_vtcs[2].id(this)];

    it = set_1_intersection(vi.icells.begin(), vi.icells.end(),
                            vj.icells.begin(), vj.icells.end(),
                            vk.icells.begin(), vk.icells.end(), &adj_id);

    if (it == &adj_id) // i.ei, no adj cell
    {
      // the new facet will always point to the new cell
      new_c.facets[i] = pushFacet(FacetT(new_cid, i, NULL_IDX, NO_TAG, NO_FLAG, 1));
    }
    else // if there is an adj cell
    {
      int side;
      bool const is_facet = CellH(adj_id).isFacet(this, f_vtcs, &side, NULL);
      ALELIB_CHECK(is_facet, "maybe something is wrong with the function CellH::isFacet", std::runtime_error);

      CellT& adj_c = m_cells[adj_id];
      new_c.facets[i] = adj_c.facets[side];
      FacetT& f = m_facets[new_c.facets[i]];
      ++(f.valency);
      // take the chance to set up the adjcent cell
      f.opp_cell = new_cid;
    }
  }

  // setting up ridges
  for (int i = 0; i < (int)CellT::n_ridges; ++i)
  {
    VertexH r_vtcs[CellT::n_verts_p_ridge+1]; // +1 to avoid zero-size arrays
    for (int j = 0; j < (int)CellT::n_verts_p_ridge; ++j)
      r_vtcs[j] = verts[this->m_table_bC_x_vC(i,j)];

    VertexT const &vi = m_verts[r_vtcs[0].id(this)];
    VertexT const &vj = m_verts[r_vtcs[1].id(this)];

    it = set_1_intersection(vi.icells.begin(), vi.icells.end(),
                            vj.icells.begin(), vj.icells.end(), &adj_id);

    if (it == &adj_id) // i.ei, no near cell
    {
      // the new facet will always point to the new cell
      new_c.ridges[i] = pushRidge(RidgeT(new_cid, i, NO_TAG, NO_FLAG, 1));
    }
    else
    {
      int lid;
      bool const is_ridge = CellH(adj_id).isRidge(this, r_vtcs, lid);
      ALELIB_CHECK(is_ridge, "maybe something is wrong with the function CellH::isRidge", std::runtime_error);

      CellT& adj_c = m_cells[adj_id];
      new_c.ridges[i] = adj_c.ridges[abs(lid)];
      RidgeT& r = m_ridges[new_c.ridges[i]];
      ++(r.valency);
    }

  }

  // Setting up vertices
  for (unsigned i = 0; i < nvpc; ++i)
  {
    new_c.verts[i] = verts[i].id(this);
    VertexT& vtx = m_verts[verts[i].id(this)];
    vtx.icells.insert(new_cid);
  }

  #undef nvpc
  #undef nfpc

  return CellH(this, new_cid);

}







template<>
Mesh<EDGE>::CellH
Mesh<EDGE>::addCell(VertexH const [])
{ return CellH(); }

template<>
Mesh<TRIANGLE>::CellH
Mesh<TRIANGLE>::addCell(VertexH const verts[])
{ return Mesh<TRIANGLE>::addCell_2D(verts); }

template<>
Mesh<QUADRANGLE>::CellH
Mesh<QUADRANGLE>::addCell(VertexH const verts[])
{ return Mesh<QUADRANGLE>::addCell_2D(verts); }

template<>
Mesh<TETRAHEDRON>::CellH
Mesh<TETRAHEDRON>::addCell(VertexH const verts[])
{ return Mesh<TETRAHEDRON>::addCell_3D(verts); }

template<>
Mesh<HEXAHEDRON>::CellH
Mesh<HEXAHEDRON>::addCell(VertexH const verts[])
{ return Mesh<HEXAHEDRON>::addCell_3D(verts); }

// ;;
template<>
void
Mesh<EDGE>::removeCell(CellH , bool )
{  }

template<>
void
Mesh<TRIANGLE>::removeCell(CellH ch, bool remove_unref_verts)
{ return Mesh<TRIANGLE>::removeCell_2D(ch, remove_unref_verts); }

template<>
void
Mesh<QUADRANGLE>::removeCell(CellH ch, bool remove_unref_verts)
{ return Mesh<QUADRANGLE>::removeCell_2D(ch, remove_unref_verts); }

template<>
void
Mesh<TETRAHEDRON>::removeCell(CellH , bool )
{  }

template<>
void
Mesh<HEXAHEDRON>::removeCell(CellH , bool )
{  }



template class Mesh<EDGE>       ;
template class Mesh<TRIANGLE>   ;
template class Mesh<QUADRANGLE> ;
template class Mesh<TETRAHEDRON>;
template class Mesh<HEXAHEDRON> ;


} // end namespace alelib



