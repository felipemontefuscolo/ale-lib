#ifndef ALELIB_HANDLER_VERTEX
#define ALELIB_HANDLER_VERTEX



namespace alelib
{



template<ECellType CT>
class Mesh<CT>::VertexH
{
  template<ECellType CT_>
  friend class Mesh;

  typedef Mesh<CT> MeshT;
  
  typedef Mesh<CT>::VertexH Self;
  
  typedef typename Mesh<CT>::CellH CellH;  

  index_t m_id;

public:

  VertexH(index_t id) :  m_id(id) {}

  VertexH(MeshT const*, index_t vid) : m_id(vid) {}

  VertexH() : m_id(NULL_IDX) {}

  bool isValid() const
  { return m_id != (index_t)NULL_IDX; }

  inline index_t id(MeshT const*) const
  { return m_id; }

  bool isDisabled(MeshT const* mp) const
  { return mp->m_verts[m_id].isDisabled();};
//
//  inline index_t incidCell() const
//  { return VertexH::incidCell(m_msh, m_id); };
//
//  inline int localId() const
//  { return VertexH::localId(m_msh, m_id); };

  inline Labelable& label(MeshT* mp)
  { return VertexH::label(mp, m_id); }

  inline Labelable const& label(MeshT const* mp) const
  { return VertexH::label(mp, m_id); }

  inline void coord(MeshT const* mp, Real *coord, int spacedim) const
  { return VertexH::coord(mp, coord, spacedim); }

  inline Real coord(MeshT const* mp, int i) const
  { return VertexH::coord(mp, m_id, i); }

  inline void setCoord(MeshT* mp, Real const* coord)
  { return VertexH::setCoord(mp, m_id, coord); }

  // number os cells that contain this vertex
  inline unsigned valency(MeshT const* mp) const
  { return mp->m_verts[m_id].valency(); }

  inline std::vector<CellH> star(MeshT const* mp) const
  {
    std::vector<CellH> x(mp->m_verts[m_id].icells.begin(), mp->m_verts[m_id].icells.end());
    return x;
  }

//  // returns the number of (local) disjoint sets that contains this vertex.
//  // if it is not singular, returns 0 or -1.
//  inline int singularity()
//  { return VertexH::singularity(m_msh, m_id); }
//
//  inline bool isSingular()
//  { return VertexH::isSingular(m_msh, m_id); }
//
//  // -------------------------------------------------
//  // -------------------------------------------------
//  //   Static version
//
//  static inline index_t incidCell(MeshT const* mp, index_t vtx)
//  { return mp->m_verts[vtx].incidCell(); };
//
//  static inline index_t localId(MeshT const* mp, index_t vtx)
//  { return mp->m_verts[vtx].localId(); };

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

private:


  static inline Labelable& label(MeshT* mp, index_t vtx)
  { return mp->m_verts[vtx]; } // conversion

  static inline Labelable const& label(MeshT const* mp, index_t vtx)
  { return mp->m_verts[vtx]; } // conversion

  static inline bool isDisabled(MeshT const* mp, index_t vtx)
  { return mp->m_verts[vtx].isDisabled(); }

  static inline void coord(MeshT const* mp, index_t vtx, Real *coord, int spacedim)
  { return mp->m_points[vtx].coord(coord, spacedim); }

  static inline Real coord(MeshT const* mp, index_t vtx, int i)
  { return mp->m_points[vtx].coord(i);  }

  static inline void setCoord(MeshT* mp, index_t vtx, Real const* coord)
  { return mp->m_points[vtx].setCoord(coord, mp->m_spacedim); }

  // number os cells that contain a vertex
  static inline int valency(MeshT const* mp, index_t vtx)
  { return mp->m_verts[vtx].valency(); }
//
//  // returns the number of (local) disjoint sets that contains a singular vertex
//  static inline int singularity(MeshT const* mp, index_t vtx)
//  { return mp->m_verts[vtx].singularity(); }
//
//  static inline bool isSingular(MeshT const* mp, index_t vtx)
//  { return singularity(mp,vtx) > 0; }
//
//
//  // SETTERS  PRIVATE  PRIVATE  PRIVATE  PRIVATE  PRIVATE  PRIVATE  PRIVATE  PRIVATE  PRIVATE
//  // SETTERS  PRIVATE  PRIVATE  PRIVATE  PRIVATE  PRIVATE  PRIVATE  PRIVATE  PRIVATE  PRIVATE
//
//
//
//  inline void setIncidCell(index_t icell_id)
//  { return VertexH::setIncidCell(m_msh, m_id, icell_id); }
//
//  // vertex lid of incident cell
//  inline void setLocalId(int pos)
//  { return VertexH::setLocalId(m_msh, m_id, pos); }
//
//  /// is the same as doing setIncidCell(icell_id); setLocalId(pos);
//  inline void setIncidence(index_t icell_id, int pos)
//  { return VertexH::setIncidence(m_msh, m_id, icell_id, pos); }
//
//  inline int incValency()
//  { return VertexH::incValency(m_msh, m_id); }
//
//  inline int decValency()
//  { return VertexH::decValency(m_msh, m_id); }
//
//  inline int incSingularity()
//  { return VertexH::incSingularity(m_msh, m_id); }
//
//  inline int decSingularity()
//  { return VertexH::decSingularity(m_msh, m_id); }
//
//  inline void setAllMembers(Real const coord[], int spacedim, index_t const* ic, int8_t const* pos, uint8_t const* tag,
//                            Flags const* flags, Vertex::Masks const* stat, uint16_t const* ref_counter, uint16_t const* singular_counter)
//  {
//    return VertexH::setAllMembers(m_msh, m_id, coord, spacedim, ic, pos,tag, flags, stat, ref_counter, singular_counter);
//  }
//
//  // -------------------------------------------------
//  // -------------------------------------------------
//  //   Static version
//
//  static inline void setIncidCell(MeshT* mp, index_t vertex, index_t icell_id)
//  { mp->m_verts[vertex].setIncidCell(icell_id); }
//
//  // vertex lid of incident cell
//  static inline void setLocalId(MeshT* mp, index_t vertex, int pos)
//  { mp->m_verts[vertex].setLocalId(pos); }
//
//  /// is the same as doing setIncidCell(icell_id); setLocalId(pos);
//  static inline void setIncidence(MeshT* mp, index_t vertex, index_t icell_id, int pos)
//  { mp->m_verts[vertex].setIncidence(icell_id, pos); }
//
//  static inline int incValency(MeshT* mp, index_t vertex)
//  { return mp->m_verts[vertex].incValency(); }
//
//  static inline int decValency(MeshT* mp, index_t vertex)
//  { return mp->m_verts[vertex].decValency(); }
//
//  static inline int incSingularity(MeshT* mp, index_t vertex)
//  { return mp->m_verts[vertex].incSingularity(); }
//
//  static inline int decSingularity(MeshT* mp, index_t vertex)
//  { return mp->m_verts[vertex].decSingularity(); }
//
//  //static inline void setAllMembers(MeshT* mp, index_t vertex, Real const coord[], int spacedim, index_t const* ic, int8_t const* pos, uint8_t const* tag,
//  //                                                     Flags const* flags, Vertex::Masks const* stat, uint16_t const* ref_counter, uint16_t const* singular_counter)
//  //{
//  //  Vertex & f = mp->m_verts[vertex];
//  //  f.setAllMembers(ic, pos, tag, flags, stat, ref_counter, singular_counter);
//  //  mp->m_points[vertex].setCoord(coord, spacedim);
//  //}

};



} // end namespace

#endif


