#ifndef ALELIB_HANDLER_FACET
#define ALELIB_HANDLER_FACET



namespace alelib
{

template<ECellType CT>
class Mesh<CT>::FacetH
{
  template<ECellType CT_>
  friend class Mesh;

  typedef Mesh<CT> MeshT;
  
  typedef FacetH Self;
  
  index_t m_id;

public:

  FacetH(index_t id) :  m_id(id) {}

  FacetH(MeshT const*, index_t fid) : m_id(fid) {}

  FacetH() :  m_id(NULL_IDX) {}

  inline index_t id(MeshT const*) const
  { return m_id; }

  bool isValid() const
  { return m_id != (index_t)NULL_IDX; }

  bool isDisabled(MeshT const* mp) const
  { return mp->m_facets[m_id].isDisabled();};

  unsigned valency(MeshT const* mp) const
  { return mp->m_facets[m_id].valency; }

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

  inline void vertices(MeshT const* mp, VertexH * facet_verts) const
  {
    FacetT const& fct = mp->m_facets[m_id];
    index_t const cell = fct.icell;
    int const f_pos = fct.local_id;

    for (int i = 0; i < (int)CellT::n_verts_p_facet; ++i)
      *facet_verts++ = VertexH(mp->m_cells[cell].verts[mp->m_table_fC_x_vC(f_pos,i)]);
  }

  static inline void verticesId(MeshT const* mp, index_t facet, index_t * facet_verts)
  {
    FacetT const& fct = mp->m_facets[facet];
    index_t const cell = fct.icell;
    int const f_pos = fct.local_id;

    for (int i = 0; i < (int)CellT::n_verts_p_facet; ++i)
      *facet_verts++ = mp->m_cells[cell].verts[mp->m_table_fC_x_vC(f_pos,i)];
  }
//
//  static inline void ridgesId(MeshT const* mp, index_t facet, index_t * ridges)
//  {
//    Facet const& fct = mp->m_facets[facet];
//    index_t const cell = fct.incidCell();
//    int const f_pos = fct.localId();
//
//    if (mp->m_cell_dim == 3)
//      // note: n_ridges_per_facet = n_vertices_per_facet
//      for (int i = 0; i < mp->m_verts_per_facet; ++i)
//        *ridges++ = mp->m_cells[cell].ridges[ mp->m_table_fC_x_bC(f_pos,i) ];
//    else
//      FacetH::verticesId(mp, facet, ridges);
//  }
//
//  static inline Labelable& label(MeshT* mp, index_t facet)
//  { return mp->m_facets[facet]; } // conversion
//
//  static inline Labelable const& label(MeshT const* mp, index_t facet)
//  { return mp->m_facets[facet]; } // conversion
//
//
//  // SETTERS  ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

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





} // end namespace

#endif


