#ifndef ALELIB_MESH_HPP
#define ALELIB_MESH_HPP

#include <vector>
#include <map>
#include "facet.hpp"
#include "ridge.hpp"
#include "vertex.hpp"
#include "point.hpp"
#include "cell.hpp"
#include "enums.hpp"
#include "Array/array.hpp"
#include "../util/list_type.hpp"
#include "../util/initializer_list.hpp"
#include "../util/algorithm.hpp"
#include "../util/misc2.hpp"
#include "../util/assert.hpp"
#include "../util/timer.hpp"
#include "AssocVector.hpp"
#include "../io/alelib_tags.hpp"

#include <iterator>      // std::iterator, std::input_iterator_tag

#ifdef ALELIB_DEBUG_ON
# define AT__(x) [(x)]
#else
# define AT__(x) .at(x)
#endif

namespace alelib
{

template<ECellType CType, int Sdim = -1, bool SCoords_ = true>
struct DefaultTraits
{
  
  static const ECellType CellType = CType;
  
  static const int SpaceDim = Sdim!=-1 ? Sdim : (
  
                                CType == EDGE        ? 1 : (
                                CType == TRIANGLE    ? 2 : (
                                CType == QUADRANGLE  ? 2 : (
                                CType == TETRAHEDRON ? 3 : (
                                CType == HEXAHEDRON  ? 3 : (
                                                       -1
                                )))))
                              );
  
  static const bool StoreCoords = SCoords_;

  typedef Cell<CType>  CellT;   // dim = d
  typedef Facet        FacetT;  // dim = d-1
  typedef Ridge        RidgeT;  // dim = d-2
  typedef Vertex       VertexT; // dim = 0
  typedef Point        PointT;  // dim = 0  


};

template<typename Traits>
class Mesh
{
  static const ECellType CType = Traits::CellType;
  //public:
  // Some aliases
  typedef typename Traits::CellT   CellT;   // dim = d
  typedef typename Traits::FacetT  FacetT;  // dim = d-1
  typedef typename Traits::RidgeT  RidgeT;  // dim = d-2
  typedef typename Traits::VertexT VertexT; // dim = 0
  typedef typename Traits::PointT  PointT;  // dim = 0
  typedef Mesh<Traits>             MeshT;


  // some sugar typedefs
  typedef SeqList<std::vector<CellT>,   SetVector<index_t> > CellContainer;
  typedef SeqList<std::vector<FacetT>,  SetVector<index_t> > FacetContainer;
  typedef SeqList<std::vector<RidgeT>,  SetVector<index_t> > RidgeContainer;
  typedef SeqList<std::vector<VertexT>, SetVector<index_t> > VertexContainer;
  typedef std::vector<PointT>                                PointList;

  struct Icell
  {
    index_t icell;
    uint8_t pos;
  };

  typedef Loki::AssocVector<index_t, SetVector< Icell >  > SingularList; // maps an entity to all his icells

  // Cell
  CellContainer m_cells;

  // Facet
  FacetContainer m_facets;

  // Ridge (for 3D mesh-cells only)
  RidgeContainer m_ridges;
  SingularList m_singular_ridges; // maps ridge_id -> all incident cells

  // Vertices
  VertexContainer m_verts;
  SingularList m_singular_verts; // maps vtx_id -> all incident cells

  // Point (coords)
  PointList m_points;
  

  const int m_spacedim;


  marray::Array<int, 2> const m_table_fC_x_vC;
  marray::Array<int, 2> const m_table_vC_x_fC;
  marray::Array<int, 2> const m_table_fC_x_bC;
  marray::Array<int, 2> const m_table_bC_x_vC;
  marray::Array<int, 2> const m_table_bC_x_fC;

public:

  Timer timer;

  // Cell attributes
  static const int cell_dim         = CellT::dim             ;
  static const int verts_per_cell   = CellT::n_verts         ;
  static const int verts_per_facet  = CellT::n_verts_p_facet ;
  static const int verts_per_ridge  = CellT::n_verts_p_ridge ;
  static const int facets_per_cell  = CellT::n_facets        ;
  static const int ridges_per_cell  = CellT::n_ridges        ;
  static const int ridges_per_facet = CellT::n_ridges_p_facet;

  class CellH;
  class FacetH;
  class RidgeH;
  class VertexH;
  #include "handle_cell.hpp"
  #include "handle_facet.hpp"
  #include "handle_ridge.hpp"
  #include "handle_vertex.hpp"


  static const ECellType CellType = CType;

  typedef std::size_t size_type;

private:
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

public:


  Mesh(unsigned spacedim = CTypeDim(CellType)) : m_spacedim       (spacedim),
                                                 m_table_fC_x_vC  (init_tables<CellType>(0)),
                                                 m_table_vC_x_fC  (init_tables<CellType>(1)),
                                                 m_table_fC_x_bC  (init_tables<CellType>(2)),
                                                 m_table_bC_x_vC  (init_tables<CellType>(3)),
                                                 m_table_bC_x_fC  (init_tables<CellType>(4))
  { ALELIB_CHECK((spacedim-1)<=2, "Invalid space dimension", std::invalid_argument); }

  ~Mesh() {}

  static Mesh* create(unsigned spacedim)
  {
    return new MeshT(spacedim);
  }


  int spaceDim() const
  { return m_spacedim; }

  // BASIC MESH OPERATORS
  // C.f. "A Topological Approach for Handling Triangle Insertion and Removal into Two-
  // Dimensional Unstructured Meshes".
  //

  void reserveCells(index_t n)
  { m_cells.reserve(n); }
  
  void reserveVerts(index_t n)
  { m_verts.reserve(n); }
  
  void reserveFacets(index_t n)
  {
    if (cell_dim > 1)
      m_facets.reserve(n);
  }
  
  void reserveRidges(index_t n)
  {
    if (cell_dim > 2)
      m_ridges.reserve(n);
  }
  
  

  void clear()
  {
    for (index_t i = 0; i < (index_t)m_cells.totalSize(); ++i)
      m_cells.disable(i);
    for (index_t i = 0; i < (index_t)m_verts.totalSize(); ++i)
      m_verts.disable(i);
    if (cell_dim > 1)
      for (index_t i = 0; i < (index_t)m_facets.totalSize(); ++i)
        m_facets.disable(i);
    if (cell_dim > 2)
      for (index_t i = 0; i < (index_t)m_ridges.totalSize(); ++i)
        m_ridges.disable(i);
  }

  /// @return the id of the added vertex
  inline VertexH addVertex()
  { return VertexH(this, pushVertex()); }

  /// @return the id of the added vertex
  inline VertexH addVertex(Point pt, int8_t tag=NO_TAG)
  {
    index_t const id = pushVertex();
    m_verts[id].setTag(tag);
    m_points[id] = pt;
    return VertexH(this, id);
  }

  /// remove a vertex only if it is unreferenced
  /// return true if it was removed
  inline bool removeUnrefVertex(VertexH vtx)
  {
    if (vtx.valency(this) == 0)
    {
      m_verts.disable(vtx.id(this));
      return true;
    }
    return false;
  }

  void removeUnrefVertices()
  {
    VertexH v    = vertexBegin();
    VertexH vend = vertexEnd();
    for (; v!= vend; ++v)
    {
      if (v.isDisabled(this))
        continue;
      
      removeUnrefVertex(v);
    }
  }

  /// @return the id of the added cell 
  CellH addCell(VertexH const verts[])
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

      // get the vertices of the facet
      VertexT const* vt[3];
      for (int k = 0; k < (int)CellT::dim; ++k)
        vt[k] = &m_verts[f_vtcs[k].id(this)];

      if (CellT::dim == 2)
        it = set_1_intersection((*vt[0]).icells.begin(), (*vt[0]).icells.end(),
                                (*vt[1]).icells.begin(), (*vt[1]).icells.end(), &adj_id);
      else if (CellT::dim == 3)
        it = set_1_intersection((*vt[0]).icells.begin(), (*vt[0]).icells.end(),
                                (*vt[1]).icells.begin(), (*vt[1]).icells.end(),
                                (*vt[2]).icells.begin(), (*vt[2]).icells.end(), &adj_id);
      else
        throw; // EDGE NOT IMPLEMENTED

      if (it == &adj_id) // i.ei, no adj cell
      {
        // TODO: can be improved with c++11
        // the new facet will always point to the new cell
        FacetT new_f;
        new_f.icell = new_cid;
        new_f.local_id = i;
        new_f.opp_cell = NULL_IDX;
        new_f.m_tag = NO_TAG ;
        new_f.m_flags = NO_FLAG;
        new_f.valency = 1;
        new_c.facets[i] = pushFacet(new_f);
        //// the new facet will always point to the new cell
        //new_c.facets[i] = pushFacet(FacetT(new_cid, i, NULL_IDX, NO_TAG, NO_FLAG, 1));
      }
      else // if there is an adj cell
      {
        int side;
        std::reverse(f_vtcs, f_vtcs+CellT::n_verts_p_facet);
        bool const is_facet = CellH(adj_id).isFacet(this, f_vtcs, &side, NULL);
        ALELIB_CHECK(is_facet, "maybe something is wrong with the function CellH::isFacet", std::runtime_error);
        ALELIB_CHECK(side >= 0, "maybe there is some inverted cell", std::runtime_error);

        CellT& adj_c = m_cells[adj_id];
        new_c.facets[i] = adj_c.facets[side];
        FacetT& f = m_facets[new_c.facets[i]];
        ++(f.valency);
        ALELIB_ASSERT(!(CellT::dim == 3 && f.valency > 2), "Oops, can not insert singular facets in 3d-cells!", std::invalid_argument);
        // take the chance to set up the adjcent cell
        f.opp_cell = new_cid;

      }
    }

    // setting up ridges
    if (CellT::dim==3)
    {
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
          RidgeT new_r;
          new_r.icell = new_cid;
          new_r.local_id = i;
          new_r.m_tag = NO_TAG;
          new_r.m_flags = NO_FLAG;
          new_r.valency = 1;
          new_c.ridges[i] = pushRidge(new_r);
          // the new facet will always point to the new cell
          //new_c.ridges[i] = pushRidge(RidgeT(new_cid, i, NO_TAG, NO_FLAG, 1));
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


  /// remove a cell
  /// Obs: its vertices are not removed
  void removeCell(CellH ch, bool remove_unref_verts)
  {
    // Some checks
    ALELIB_CHECK(!ch.isDisabled(this) && !ch.isNull(this), "This cell can not be deleted", std::invalid_argument);

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
      else // it happens only in 2D-cells case
      {
        CellH ics[3];
        fh.first3icells(this, ics);
        int I;
        if      (ics[0] == ch) I = 0;
        else if (ics[1] == ch) I = 1;
        else if (ics[2] == ch) I = 2;
        else {
      ALELIB_ASSERT(false, "maybe there is a bug in this function ...", std::runtime_error);
      throw;
      }
        int J=(I+1)%3, K=(I+2)%3;

        --(f.valency);
        f.icell    = ics[J].id(this);
        f.opp_cell = ics[K].id(this);
        f.local_id = ics[J].facetLocalId(this, fh);
      }
    }

    // RIDGES
    if (CellT::dim>2)
    {
      // the ridges
      for (int i = 0; i < (int)CellT::n_ridges; ++i)
      {
        RidgeH rh = ch.ridge(this, i);
        RidgeT &r = this->m_ridges[rh.id(this)];

        // Update facets and remove unref facets

        if (r.valency == 1) // delete it
          m_ridges.disable(rh.id(this));
        else // if (r.valency > 1)
        {
          CellH ics[2];
          rh.first2icells(this, ics);
          int I = 0;
          if (ics[0] == ch)
            I = 1;
          --(r.valency);
          r.icell    = ics[I].id(this);
          r.local_id = ics[I].ridgeLocalId(this, rh);
        }
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





  /// Return the number of activated cells. 
  size_type numCells() const
  { return static_cast<size_type>( m_cells.size() ); }

  /// Return the number cells (activated and deactivated). 
  size_type numCellsTotal() const
  { return static_cast<size_type>( m_cells.totalSize() ); }

  /// Return the number of activated vertices 
  size_type numVertices() const
  { return static_cast<size_type>( m_verts.size() ); }

  /// Return the number of vertices (activated and deactivated) 
  size_type numVerticesTotal() const
  { return static_cast<size_type>( m_verts.totalSize() ); }

  /// Return the number of activated facets 
  size_type numFacets() const
  { return static_cast<size_type>( m_facets.size() ); }

  /// Return the number of facets (activated and deactivated) 
  size_type numFacetsTotal() const
  { return static_cast<size_type>( m_facets.totalSize() ); }

  /// Return the number of activated ridges 
  size_type numRidges() const
  {
    //if (this->cell_dim == 3)
      return static_cast<index_t>( m_ridges.size() );
    //else
    //  return static_cast<index_t>( m_verts.size() );
  }

  /// Return the number of ridges (activated and deactivated) 
  size_type numRidgesTotal() const
  {
    //if (this->cell_dim == 3)
      return static_cast<index_t>( m_ridges.totalSize() );
    //else
    //  return static_cast<index_t>( m_verts.totalSize() );
  }

  // NON_CONST

  VertexH vertexBegin() const
  { return VertexH(this, 0); }

  VertexH vertexEnd() const
  { return VertexH(this, numVerticesTotal()); }

  CellH cellBegin() const
  { return CellH(this, 0); }

  CellH cellEnd() const
  { return CellH(this, numCellsTotal()); }

  FacetH facetBegin() const
  { return FacetH(this, 0); }

  FacetH facetEnd() const
  { return FacetH(this, numFacetsTotal()); }

  RidgeH ridgeBegin() const
  { return RidgeH(this, 0); }

  RidgeH ridgeEnd() const
  { return RidgeH(this, numRidgesTotal()); }

 
  /// @param vs the vertices
  /// @return The facet. If the facet is not found, return an invalid facet. Check with facet.isNull()
  FacetH getFacetFromVertices(VertexH const* vs) const
  {
    std::vector<CellH> star = vs[0].star(this); // the facet is in one of these cells
    
    int ff;
    
    for (unsigned i = 0; i < star.size(); ++i)
    {
      if(star[i].isFacet(this, vs, &ff))
        return star[i].facet(this, abs(ff));
    }
    
    return FacetH(NULL_IDX);
  }

  /// @param vs the vertices
  /// @return The ridge. If the ridge is not found, return an invalid ridge. Check with ridge.isNull()
  RidgeH getRidgeFromVertices(VertexH const* vs) const
  {
    std::vector<CellH> star = vs[0].star(this); // the ridge is in one of these cells
    
    int rr;
    
    for (unsigned i = 0; i < star.size(); ++i)
    {
      if(star[i].isRidge(this, vs, rr))
        return star[i].ridge(this, abs(rr));
    }
    
    return RidgeH(NULL_IDX);
  } 


  // DEBUG purposes
  static void printElementsSize()
  {
    std::cout << "Cell("<<CellH::shortName()<<"): "   << sizeof(CellT) << std::endl;
    std::cout << "Facet: "  << sizeof(FacetT) << std::endl;
    std::cout << "Ridge: "  << sizeof(RidgeT) << std::endl;
    std::cout << "Vertex: " << sizeof(VertexT) << std::endl;
  }

private:

  // return id of the cell
  index_t pushCell()
  { return m_cells.insert(); }

  // return id of the cell
  index_t pushCell(CellT const& a)
  { return m_cells.insert(a); }

  index_t pushFacet()
  { return m_facets.insert(); }

  index_t pushFacet(FacetT const& a)
  { return m_facets.insert(a); }

  index_t pushRidge()
  { return m_ridges.insert(); }

  index_t pushRidge(RidgeT const& a)
  { return m_ridges.insert(a); }

  index_t pushVertex()
  {
    index_t const id = m_verts.insert();
    if (id == (index_t)m_points.size())
      m_points.push_back(PointT());
    return id;
  }

  index_t pushVertex(VertexT const& a, PointT const& b)
  {
    index_t const id = m_verts.insert(a);
    if (id == (index_t)m_points.size())
      m_points.push_back(b);
    return id;
  }



}; // end Mesh class







} // end namespace






#endif



