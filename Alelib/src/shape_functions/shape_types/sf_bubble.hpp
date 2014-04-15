#ifndef ALELIB_SHAPE_BUBBLE_HPP
#define ALELIB_SHAPE_BUBBLE_HPP
/* Every Shape type must include this file */

#include "shape_impl.hpp"  // the interface
#include <vector>
#include <string>
#include <cstdlib>
#include <cmath>
#include <algorithm>

namespace alelib
{


// Every Shape type must inherits this class
class SfBubble : public ShapeFuncImpl
{
  unsigned m_dim;    // =2 is triangle, =3 is tetrahedron
  std::string m_name; // contains name and options

public:

  SfBubble(unsigned dim, unsigned /*degree*/, bool /*is_discontinuous*/) : m_dim(dim), m_name("Bubble")
  {
    struct A {
      static inline std::string itos(int value)
      {

        std::string buf;

        enum { kMaxDigits = 35, base=10 };
        buf.reserve( kMaxDigits ); // Pre-allocate enough space.

        int quotient = value;

        // Translating number to string with base:
        do {
          buf += "0123456789"[ std::abs( quotient % base ) ];
          quotient /= base;
        } while ( quotient );

        // Append the negative sign
        if ( value < 0) buf += '-';

        std::reverse( buf.begin(), buf.end() );
        return buf;
      }   
    }; 
    
    m_name += ",";
    m_name += A::itos(dim);
  }

  virtual const char* name() const
  {
    return m_name.c_str();
  };

  static const char* nameId()
  {
    return "Bubble";
  };

  static ShapeFuncImpl* create(std::vector<std::string> /*options*/, int dim, int degree)
  {
    return new SfBubble(dim,degree,0);
  }

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
      case 3:
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
        return (- 2.*x[0]*xd[0]);
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
  Real hessian(const Real *x, unsigned /*ith*/, unsigned int c, unsigned d) const
  {
    /*ith*/
    Real xd[3] = {0.,0.,0.};
    xd[d] = 1.;
    Real yd[3] = {0., 0., 0.};
    yd[c] = 1.;
    switch (m_dim) {
      case 1 :
          return -(2.*yd[0]*xd[0]);
      case 2 :
          return 27.*((yd[0]*xd[1]+yd[1]*xd[0])*(1.-x[0]-x[1])+(yd[0]*x[1]+x[0]*
          yd[1])*(-xd[0]-xd[1])+(-yd[0]-yd[1])*(xd[0]*x[1]+x[0]*xd[1]));
      case 3 :
          return 256.*(((yd[0]*xd[1]+yd[1]*xd[0])*x[2]+(yd[0]*x[1]+x[0]*yd[1])*
          xd[2]+yd[2]*(xd[0]*x[1]+x[0]*xd[1]))*(1.-x[0]-x[1]-x[2])+((yd[0]*x[1]+
          x[0]*yd[1])*x[2]+x[0]*x[1]*yd[2])*(-xd[0]-xd[1]-xd[2])+(-yd[0]-yd[1]-
          yd[2])*((xd[0]*x[1]+x[0]*xd[1])*x[2]+x[0]*x[1]*xd[2]));
      default :
          throw;
    }
  }


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
