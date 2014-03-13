#ifndef ALELIB_HANDLER_CELL
#define ALELIB_HANDLER_CELL


namespace alelib
{



//template<ECellType CT>
//class Mesh<CT>::VertexH;

template<ECellType CT>
class Mesh<CT>::CellH
{
  template<ECellType CT_>
  friend class Mesh;

  typedef Mesh<CT> MeshT;  
  typedef typename Mesh<CT>::VertexH VertexH;  
  typedef typename MeshT::CellT CellT;
  
  typedef CellH Self;
  
  index_t m_id;
  

public:

  CellH(index_t id) :  m_id(id) {}

  CellH(MeshT const*, index_t cid) :  m_id(cid) {}

  CellH() :  m_id(NULL_IDX) {}

  inline index_t id(MeshT const*) const
  { return m_id; }

  bool isValid(MeshT const* = 0) const
  { return m_id != (index_t)NULL_IDX; }

  bool isDisabled(MeshT const* mp) const
  { return mp->m_cells[m_id].isDisabled();};

  inline unsigned numVertices(MeshT const*) const
  { return CellT::n_verts; }

  inline unsigned numFacets(MeshT const*) const
  { return CellT::n_facets; }

  inline CellH adjCell(MeshT const* mp, unsigned side) const
  {
    FacetT const& f = mp->m_facets[  mp->m_cells[m_id].facets[side]  ];
    if (f.icell == m_id)
      return CellH(f.opp_cell);
    else
      return CellH(f.icell);
  }

  inline FacetH facet(MeshT const* mp, unsigned side) const
  { return FacetH(mp->m_cells[m_id].facets[side]);}

  inline std::vector<VertexH> vertices(MeshT const* mp) const
  {
    CellT const& c = mp->m_cells[m_id];
    return std::vector<VertexH>(c.verts, c.verts+CellT::n_verts);
  }

  /// same as adjSideAndAnchor, but also returns the adjacent cell
  /// anchor can be NULL
  /// @return the adj cell
  CellH adjCellSideAndAnchor(MeshT const* mp, int side, int* adj_side, int* anchor = NULL) const
  {
    CellH adj = adjCell(mp, side);
    if(!adj.isValid())
      return adj;
    VertexH v[CellT::n_verts_p_facet];
    facetVertices(mp, side, v);
    std::reverse(v, v+CellT::n_verts_p_facet);
    bool const is_facet = adj.isFacet(mp, v, adj_side, anchor);
    ALELIB_CHECK(is_facet, "something is wrong with the adjacencies (2)", std::runtime_error);
    return adj;
  }

  /// @param[out] anchor for 3-cells, return the `anchor' (how the adj cell is rotated)
  /// @return which side of the adjacent cell corresponds to this side. If there is no
  ///  adj cell, return -1
  /// @warning make sure that there is a adjacent cell by the side `side`
  int adjSideAndAnchor(MeshT const* mp, int side, int* anchor = NULL) const
  {
    CellH adj = adjCell(mp, side);
    VertexH v[CellT::n_verts_p_facet];
    facetVertices(mp, side, v);
    std::reverse(v, v+CellT::n_verts_p_facet);
    int ff;
    bool const is_facet = adj.isFacet(mp, v, &ff, anchor);
    ALELIB_CHECK(is_facet, "something is wrong with the adjacencies", std::runtime_error);
    return ff;
  }

  void matesByVtx(MeshT const* mp, int  vtx_loc_id, CellH * mates) const
  {
    for (unsigned fv = 0; fv < CellT::dim; ++fv) // for each facet of this vertex
    {
      int const side = mp->m_table_vC_x_fC( vtx_loc_id , fv );
      mates[fv] = adjCell(mp, side);
    }
    
  }

  static inline const char* shortName()
  {
    switch (MeshT::CellType)
    {
      case EDGE        : return "Edge";
      case TRIANGLE    : return "Tri";
      case QUADRANGLE  : return "Quad";
      case TETRAHEDRON : return "Tet";
      case HEXAHEDRON  : return "Hex";
    };
    return "None";
  };

  static inline index_t facetId(MeshT const* mp, index_t cell, uint8_t pos)
  {
    if (CellT::dim > 1)
      return mp->m_cells[cell].facets[pos];
    else
      return mp->m_cells[cell].verts[pos];
  }

  /// Check if the vertices form a facet of this cell, if so returns facet's id.
  /// @param[in] pointer to the mesh
  /// @param[in] verts vector with the ids of the vertices.
  /// @param[out] ff local id of the facet that has those vertices. For 3d-cells, if the vertices form a facet
  ///  in a anticyclically manner, then the negative of id is returned.
  /// @param[out] anchor in 3D, returns the anchor (how the facet is positioned, see doc.). In 2d, 1 means inverted and 0 otherwise.
  /// @return true if a facet were found, false otherwise.
  inline bool isFacet(MeshT const* mp, VertexH const verts[], int *ff = NULL, int *anchor = NULL) const
  {
#define vpf (CellT::n_verts_p_facet)
    
    //index_t verts[vpf];
    //for (unsigned i = 0; i < vpf; ++i)
    //  verts[i] = verts_[i].id(mp);

    VertexH  facet_verts[vpf]; //n_vertices_per_facet
    VertexH* facet_verts_end = facet_verts + vpf;
    CellH    cell(m_id);
    int  f, a;

    for (f = 0; f < (int)CellT::n_facets; ++f)
    {
      //FacetH const f_global = cell.facet(mp, f);
      //f_global.vertices(mp, facet_verts);
      this->facetVertices(mp, f, facet_verts);
      a = 0;

      for (int i = 0; i < (int)vpf; ++i)
      {
        if (std::equal(facet_verts, facet_verts_end, verts) )
        {
          if (anchor)
            *anchor = a;
          if (ff)
            *ff = f;
          return true;
        }
        else
        {
          std::rotate(facet_verts, facet_verts+1, facet_verts_end);
          ++a;
        }
      }

      if (CellT::dim < 3)
        continue;

      std::reverse(facet_verts, facet_verts_end);
      a = 0;

      for (int i = 0; i < (int)vpf; ++i)
      {
        if (std::equal(facet_verts, facet_verts_end, verts) )
        {
          if (anchor)
            *anchor = a;
          if (ff)
            *ff = -f;
          return true;
        }
        else
        {
          std::rotate(facet_verts, facet_verts+1, facet_verts_end);
          ++a;
        }
      }

    } // end facets

    return false;
#undef vpf
  }  

  std::vector<VertexH> facetVertices(MeshT const* mp, unsigned side) const
  {
    std::vector<VertexH> vts(CellT::n_verts_p_facet);
    facetVertices(mp,side,vts.data());
    return vts;
  }

  void facetVertices(MeshT const* mp, unsigned side, VertexH* vts) const
  {
    ALELIB_CHECK(side < CellT::n_facets, "invalid side", std::invalid_argument);
    for (int i = 0; i < (int)CellT::n_verts_p_facet; ++i)
      *vts++ = VertexH(mp->m_cells[m_id].verts[mp->m_table_fC_x_vC(side,i)]);    
  }

  bool operator != (Self const& x) const
  { return m_id != x.m_id; }

  bool operator == (Self const& x) const
  { return m_id == x.m_id; }
  
  bool operator < (Self const& x) const
  { return m_id < x.m_id; }
  
  Self&
  operator++()
  {
    ++m_id;
    return *this;
  }
  
  Self
  operator++(int)
  { return Self(m_id++); }
  
  Self&
  operator--()
  {
    --m_id;
    return *this;
  }
  
  Self
  operator--(int)
  { return Self(m_id--); }

  friend inline std::ostream& operator << (std::ostream& os, Self const& v) 
  {
    os << v.m_id;
    return os;
  }

//  inline index_t id(MeshT const*) const
//  { return m_id; }
//
//  inline int dim() const
//  { return CellH::dim(m_msh); };
//
//  inline int numVertices() const
//  { return CellH::numVertices(m_msh); }
//
//  inline int numFacets() const
//  { return CellH::numFacets(m_msh); }
//
//  inline int numRidges() const
//  { return CellH::numRidges(m_msh); }
//
//  inline index_t vertexId(int pos) const
//  { return CellH::vertexId(m_msh, m_id, pos); }
//
//  inline void verticesId(index_t begin[]) const
//  { return CellH::verticesId(m_msh, m_id, begin); }
//
//  inline index_t facetId(int pos) const
//  { return CellH::facetId(m_msh, m_id, pos); }
//
//  inline void facetsId(index_t begin[]) const
//  { return CellH::facetsId(m_msh, m_id, begin); }
//
//  inline index_t ridgeId(int pos) const
//  { return CellH::ridgeId(m_msh, m_id, pos); }
//
//  inline void ridgesId(index_t begin[]) const
//  { return CellH::ridgesId(m_msh, m_id, begin); }
//
//  inline index_t adjCell(int pos) const
//  { return CellH::adjCell(m_msh, m_id, pos); }
//
//  inline char adjCellSide(int pos) const
//  { return CellH::adjCellSide(m_msh, m_id, pos); }
//
//  inline char adjCellAnch(int pos) const
//  { return CellH::adjCellAnch(m_msh, m_id, pos); }
//
//  inline bool isRidge(index_t const verts[], int &ridge_pos) const
//  { return CellH::isRidge(m_msh, m_id, verts, ridge_pos); }
//
//  inline bool isFacet(index_t const verts[], int *ff = NULL, int *anchor = NULL) const
//  { return CellH::isFacet(m_msh, m_id, verts, ff, anchor); }
//
//  inline int numBoundaryFacets(int pos[] = NULL) const
//  { return CellH::numBoundaryFacets(m_msh, m_id, pos); }
//
//  inline Labelable& label()
//  { return CellH::label(m_msh, m_id); }
//
//  inline Labelable const& label() const
//  { return CellH::label(m_msh, m_id); }
//
//
//  // -------------------------------------------------------
//  // -------------------------------------------------------
//  // -------------------------------------------------------
//
  
//
//  static inline int dim(MeshT const* mp)
//  { return mp->m_cell_dim; }
//
//  static inline int numVertices(MeshT const* mp)
//  { return mp->m_verts_per_cell; }
//
//  static inline int numFacets(MeshT const* mp)
//  { return mp->m_facets_per_cell; }
//
//  static inline int numRidges(MeshT const* mp)
//  { return mp->m_ridges_per_cell; }
//
//  static inline index_t vertexId(MeshT const* mp, index_t cell, int pos)
//  { return mp->m_cells[cell].verts[pos]; }
//
//  static inline void verticesId(MeshT const* mp, index_t cell, index_t begin[])
//  {
//    for (int i = 0; i < mp->m_verts_per_cell; ++i)
//      *begin++ = mp->m_cells[cell].verts[i];
//  }
//

  
//
//  static inline void facetsId(MeshT const* mp, index_t cell, index_t begin[])
//  {
//    if (mp->m_cell_dim > 1)
//      for (int i = 0; i < mp->m_facets_per_cell; ++i)
//        *begin++ = mp->m_cells[cell].facets[i];
//    else
//      for (int i = 0; i < mp->m_verts_per_cell; ++i)
//        *begin++ = mp->m_cells[cell].verts[i];
//  }
//
//  static inline index_t ridgeId(MeshT const* mp, index_t cell, int pos)
//  { return mp->m_cells[cell].ridges[pos];  }
//
//  static inline void ridgesId(MeshT const* mp, index_t cell, index_t begin[])
//  {
//    for (int i = 0; i < mp->m_ridges_per_cell; ++i)
//      *begin++ = mp->m_cells[cell].ridges[i];
//  }
//
//  //static inline index_t adjCell(MeshT const* mp, index_t cell, int pos)
//  //{
//  //  return mp->m_cells[cell].adj_cells[pos];
//  //}
//
//  //static inline char adjCellSide(MeshT const* mp, index_t cell, int pos)
//  //{
//  //  return mp->m_cells[cell].adj_cells_sides[pos];
//  //}
//
//  //static inline char adjCellAnch(MeshT const* mp, index_t cell, int pos)
//  //{
//  //  if (mp->m_cell_dim==3)
//  //    return mp->m_cells[cell].adj_cells_anchs[pos];
//  //  else
//  //    return -1;
//  //}
//
//  /// Check if the vertices form a ridge of this cell, if so returns ridge's local id.
//  /// @param[in] verts vector with the ids of the vertices.
//  /// @param[out] ridge_pos local id of the ridge that has those vertices. If the vertices form a ridge
//  ///  in a anticyclically manner, then the negative of the ridge's id is returned.
//  /// @return true if a ridge were found, false otherwise.
//  /// @note always return false for 1d and 2d elements
//  ///
//  static inline bool isRidge(MeshT const* mp, index_t cell, index_t const verts[], int &ridge_pos)
//  {
//    index_t ridge_verts[2];
//
//    if (mp->m_cell_dim==3)
//    {
//      for (ridge_pos = 0; ridge_pos < mp->m_ridges_per_cell; ++ridge_pos)
//      {
//        int const crn_global = CellH::ridgeId(mp, cell, ridge_pos);
//        RidgeH::verticesId(mp, crn_global, ridge_verts);
//        if (  (ridge_verts[0]==verts[0]) && (ridge_verts[1]==verts[1])  )
//          return true;
//        if (  (ridge_verts[0]==verts[1]) && (ridge_verts[1]==verts[0])  )
//        {
//          ridge_pos = -ridge_pos;
//          return true;
//        }
//      }
//    }
//    return false;
//
//  }





  
  
//
//  /// @param[out] pos a vector with the local id of the facets
//  static inline int numBoundaryFacets(MeshT const* mp, index_t cell, int pos[] = NULL)
//  {
//    int c=0;
//    for (int i = 0; i < mp->m_facets_per_cell; ++i)
//    {
//      if (CellH::adjCell(mp,cell,i) == NULL_IDX)
//      {
//        if (pos)
//          *pos++ = i;
//        ++c;
//      }
//    }
//    return c;
//  }
//
//  static inline Labelable& label(MeshT* mp, index_t cell)
//  { return mp->m_cells[cell]; }
//
//  static inline Labelable const& label(MeshT const* mp, index_t cell)
//  { return mp->m_cells[cell]; }
//
//
//  // SETTERS  ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//  // SETTERS  ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//  // SETTERS  ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//  // SETTERS  ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//  // SETTERS  ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//  // SETTERS  ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//  // SETTERS  ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//  // SETTERS  ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//  // SETTERS  ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//  // SETTERS  ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//  // SETTERS  ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//private:
//
//  inline void reset()
//  {return CellH::reset(m_msh, m_id); }
//
//  inline void resetFacets()
//  { return CellH::resetFacets(m_msh, m_id);}
//
//  inline void resetRidges()
//  { return CellH::resetRidges(m_msh, m_id);}
//
//  inline void resetAdjCells()
//  { return CellH::resetAdjCells(m_msh, m_id);}
//
//  inline void resetVertices()
//  { return CellH::resetVertices(m_msh, m_id);}
//
//  inline void setIncidence(int fth, index_t adj_cell, char side, char anch=0)
//  { return CellH::setIncidence(m_msh, m_id, fth, adj_cell, side, anch);}
//
//  inline void setAdjCell(int fth, index_t adj_cell)
//  { return CellH::setAdjCell(m_msh, m_id, fth, adj_cell);}
//
//  inline void setAdjCellSide(int fth, int side)
//  { return CellH::setAdjCellSide(m_msh, m_id, fth, side);}
//
//  inline void setAdjCellAnch(int fth, int anch)
//  { return CellH::setAdjCellAnch(m_msh, m_id, fth, anch);}
//
//  inline void setVerticeId(int pos, index_t id)
//  { return CellH::setVerticeId(m_msh, m_id, pos, id);}
//
//  inline void setVerticesId(index_t const* ids, int n_verts = MeshT::m_verts_per_cell)
//  { return CellH::setVerticesId(m_msh, m_id, ids, n_verts);}
//
//  inline void setFacetId(int pos, index_t id)
//  { return CellH::setFacetId(m_msh, m_id, pos, id);}
//
//  inline void setFacetsId(index_t const* ids, int n_facets = MeshT::m_facets_per_cell)
//  { return CellH::setFacetsId(m_msh, m_id, ids, n_facets);}
//
//  inline void setRidgeId(int pos, index_t id)
//  { return CellH::setRidgeId(m_msh, m_id, pos, id);}
//
//  inline void setRidgesId(index_t const* ids, int n_ridges = MeshT::m_ridges_per_cell)
//  { return CellH::setRidgesId(m_msh, m_id, ids, n_ridges);}
//
//  void setAllMembers(index_t const* vertices, index_t const* ridges, index_t const* facets, index_t const* adj_cells, int const* sides,
//                                   int const* anchs, int const* tag, int const* flag)
//  { return CellH::setAllMembers(m_msh, m_id, vertices, ridges, facets, adj_cells, sides,
//                                   anchs, tag, flag);}
//
//
//
//  // -------------------------------------------------------
//  // -------------------------------------------------------
//  // -------------------------------------------------------
//
//
//  inline void reset(MeshT* mp, int cell)
//  { mp->m_cells[cell].reset(); }
//
//  inline void resetFacets(MeshT* mp, int cell)
//  { mp->m_cells[cell].resetFacets(); }
//
//  inline void resetRidges(MeshT* mp, index_t cell)
//  { mp->m_cells[cell].resetRidges(); }
//
//
//
//
//  inline void resetVertices(MeshT* mp, index_t cell)
//  { mp->m_cells[cell].resetVertices(); }
//
//  /// is same to:
//  /// setAdjCell(facet, adjCell);
//  /// setIncidCellPos(facet, pos);
//  /// setIncidCellAnch(facet, anch);
//  ///
//  inline void setIncidence(MeshT* mp, index_t cell, int fth, index_t adj_cell, char side, char anch=0)
//  {
//    setAdjCell(mp, cell, fth, adj_cell);
//    setAdjCellSide(mp, cell, fth, side);
//    setAdjCellAnch(mp, cell, fth, anch);
//  }
//
//  // inline void setAdjCell(MeshT* mp, index_t cell, int fth, index_t adj_cell)
//  // { mp->m_cells[cell].adj_cells[fth] = adj_cell; }
//  // 
//  // inline void setAdjCellSide(MeshT* mp, index_t cell, int fth, int side)
//  // { mp->m_cells[cell].adj_cells_sides[fth] = side; }
//  // 
//  // inline void setAdjCellAnch(MeshT* mp, index_t cell, int fth, int anch)
//  // { mp->m_cells[cell].adj_cells_anchs[fth] = anch; }
//
//  inline void setVerticeId(MeshT* mp, index_t cell, int pos, index_t id)
//  { mp->m_cells[cell].verts[pos] = id; }
//
//  inline void setVerticesId(MeshT* mp, index_t cell, index_t const* ids, int n_verts = MeshT::m_verts_per_cell)
//  {
//    for (int i = 0; i < n_verts; ++i)
//      mp->m_cells[cell].verts[i] = ids[i];
//  }
//
//  inline void setFacetId(MeshT* mp, index_t cell, int pos, index_t id)
//  { mp->m_cells[cell].facets[pos] = id; }
//
//  inline void setFacetsId(MeshT* mp, index_t cell, index_t const* ids, int n_facets = MeshT::m_facets_per_cell)
//  {
//    for (int i = 0; i < n_facets; ++i)
//      mp->m_cells[cell].facets[i] = ids[i];
//  }
//
//  inline void setRidgeId(MeshT* mp, index_t cell, int pos, index_t id)
//  { mp->m_cells[cell].ridges[pos] = id; }
//
//  inline void setRidgesId(MeshT* mp, index_t cell, index_t const* ids, int n_ridges = MeshT::m_ridges_per_cell)
//  {
//    for (int i = 0; i < n_ridges; ++i)
//      mp->m_cells[cell].ridges[i] = ids[i];
//  }
//
//  void setAllMembers(MeshT* mp, index_t cell, index_t const* vertices, index_t const* ridges, index_t const* facets, index_t const* adj_cell, int const* side,
//                                   int const* anch, int const* tag, int const* flag)
//  {
//    // nodes
//    if (vertices != NULL)
//      setVerticesId(mp, cell, vertices);//, mp->m_verts_per_cell);
//
//    if (ridges!= NULL)
//      setRidgesId(mp, cell, ridges);//, mp->m_ridges_per_cell);
//
//    if (facets!= NULL)
//      setFacetsId(mp, cell, facets);//, mp->m_facets_per_cell);
//
//    for (int i = 0; i < mp->m_facets_per_cell; ++i)
//    {
//      if (adj_cell!=NULL)
//        setAdjCell(mp, cell, i, adj_cell[i]);
//        
//      if (side!=NULL)
//        setAdjCellSide(mp, cell, i, side[i]);  
//        
//      if (anch!=NULL)
//        setAdjCellAnch(mp, cell, i, anch[i]);
//      
//    }
//
//    if (tag != NULL)
//      mp->m_cells[cell].setTag(*tag);
//    if (flag != NULL)
//      mp->m_cells[cell].setFlag(*flag);
//  }
//
//




};



} // end namespace


#endif


