#ifndef ALELIB_SF_SIMPLEX_LAGRANGE
#define ALELIB_SF_SIMPLEX_LAGRANGE

#include "shape_impl.hpp"  // the interface
#include <vector>
#include <string>


namespace alelib
{

class SfSimplexLagrange : public ShapeFuncImpl
{  
  unsigned m_degree;
  unsigned m_dim;    // =2 is triangle, =3 is tetrahedron
  unsigned m_ndofs;
  std::string m_name; // contains name and options
  std::vector<long int> m_denominator; // lagrange functions have the form N/D. the denominators are computed once
  std::vector<int>      m_integer_pts; // integer coordinates of the master cells points

  typedef SfSimplexLagrange Self;
  typedef ShapeFuncImpl Base;

public:
  
  SfSimplexLagrange(unsigned dim, unsigned degree, bool is_discontinuous);

  // used for Shape Function registration
  static const char* nameId();
  const char* name() const;

  static ShapeFuncImpl* create(std::vector<std::string> options, int dim, int degree);

  virtual Real value(Real const*x, unsigned ith) const;
  virtual Real grad(Real const*x, unsigned ith, unsigned c) const;
  virtual Real hessian(Real const*x, unsigned ith, unsigned c, unsigned d) const;

  virtual bool isTauEquivalent() const;
  virtual bool isLinear() const;
  virtual bool isConforming() const;
  virtual bool isInterpolator() const {return true;}
  
  virtual int numDofs() const;
  virtual int dim()    const;

  virtual int numDofsInRidge()  const;
  virtual int numDofsInFacet()   const;
  virtual int numDofsInCell()    const;

  virtual int numDofsPerVertex() const;
  virtual int numDofsPerRidge()  const;
  virtual int numDofsPerFacet()   const;

  virtual SfSimplexLagrange* clone() const;

  int degree() const;

private:

  void computeDenominators();

  //Compute:  product(n*x-k, k, 0, Q-1). See documentation.
  double numeratorAt(int n, int Q, double x) const;
  
  double numeratorAt_d(int n, int Q, double x, double xd, double *numerator_val) const; // derivative of numeratorAt
  
};






} // namespace alelib



#endif // ALELIB_SF_SIMPLEX_LAGRANGE


