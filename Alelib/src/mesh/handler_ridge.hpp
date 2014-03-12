#ifndef ALELIB_HANDLER_RIDGE
#define ALELIB_HANDLER_RIDGE



namespace alelib
{

template<ECellType CT>
class Mesh<CT>::RidgeH
{
  template<ECellType CT_>
  friend class Mesh;

  typedef Mesh<CT> MeshT;

  typedef RidgeH Self;

  index_t m_id;

public:

  RidgeH(index_t id) :  m_id(id) {}

  RidgeH(MeshT const*, index_t rid) :  m_id(rid) {}

  RidgeH() :  m_id(NULL_IDX) {}

  inline index_t id(MeshT const*) const
  { return m_id; }

  bool isValid() const
  { return m_id != (index_t)NULL_IDX; }

  bool isDisabled(MeshT const* mp) const
  { return mp->m_ridges[m_id].isDisabled();};

//  inline index_t id(MeshT const*) const
//  { return m_id; }
//
//  inline index_t incidCell() const
//  { return RidgeH::incidCell(m_msh, m_id); };
//
//  inline int localId() const
//  { return RidgeH::localId(m_msh, m_id); };
//
//  inline void verticesId(index_t *ridge_verts) const
//  { return RidgeH::verticesId(m_msh, m_id, ridge_verts); }
//
//  inline Labelable& label()
//  { return RidgeH::label(m_msh, m_id); }
//
//  inline Labelable const& label() const
//  { return RidgeH::label(m_msh, m_id); }
//
//  // number os cells that contain this ridge
//  inline int valency()
//  { return RidgeH::valency(m_msh, m_id); }
//
//  // returns the number of (local) disjoint sets that contains this ridge.
//  // if it is not singular, returns 0 or -1.
//  inline int singularity()
//  { return RidgeH::singularity(m_msh, m_id); }
//
//  inline bool isSingular()
//  { return RidgeH::isSingular(m_msh, m_id); }
//
//  // -------------------------------------------------
//  // -------------------------------------------------
//  //   Static version
//
//  static inline int incidCell(MeshT const* mp, index_t ridge)
//  { return mp->m_idges[ridge].incidCell(); };
//
//  static inline int localId(MeshT const* mp, index_t ridge)
//  { return mp->m_idges[ridge].localId(); };
//
//  static inline void verticesId(MeshT const* mp, index_t ridge, index_t *ridge_verts)
//  {
//    Ridge const& rge = mp->m_idges[ridge];
//    index_t const cell = rge.incidCell();
//    int const ridge_pos = rge.localId();
//
//    if (mp->m_cell_dim == 3)
//      for (int i = 0; i < mp->m_verts_per_ridge; ++i)
//        *ridge_verts++ = mp->m_cells[cell].verts[mp->m_table_bC_x_vC(ridge_pos,i)];
//  }
//
//  static inline Labelable& label(MeshT* mp, index_t ridge)
//  { return mp->m_idges[ridge]; } // conversion
//
//  static inline Labelable const& label(MeshT const* mp, index_t ridge)
//  { return mp->m_idges[ridge]; } // conversion
//
//  // number os cells that contain a ridge
//  static inline int valency(MeshT const* mp, index_t ridge)
//  { return mp->m_idges[ridge].valency(); }
//
//  // returns the number of (local) disjoint sets that contains a singular ridge
//  static inline int singularity(MeshT const* mp, index_t ridge)
//  { return mp->m_idges[ridge].singularity(); }
//
//  static inline bool isSingular(MeshT const* mp, index_t ridge)
//  { return RidgeH::singularity(mp,ridge) > 0; }
//
//  // SETTERS  PRIVATE  PRIVATE  PRIVATE  PRIVATE  PRIVATE  PRIVATE  PRIVATE  PRIVATE  PRIVATE
//  // SETTERS  PRIVATE  PRIVATE  PRIVATE  PRIVATE  PRIVATE  PRIVATE  PRIVATE  PRIVATE  PRIVATE

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

//
//private:
//  inline void setIncidCell(MeshT* mp, index_t ridge, index_t icell_id)
//  { mp->m_idges[ridge].setIncidCell(icell_id); }
//
//  // ridge lid of incident cell
//  inline void setLocalId(MeshT* mp, index_t ridge, int pos)
//  { mp->m_idges[ridge].setLocalId(pos); }
//
//  /// is the same as doing setIncidCell(icell_id); setLocalId(pos);
//  inline void setIncidence(MeshT* mp, index_t ridge, index_t icell_id, int pos)
//  { mp->m_idges[ridge].setIncidence(icell_id, pos); }
//
//  inline int valency(MeshT* mp, index_t ridge) const
//  { return mp->m_idges[ridge].valency(); }
//
//  inline int incValency(MeshT* mp, index_t ridge)
//  { return mp->m_idges[ridge].incValency(); }
//
//  inline int decValency(MeshT* mp, index_t ridge)
//  { return mp->m_idges[ridge].decValency(); }
//
//  inline void setAllMembers(MeshT* mp, index_t ridge, index_t const* ic, uint8_t const* pos, uint8_t const* tag, Flags const* flags, uint16_t const* ref_counter,
//                                                      uint16_t const* singular_counter)
//  {
//    Ridge & f = mp->m_idges[ridge];
//    f.setAllMembers(ic, pos, tag, flags, ref_counter, singular_counter);
//  }


};   // Ridge :::::::::::::::::::::::::::::::::::::::::





} // end namespace

#endif


