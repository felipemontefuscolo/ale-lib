// This file is part of Alelib, a toolbox for finite element codes.
//
// Alelib is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// Alternatively, you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// Alelib is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License and a copy of the GNU General Public License along with
// Alelib. If not, see <http://www.gnu.org/licenses/>.

#ifndef ALELIB_QUADRATURE_HPP
#define ALELIB_QUADRATURE_HPP

#include "conf/directives.hpp"
#include "../mesh/enums.hpp"
#include <vector>

namespace alelib {

class Quadrature
{
public:

  struct Vec3 {
    Real z[3];
    Real& operator[] (int i) {return z[i];}
    Real const& operator[] (int i) const {return z[i];}
    operator Real* () {return z;}
    operator Real const* () const {return z;}
  }; 

  static void igetQuadrPtsHypercube(int n, int dim, std::vector<Vec3>& points, std::vector<Real>& weights);
  
  Quadrature() {};

  Quadrature(ECellType ct, int degree);

  void setType(ECellType ct, int degree);
  

  Real const* point(int qp) const
  { return m_qpoints[qp]; }
  
  Real weight(int qp) const
  { return m_weights[qp]; }

  int numPoints() const
  { return m_qpoints.size(); }

  int degree() const
  { return m_degree; }

protected:
  int m_degree;
  
  std::vector<Vec3> m_qpoints;
  std::vector<Real> m_weights;
};

} // end namespace alelib

#endif


