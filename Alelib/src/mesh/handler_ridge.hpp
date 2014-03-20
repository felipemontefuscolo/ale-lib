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

  void vertices(MeshT const* mp, VertexH *ridge_verts) const
  {
    if (MeshT::cell_dim == 3)
    {
      index_t const cell = icell(mp).id(mp);
      int const ridge_pos = localId(mp, icell(mp));
    
      for (int i = 0; i < MeshT::verts_per_ridge; ++i)
        *ridge_verts++ = mp->m_cells[cell].verts[mp->m_table_bC_x_vC(ridge_pos,i)];
    }
  }

  void points(MeshT const* mp, Point* ridge_pts) const
  {
    VertexH vs[2];
    vertices(mp, vs);
    ridge_pts[0] = mp->m_points[vs[0].id(mp)];
    ridge_pts[1] = mp->m_points[vs[1].id(mp)];
  }

  CellH icell(MeshT const* mp) const
  { return CellH(mp->m_ridges[m_id].icell); }

  inline int localId(MeshT const* mp, CellH const cell) const
  {
    CellT const& c = mp->m_cells[cell.id(mp)];
    for (int i = 0; i < (int)CellT::n_ridges; ++i)
    {
      if (c.ridges[i] == m_id)
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


};   // Ridge :::::::::::::::::::::::::::::::::::::::::





} // end namespace

#endif


