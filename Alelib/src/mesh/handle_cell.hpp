class CellH
{
  template<ECellType CT_>
  friend class Mesh;

  typedef Mesh<Ctype> MeshT;  
  typedef typename Mesh<Ctype>::VertexH VertexH;  
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

  void setTag(MeshT* mp, int tag)
  { mp->m_cells[m_id].setTag(tag); }

  int tag(MeshT const* mp) const
  { return mp->m_cells[m_id].getTag(); }

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

  inline RidgeH ridge(MeshT const* mp, unsigned i) const
  { return RidgeH(mp->m_cells[m_id].ridges[i]);}

  inline VertexH vertex(MeshT const* mp, unsigned i) const
  { return VertexH(mp->m_cells[m_id].verts[i]);}


  inline std::vector<VertexH> vertices(MeshT const* mp) const
  {
    CellT const& c = mp->m_cells[m_id];
    return std::vector<VertexH>(c.verts, c.verts+CellT::n_verts);
  }

  inline void vertices(MeshT const* mp, VertexH* verts) const
  {
    CellT const& c = mp->m_cells[m_id];
    for (int i = 0; i < (int)CellT::n_verts; ++i)
      verts[i] = VertexH(c.verts[i]);
  }

  inline void verticesCoord(MeshT const* mp, Real* coords) const
  {
    CellT const& c = mp->m_cells[m_id];
    int const sd = mp->spaceDim();
    for (int i = 0; i < (int)CellT::n_verts; ++i)
      mp->m_points[c.verts[i]].coord(coords+i*sd, sd);
  }

  inline void facets(MeshT const* mp, FacetH* facets) const
  {
    CellT const& c = mp->m_cells[m_id];
    for (int i = 0; i < (int)CellT::n_facets; ++i)
      facets[i] = FacetH(c.facets[i]);
  }

  inline void ridges(MeshT const* mp, RidgeH* ridges) const
  {
    CellT const& c = mp->m_cells[m_id];
    for (int i = 0; i < (int)CellT::n_ridges; ++i)
      ridges[i] = RidgeH(c.ridges[i]);
  }

  inline std::vector<VertexH> ridges(MeshT const* mp) const
  {
    CellT const& c = mp->m_cells[m_id];
    return std::vector<VertexH>(c.ridges, c.ridges+CellT::n_ridges);
  }

  inline void verticesContigId(MeshT const* mp, index_t* ids) const
  {
    CellT const& c = mp->m_cells[m_id];
    for (int i = 0; i < (int)CellT::n_verts; ++i)
      ids[i] = mp->m_verts.contiguousId(c.verts[i]);
  }

  inline void facetsContigId(MeshT const* mp, index_t* ids) const
  {
    CellT const& c = mp->m_cells[m_id];
    for (int i = 0; i < (int)CellT::n_facets; ++i)
      ids[i] = mp->m_facets.contiguousId(c.facets[i]);
  }

  inline void ridgesContigId(MeshT const* mp, index_t* ids) const
  {
    CellT const& c = mp->m_cells[m_id];
    for (int i = 0; i < (int)CellT::n_ridges; ++i)
      ids[i] = mp->m_ridges.contiguousId(c.ridges[i]);
  }

  inline index_t contiguousId(MeshT const* mp) const
  { return mp->m_cells.contiguousId(m_id); }




  inline void points(MeshT const* mp, Point * pts) const
  {
    VertexH vs[CellT::n_verts];
    vertices(mp, vs);
    for (int i = 0; i < (int)CellT::n_verts; ++i)
      pts[i] = mp->m_points[vs[i].id(mp)];
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
    VertexH vts[CellT::n_verts_p_facet];
    facetVertices(mp, side, vts);
    std::reverse(vts, vts+CellT::n_verts_p_facet);
    int ff;
    bool const is_facet = adj.isFacet(mp, vts, &ff, anchor);
    //if (!is_facet) // debug
    //{
    //  std::cout << "AQUII " <<"me:"<<m_id <<" adj:"<<adj<<", "<<vts[0] <<" "<<vts[1]<<"; side:"<<side<< std::endl;
    //}
    ALELIB_CHECK(is_facet, "No adjacent cell", std::runtime_error);
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


  /// Check if the vertices form a facet of this cell, if so returns facet's id.
  /// @param[in] pointer to the mesh
  /// @param[in] verts vector with the ids of the vertices.
  /// @param[out] ff local id of the facet that has those vertices. For 3d-cells, if the vertices form a facet
  ///  in a anticyclically manner, then the negative of id is returned.
  /// @param[out] anchor in 3D, returns the anchor (how the facet is positioned, see doc.). In 2d, 1 means inverted and 0 not inverted.
  /// @return true if a facet were found, false otherwise.
  inline bool isFacet(MeshT const* mp, VertexH const verts[], int *ff = NULL, int *anchor = NULL) const
  {
#define vpf (CellT::n_verts_p_facet)
    
    //index_t verts[vpf];
    //for (unsigned i = 0; i < vpf; ++i)
    //  verts[i] = verts_[i].id(mp);

    VertexH  facet_verts[vpf]; //n_vertices_per_facet
    VertexH* facet_verts_end = facet_verts + vpf;
    std::reverse_iterator<VertexH*> from(facet_verts_end);
    std::reverse_iterator<VertexH*> to(facet_verts);
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
          //std::rotate(facet_verts, facet_verts+1, facet_verts_end);
          std::rotate(from, from+1, to);
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
          //std::rotate(facet_verts, facet_verts+1, facet_verts_end);
          std::rotate(from, from+1, to);
          ++a;
        }
      }

    } // end facets

    return false;
#undef vpf
  }  

  // TODO: ridge_pos must be optional
  inline bool isRidge(MeshT const* mp, VertexH const verts[], int &ridge_pos) const
  {
    VertexH ridge_verts[2];

    if (MeshT::cell_dim==3)
    {
      for (ridge_pos = 0; ridge_pos < MeshT::ridges_per_cell; ++ridge_pos)
      {
        for (int i = 0; i < MeshT::verts_per_ridge; ++i)
          ridge_verts[i] = mp->m_cells[m_id].verts[mp->m_table_bC_x_vC(ridge_pos,i)];
        
        if (  (ridge_verts[0]==verts[0]) && (ridge_verts[1]==verts[1])  )
          return true;
        if (  (ridge_verts[0]==verts[1]) && (ridge_verts[1]==verts[0])  )
        {
          ridge_pos = -ridge_pos;
          return true;
        }
      }
    }
    return false;

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

  void ridgeVertices(MeshT const* mp, int ridge_pos, VertexH* vts) const
  {
    // the compiler complais here when CellT::n_ridges=0
    ALELIB_CHECK((unsigned)ridge_pos < (CellT::n_ridges>0u?CellT::n_ridges:1u), "invalid ridge_pos", std::invalid_argument);
    for (int i = 0; i < (int)CellT::n_verts_p_ridge; ++i)
      *vts++ = VertexH(mp->m_cells[m_id].verts[mp->m_table_bC_x_vC(ridge_pos,i)]);    
  }

  /// return the local id of the facet f
  int facetLocalId(MeshT const* mp, FacetH f) const
  {
    CellT const& c = mp->m_cells[m_id];
    for (int i = 0; i < (int)CellT::n_facets; ++i)
    {
      if (f.id(mp) == c.facets[i])
        return i;
    }
    return NULL_IDX;
  }

  /// return the local id of the facet f
  int ridgeLocalId(MeshT const* mp, RidgeH r) const
  {
    CellT const& c = mp->m_cells[m_id];
    for (int i = 0; i < (int)CellT::n_ridges; ++i)
    {
      if (r.id(mp) == c.ridges[i])
        return i;
    }
    return NULL_IDX;
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




};


