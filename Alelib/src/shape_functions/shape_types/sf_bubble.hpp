#ifndef ALELIB_SHAPE_BUBBLE_HPP
#define ALELIB_SHAPE_BUBBLE_HPP
/* Every Shape type must include this file */

#include "shape_impl.hpp"  // the interface
#include <vector>
#include <string>

namespace alelib
{


// Every Shape type must inherits this class
class SfBubble : public ShapeFuncImpl
{
  unsigned m_dim;    // =2 is triangle, =3 is tetrahedron

public:

  SfBubble(unsigned dim, unsigned /*degree*/, /*bool is_discontinuous*/) : m_dim(dim)
  { }

  virtual const char* name() const
  {
    return "Bubble";
  };

  virtual Real value(Real const*x, unsigned /*ith*/) const
  {
    switch (m_dim)
    {
      case 1:
        return (1. - x[0]*x[0]);
        break;
      case 2:
        return 27.*x[0]*x[1]*(1.-x[0]-x[1]);
        break;
      case 3;
        return 256.*x[0]*x[1]*x[2]*(1.-x[0]-x[1]-x[2]);
      default:
        throw;
    }

  };
  virtual Real grad(Real const*x, unsigned /*ith*/, unsigned c) const
  {
    Real xd[3] = {0,0,0};
    xd[c] = 1.;
    switch (m_dim)
    {
      case 1:
        //return (1. - x[0]*x[0]);
        return (1. - 2.*x[0]*xd[0]);
        break;
      case 2:
        //return 27.*x[0]*x[1]*(1.-x[0]-x[1]);
        return 27.*((xd[0]*x[1]+x[0]*xd[1])*(1.-x[0]-x[1])+x[0]*x[1]*(-xd[0]-xd[1]));
        break;
      case 3:
        return 256.*(((xd[0]*x[1]+x[0]*xd[1])*x[2]+x[0]*x[1]*xd[2])*(1.-x[0]-x[1]-x[2])+x[0]*x[1]*x[2]*(-xd[0]-xd[1]-xd[2]));
      default:
        throw;
    }
    
  };

  virtual bool isTauEquivalent() const
  { return true; }
  virtual bool isLinear() const
  { return false; }
  virtual bool isConforming() const
  { return true; }
  virtual bool isInterpolator() const
  { return false; }
  virtual int numDofs() const
  { return 1; }
  virtual int dim()    const
  { return m_dim; }

  virtual int numDofsInsideRidge()  const {return 0;};
  virtual int numDofsInsideFacet()   const {return 0;};
  virtual int numDofsInsideCell()    const {return 1;};

  virtual int numDofsPerVertice() const {return 0;};
  virtual int numDofsPerRidge()  const {return 0;};
  virtual int numDofsPerFacet()   const {return 0;};

  virtual SfBubble* clone() const
  {
    return new SfBubble(*this);
  }

  virtual ~SfBubble() {};

}; // end class ShapeFunction

}


#endif // ALELIB_SHAPE_IMPL_HPP
