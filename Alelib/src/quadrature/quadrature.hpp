// This file is part of FEPiC++, a toolbox for finite element codes.
//
// FEPiC++ is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// Alternatively, you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// FEPiC++ is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License and a copy of the GNU General Public License along with
// FEPiC++. If not, see <http://www.gnu.org/licenses/>.

#ifndef ALELIB_QUADRATURE_HPP
#define ALELIB_QUADRATURE_HPP

#include "conf/directives.hpp"
#include "../mesh/enums.hpp"
#include <vector>

namespace alelib {

void igetQuadrPtsHypercube(int n, int dim, Real **points, Real *weights, int &nm_points);

class Quadrature
{
public:

  Quadrature(ECellType ct, int degree);

  void setType(ECellType ct, int degree);
  

  Real const* point(int qp) const
  {
    return m_points[qp];
  }
  Real weight(int qp) const
  {
    return m_weights[qp];
  }

  int numPoints() const
  {
    return m_points.size();
  }

  int degree() const
  {
    return m_degree;
  }

protected:
  int m_degree;
  
  typedef Real Vec3[3];
  
  std::vector<Vec3> m_points;
  std::vector<Real> m_weights;
};



// =============================================

//typedef Quadrature_<EDGE2,        1, 5  > Quadrature_EDGE;
//typedef Quadrature_<TRIANGLE3,    2, 25 > Quadrature_TRIANGLE;
//typedef Quadrature_<QUADRANGLE4,  2, 25 > Quadrature_QUADRANGLE;
//typedef Quadrature_<TETRAHEDRON4, 3, 46 > Quadrature_TETRAHEDRON;
//typedef Quadrature_<HEXAHEDRON8,  3, 125> Quadrature_HEXAHEDRON;

#define Quadrature_POINT       Quadrature_<POINT,       0, 1  , 10>
#define Quadrature_EDGE        Quadrature_<EDGE,        1, 5  , 9 >
#define Quadrature_TRIANGLE    Quadrature_<TRIANGLE,    2, 25 , 10>
#define Quadrature_QUADRANGLE  Quadrature_<QUADRANGLE,  2, 25 , 9 >
#define Quadrature_TETRAHEDRON Quadrature_<TETRAHEDRON, 3, 46 , 8 >
#define Quadrature_HEXAHEDRON  Quadrature_<HEXAHEDRON,  3, 125, 9 >

} // end namespace alelib

#endif


