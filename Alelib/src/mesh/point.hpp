#ifndef ALELIB_POINT_HPP
#define ALELIB_POINT_HPP

namespace alelib
{
  
  
  
template<int Sdim>
class Point
{
  Real m_coord[Sdim]; // x,y,z
  
public:

  static const int SpaceDim = Sdim;

  explicit
  Point(Real x, Real y = 0, Real z = 0)
  {
    m_coord[0] = x;
    if (Sdim == 2)
      m_coord[1] = y;
    else if (Sdim == 3)
    {
      m_coord[1] = y;
      m_coord[2] = z;
    }
  }
  
  explicit
  Point(Real const* c)
  {
    for (int i = 0; i < Sdim; ++i)
      m_coord[i] = c[i];
  }
  
  Point() : m_coord(){}
  
  /// Define a coordenada deste ponto.
  /// @param coord um vetor com a coordenada.
  inline void setCoord(Real const* coord)
  {
    for (int i = 0; i < SpaceDim; ++i)
      m_coord[i] = coord[i];
  }

  inline void setComponent(int i, Real x)
  { m_coord[i] = x; }

  /// Retorna a coordenada deste ponto em coord.
  /// @param[out] coord a coordenada.
  inline void coord(Real *coord) const
  {
    for (int i = 0; i < SpaceDim; ++i)
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
  
};



} // end namespace

#endif



