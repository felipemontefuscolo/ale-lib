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
#include "AssocVector.hpp"

#ifdef ALELIB_DEBUG_ON
# define AT__(x) [(x)]
#else
# define AT__(x) .at(x)
#endif

namespace alelib
{

template<ECellType Ctype>
class Mesh
{
  // Some aliases
  typedef Cell<Ctype> CellT;   // dim = d
  typedef Facet       FacetT;  // dim = d-1
  typedef Ridge       RidgeT;  // dim = d-2
  typedef Vertex      VertexT; // dim = 0
  typedef Point       PointT; // dim = 0
  typedef Mesh<Ctype> MeshT;


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

  static const ECellType CellType = Ctype;

  typedef std::size_t size_type;


  Mesh(unsigned spacedim);

  ~Mesh() {}

  static Mesh* create(unsigned spacedim);


  // BASIC MESH OPERATORS
  // C.f. "A Topological Approach for Handling Triangle Insertion and Removal into Two-
  // Dimensional Unstructured Meshes".
  //

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
  CellH addCell(VertexH const verts[]);

  /// remove a cell
  /// Obs: its vertices are not removed
  void removeCell(CellH c, bool remove_unref_verts);






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
    if (this->cell_dim == 3)
      return static_cast<index_t>( m_ridges.size() );
    else
      return static_cast<index_t>( m_verts.size() );
  }

  /// Return the number of ridges (activated and deactivated) 
  size_type numRidgesTotal() const
  {
    if (this->cell_dim == 3)
      return static_cast<index_t>( m_ridges.totalSize() );
    else
      return static_cast<index_t>( m_verts.totalSize() );
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


  /// @return the id of the added cell 
  CellH addCell_2D(VertexH const verts[]);

  /// remove a cell
  /// Obs: its vertices are not removed
  void removeCell_2D(CellH c, bool remove_unref_verts);


}; // end Mesh class







} // end namespace






#endif



