class FacetH
{
  template<typename Traits_>
  friend class Mesh;

  typedef Mesh<Traits> MeshT; 
  
  typedef FacetH Self;
  
  index_t m_id;

public:

  FacetH(index_t id) :  m_id(id) {}

  FacetH(MeshT const*, index_t fid) : m_id(fid) {}

  FacetH() :  m_id(NULL_IDX) {}

  inline index_t id(MeshT const*) const
  { return m_id; }

  void setTag(MeshT* mp, int tag)
  { mp->m_facets[m_id].setTag(tag); }

  int tag(MeshT const* mp) const
  { return mp->m_facets[m_id].getTag(); }

  /// For debug purposes, it contains isNull and isDisabled
  /// This check if this entity is usable.
  bool isValid(MeshT const* mp) const
  {
    bool b = ! (isNull(mp) || isDisabled(mp));
    
    if ((size_type)m_id < mp->numFacetsTotal())
      return b;
    else
      return false;
  }

  bool isNull(MeshT const* = 0) const
  { return m_id == (index_t)NULL_IDX; }

  bool isDisabled(MeshT const* mp) const
  { return mp->m_facets[m_id].isDisabled();};

  unsigned valency(MeshT const* mp) const
  { return mp->m_facets[m_id].valency; }

  std::pair<CellH,CellH> icellAndMate(MeshT const* mp) const
  { return std::make_pair(CellH(mp->m_facets[m_id].icell), CellH(mp->m_facets[m_id].opp_cell)); }

  CellH icellSide0(MeshT const* mp) const
  { return CellH(mp->m_facets[m_id].icell); }

  CellH icellSide1(MeshT const* mp) const
  { return CellH(mp->m_facets[m_id].opp_cell); }

  // in this version the cell is the icell
  inline int localId(MeshT const* mp) const
  {
    return mp->m_facets[m_id].local_id;
  }

  /// return the third cell connects to this facet (singular facet).
  /// If a third cell does not exist, then return NULL_IDX
  void first3icells(MeshT const* mp, CellH ics[]) const
  {
    ALELIB_CHECK(CellT::dim == 2, "FacetH::first3icells only can be called by 2-cell meshes", std::invalid_argument);
    
    VertexH vts[CellT::n_verts_p_facet];
    this->vertices(mp, vts);
    typedef typename VertexT::VectorT VectorT;
    typedef typename VectorT::const_iterator iterator;
    
    VectorT const& star0 = mp->m_verts[vts[0].id(mp)].icells;
    VectorT const& star1 = mp->m_verts[vts[1].id(mp)].icells;
    
     // put index_t conversion here because g++ complains
    index_t intersect[3] = {index_t(NULL_IDX), index_t(NULL_IDX), index_t(NULL_IDX)};

    set_3_intersection(star0.begin(), star0.end(),
                       star1.begin(), star1.end(), intersect);
    ics[0] = CellH(intersect[0]);
    ics[1] = CellH(intersect[1]);
    ics[2] = CellH(intersect[2]);
    
  }

  inline void vertices(MeshT const* mp, VertexH * facet_verts) const
  {
    FacetT const& fct = mp->m_facets[m_id];
    index_t const cell = fct.icell;
    int const f_pos = fct.local_id;

    for (int i = 0; i < (int)CellT::n_verts_p_facet; ++i)
      *facet_verts++ = VertexH(mp->m_cells[cell].verts[mp->m_table_fC_x_vC(f_pos,i)]);
  }

  inline void points(MeshT const* mp, Point * facet_points) const
  {
    VertexH vs[MeshT::verts_per_facet];
    vertices(mp, vs);
    for (int i = 0; i < MeshT::verts_per_facet; ++i)
      facet_points[i] = mp->m_points[vs[i].id(mp)];
  }

  void ridges(MeshT const* mp, RidgeH * ridges)
  {
    FacetT const& fct = mp->m_facets[m_id];
    //index_t const cell = fct.incidCell();
    CellT const& cell = mp->m_cells[fct.icell];
    int const f_pos = fct.local_id;

    if (MeshT::cell_dim == 3)
      // note: n_ridges_per_facet = n_vertices_per_facet
      for (int i = 0; i < MeshT::verts_per_facet; ++i)
        *ridges++ = cell.ridges[ mp->m_table_fC_x_bC(f_pos,i) ];
  }


  static inline void verticesId(MeshT const* mp, index_t facet, index_t * facet_verts)
  {
    FacetT const& fct = mp->m_facets[facet];
    index_t const cell = fct.icell;
    int const f_pos = fct.local_id;

    for (int i = 0; i < (int)CellT::n_verts_p_facet; ++i)
      *facet_verts++ = mp->m_cells[cell].verts[mp->m_table_fC_x_vC(f_pos,i)];
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
//  inline index_t incidCell() const
//  { return FacetH::incidCell(m_msh, m_id); };
//
//  inline int localId() const
//  { return FacetH::localId(m_msh, m_id); };
//
//  inline int numVertices() const
//  { return FacetH::numVertices(m_msh); }
//
//  inline int numRidges() const
//  { return FacetH::numRidges(m_msh); }
//
//  inline void verticesId(index_t * facet_verts) const
//  { return FacetH::verticesId(m_msh, m_id, facet_verts); }
//
//  inline void ridgesId(index_t * ridges) const
//  { return FacetH::ridgesId(m_msh, m_id, ridges); }
//
//  inline Labelable& label()
//  { return FacetH::label(m_msh, m_id); }
//
//  inline Labelable const& label() const
//  { return FacetH::label(m_msh, m_id); }
//
//  // ----------------------------------------------------------
//  // ----------------------------------------------------------
//  // ----------------------------------------------------------
//
//
//  static inline index_t incidCell(MeshT const* mp, index_t facet)
//  { return mp->m_facets[facet].incidCell(); };
//
//  static inline int localId(MeshT const* mp, index_t facet)
//  { return mp->m_facets[facet].localId(); };
//
//  static inline int numVertices(MeshT const* mp)
//  { return mp->m_verts_per_facet; }
//
//  static inline int numRidges(MeshT const* mp)
//  { return mp->m_ridges_per_facet; }
//

//

//
//  static inline Labelable& label(MeshT* mp, index_t facet)
//  { return mp->m_facets[facet]; } // conversion
//
//  static inline Labelable const& label(MeshT const* mp, index_t facet)
//  { return mp->m_facets[facet]; } // conversion
//
//
//  // SETTERS  ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::



//private:
//  inline void setIncidCell(MeshT* mp, int facet, index_t icell_id)
//  { mp->m_facets[facet].setIncidCell(icell_id); }
//
//  // facet lid of incident cell
//  inline void setLocalId(MeshT* mp, int facet, int pos)
//  { mp->m_facets[facet].setLocalId(pos); }
//
//  /// is the same as doing setIncidCell(icell_id); setLocalId(pos);
//  inline void setIncidence(MeshT* mp, int facet, index_t icell_id, int pos)
//  { mp->m_facets[facet].setIncidence(icell_id, pos); }
//
//  inline void setAllMembers(MeshT* mp, int facet, index_t const* ic, uint8_t const* pos, uint8_t const* tag, Flags const* flags)
//  {
//    Facet & f = mp->m_facets[facet];
//    f.setAllMembers(ic, pos, tag, flags);
//  }
//
//  // :::::::::::::::::::::::::::::::::::::::::  Facet



};
