class RidgeH
{
  template<typename Traits_>
  friend class Mesh;

  typedef Mesh<Traits> MeshT; 
  typedef MeshT::RidgeT RidgeT;

  typedef RidgeH Self;

  index_t m_id;

public:

  RidgeH(index_t id) :  m_id(id) {}

  RidgeH(MeshT const*, index_t rid) :  m_id(rid) {}

  RidgeH() :  m_id(NULL_IDX) {}

  inline index_t id(MeshT const*) const
  { return m_id; }

  /// For debug purposes, it contains isNull and isDisabled
  /// This check if this entity is usable.
  bool isValid(MeshT const* mp) const
  {
    bool b = ! (isNull(mp) || isDisabled(mp));
    
    if ((size_type)m_id < mp->numRidges())
      return b;
    else
      return false;
  }

  bool isNull(MeshT const* = 0) const
  { return m_id == (index_t)NULL_IDX; }

  bool isDisabled(MeshT const* mp) const
  { return mp->m_ridges[m_id].isDisabled();};

  RidgeT& user(MeshT* mp) const
  { return mp->m_verts[m_id];}
  
  RidgeT const& user(MeshT const* mp) const
  { return mp->m_verts[m_id];}

  void setTag(MeshT* mp, int tag)
  { mp->m_ridges[m_id].setTag(tag); }

  int tag(MeshT const* mp) const
  { return mp->m_ridges[m_id].getTag(); }

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

  void points(MeshT const* mp, PointT* ridge_pts) const
  {
    ALE_STATIC_CHECK(StoreCoords, ThisMeshDoesNotStoreCoordinates);
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

  // in this version the cell is the icell
  inline int localId(MeshT const* mp) const
  {
    return mp->m_ridges[m_id].local_id;
  }

  // number os cells that contain this vertex
  inline unsigned valency(MeshT const* mp) const
  { return mp->m_ridges[m_id].valency; }

  inline std::vector<CellH> star(MeshT const* mp) const
  {
    VertexH verts[2];
    vertices(mp, verts);
    std::vector<CellH> star0 = verts[0].star(mp);
    std::vector<CellH> star1 = verts[1].star(mp);
    std::vector<CellH> s;
    s.reserve(std::min(star0.size(), star1.size()));
    std::set_intersection(star0.begin(), star0.end(),
                          star1.begin(), star1.end(),
                          std::back_inserter(s));
    return s;
  }

  void first2icells(MeshT const* mp, CellH ics[]) const
  {
    ALELIB_CHECK(CellT::dim == 3, "FacetH::first2icells only can be called by 3-cell meshes", std::invalid_argument);
    
    VertexH vts[2];
    this->vertices(mp, vts);
    typedef typename VertexT::VectorT VectorT;
    //typedef typename VectorT::const_iterator iterator;
    
    VectorT const& star0 = mp->m_verts[vts[0].id(mp)].icells;
    VectorT const& star1 = mp->m_verts[vts[1].id(mp)].icells;
    
     // put index_t conversion here because g++ complains
    index_t intersect[2] = {index_t(NULL_IDX), index_t(NULL_IDX)};

    set_2_intersection(star0.begin(), star0.end(),
                       star1.begin(), star1.end(), intersect);
                       
    ics[0] = CellH(intersect[0]);
    ics[1] = CellH(intersect[1]);
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
