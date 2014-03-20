#ifndef ALELIB_POINT_HPP
#define ALELIB_POINT_HPP

namespace alelib
{
  
  
  

class Point
{
  Real m_coord[3]; // x,y,z
  
public:

  Point(Real x, Real y, Real z)
  { m_coord[0]=x; m_coord[1]=y; m_coord[2]=z;}
  
  Point(Real x, Real y)
  { m_coord[0]=x; m_coord[1]=y; m_coord[2]=0;}
  
  explicit
  Point(Real x)
  { m_coord[0]=x; m_coord[1]=0; m_coord[2]=0;}
  
  Point() {}
  
  /// Define a coordenada deste ponto.
  /// @param coord um vetor com a coordenada.
  inline void setCoord(Real const* coord, int spacedim)
  {
    for (int i = 0; i < spacedim; ++i)
      m_coord[i] = coord[i];
  }

  inline void setComponent(int i, Real x)
  { m_coord[i] = x; }

  /// Retorna a coordenada deste ponto em coord.
  /// @param[out] coord a coordenada.
  inline void coord(Real *coord, int spacedim) const
  {
    for (int i = 0; i < spacedim; ++i)
      coord[i] = m_coord[i];
  }

  inline Real const* coord() const
  {
    return &m_coord[0];
  }

  inline Real* coord()
  {
    return &m_coord[0];
  }

  /// Retorna a i-ésima componente da coordenada
  inline Real coord(int i) const
  {
    return m_coord[i];
  }
  
  Real& operator[](int i)
  { return m_coord[i]; }
  
  Real const& operator[](int i) const
  { return m_coord[i]; }
  
  friend
  Point operator+ (Point const& a, Point const& b)
  {
    return Point(a.coord(0)+b.coord(0), a.coord(1)+b.coord(1), a.coord(2)+b.coord(2));
  }
  
};



} // end namespace

#endif



