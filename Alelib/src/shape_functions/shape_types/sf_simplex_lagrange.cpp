#include "../parametric_pts.hpp" // probably you will need to include this

#include "sf_simplex_lagrange.hpp"
#include <stdexcept>
#include <cstdlib> // atoi
#include <cmath>
#include <Ead/ead.hpp>
#include <Ead/ead2.hpp>
#include <sstream>

#define ALELIB_LAGRANGE_DEG_LIMIT 30

namespace alelib
{

template<class Scalar>
Scalar lagrange(Scalar const*x_, unsigned ith, int degree, int dim, std::vector<long int> const& m_denominator, std::vector<int>const& m_integer_pts)
{
  if (degree == 0)
    return Scalar(1.);

  Scalar x[3];
  for (int i = 0; i < 3; ++i)
    x[i] = x_[i];

  if (dim==1) // rescale
    x[0] = 0.5*(1.+x[0]);

  Scalar L[4]; // barycentric coordinates
  L[0] = 1.;
  for (int k = 1; k <= dim; ++k)
  {
    L[k] = x[k-1];
    L[0] -= L[k];
  }

  int sup[4]; // final index in the products of sequences
  sup[0] = degree;
  for (int k = 1; k <= dim; ++k)
  {
    sup[k] = m_integer_pts.at(dim*ith + k-1);
    sup[0] -= sup[k];
  }

  Scalar numerator(1);
  for (int k = 0; k < dim+1; ++k)
  {
    //Scalar prod(1); // this commented part is a good code to test the EAD lib
    //for (int q = 0; q < sup[k]; q++)
    //{
    //  prod *= degree*L[k]-q;
    //}

    for (int q = 0; q < sup[k]; q++)
      numerator *= degree*L[k]-q;
    
    //numerator *= prod;
  }

  
  numerator /= m_denominator.at(ith);


  return numerator;
}


ShapeFuncImpl* SfSimplexLagrange::create(std::vector<std::string> /*options*/, int dim, int degree)
{
  return new Self(dim,degree,0);
}


SfSimplexLagrange::SfSimplexLagrange(unsigned dim, unsigned degree, bool /*is_disc*/) : m_degree(degree), m_dim(dim)
{
  if (dim < 1 || dim > 3)
    throw std::invalid_argument("SfSimplexLagrange constructor: `dim` must be in 1,2 or 3");
  if (m_degree > ALELIB_LAGRANGE_DEG_LIMIT)
    throw std::invalid_argument("SfSimplexLagrange constructor: `degree` exceeded ALELIB_LAGRANGE_DEG_LIMIT");

  switch (dim)
  {
    case 1: m_ndofs = (degree + 1); break;
    case 2: m_ndofs = (degree + 1)*(degree + 2)/2; break;
    case 3: m_ndofs = (degree + 1)*(degree + 2)*(degree + 3)/6; break;
  };

  Self::computeDenominators();
  
  std::stringstream ss;
  
  ss << "Lagrange," << dim << "," << degree;
  
  m_name = ss.str();
  
}


void SfSimplexLagrange::computeDenominators()
{
  m_denominator.clear();

  if (Self::degree() < 1)
  {
    m_denominator.push_back(1L);
    return;
  }

  switch (Self::dim())
  {
    case 1: genLineParametricPtsINT(Self::degree(), m_integer_pts); break;
    case 2: genTriParametricPtsINT(Self::degree(),  m_integer_pts); break;
    case 3: genTetParametricPtsINT(Self::degree(),  m_integer_pts); break;
  };

  int const dim = Self::dim();

  // rescale 1d case
  if (dim == 1)
  {
    for (int i = 0; i < (int)m_integer_pts.size(); ++i)
      m_integer_pts[i] = (m_integer_pts[i] + degree())/2;
  }

  m_denominator.resize( m_integer_pts.size()/dim );

  // compute `denominator` for each point i
  for (unsigned i = 0; i < m_integer_pts.size()/dim; ++i)
  {
    std::vector<long int> denom_term(dim+1, 1L); // denominator terms;
    std::vector<long int> sup(dim+1);      // final index in the products of sequences

    sup.front() = Self::degree();
    for (int c = 1; c <= dim; ++c)
    {
      sup.at(c) = m_integer_pts.at(dim*i + c-1);
      sup.front() -= sup.at(c);
    }

    for (int c = 0; c < dim+1; ++c)
      for (int k = 0; k < sup.at(c); ++k)
        denom_term.at(c) *= sup.at(c) - k;

    m_denominator.at(i) = 1;
    for (int c = 0; c < dim+1; ++c)
      m_denominator.at(i) *= denom_term.at(c);

  }

}

Real SfSimplexLagrange::value(Real const*x_, unsigned ith) const
{
  const int n_dofs = Self::numDofs();
  const int degree = Self::degree();
  const int dim    = Self::dim();

  if (ith >= (unsigned)n_dofs)
    throw std::out_of_range("SfSimplexLagrange::value: invalid index");

  Real ret = lagrange(x_, ith, degree, dim, m_denominator, m_integer_pts);
  
  return ret;

}

Real SfSimplexLagrange::grad(Real const*x_, unsigned ith, unsigned c) const
{
  const int n_dofs = Self::numDofs();
  const int degree = Self::degree();
  const int dim    = Self::dim();

  if (ith >= (unsigned)n_dofs)
    throw std::out_of_range("SfSimplexLagrange::value: invalid index");

  typedef ead::DFad<Real, 3> adouble;

  adouble x[3];
  
  for (int i = 0; i < dim; ++i)
  {
    x[i].setDiff(i,dim);
    x[i].val() = x_[i]; 
  }

  adouble ret(0,dim);
  ret.setNumVars(dim);
  
  ret = lagrange(x, ith, degree, dim, m_denominator, m_integer_pts);

  return ret.dx(c);
}

Real SfSimplexLagrange::hessian(Real const*x_, unsigned ith, unsigned c, unsigned d) const
{
  const int n_dofs = Self::numDofs();
  const int degree = Self::degree();
  const int dim    = Self::dim();

  if (ith >= (unsigned)n_dofs)
    throw std::out_of_range("SfSimplexLagrange::value: invalid index");

  typedef ead::D2Fad<Real, 3> adouble;

  adouble x[3];
  
  for (int i = 0; i < dim; ++i)
  {
    x[i].setDiff(i,dim);
    x[i].val() = x_[i]; 
  }

  adouble ret(0,dim);
  ret.setNumVars(dim);
  
  ret = lagrange(x, ith, degree, dim, m_denominator, m_integer_pts);

  return ret.d2x(c,d);
}

const char* SfSimplexLagrange::name() const
{
  return m_name.c_str();
}

// used for Shape Function registration
const char* SfSimplexLagrange::nameId()
{
  return "Lagrange";
}


bool SfSimplexLagrange::isTauEquivalent() const
{
  return true;
}

bool SfSimplexLagrange::isLinear() const
{
  return SfSimplexLagrange::degree() == 1;
}

bool SfSimplexLagrange::isConforming() const
{
  return SfSimplexLagrange::degree() > 0;
}

int SfSimplexLagrange::numDofs() const
{
  return m_ndofs;
}

int SfSimplexLagrange::dim() const
{
  return m_dim;
}

int SfSimplexLagrange::numDofsInRidge()  const
{
  if (Self::degree() == 0)
    return 0;
  switch (Self::dim())
  {
    case 1: return 0;
    case 2: return 1;
    case 3: return Self::degree()-1;
  }
  throw std::runtime_error("SfSimplexLagrange::numDofsInRidge: I should not be here");
}

int SfSimplexLagrange::numDofsInFacet()   const
{
  if (Self::degree() == 0)
    return 0;
  switch (Self::dim())
  {
    case 1: return 1;
    case 2: return Self::degree()-1;
    case 3: return (Self::degree()-1)*(Self::degree()-2)/2;
  }
  throw std::runtime_error("SfSimplexLagrange::numDofsInFacet: I should not be here");
}

int SfSimplexLagrange::numDofsInCell()    const
{
  if (Self::degree() == 0)
    return 1;
  switch (Self::dim())
  {
    case 1: return Self::degree()-1;
    case 2: return (Self::degree()-1)*(Self::degree()-2)/2;
    case 3: return (Self::degree()-1)*(Self::degree()-2)*(Self::degree()-3)/6;
  }
  throw std::runtime_error("SfSimplexLagrange::numDofsInCell: I should not be here");
}

int SfSimplexLagrange::numDofsPerVertex() const
{
  if (Self::degree() == 0)
    return 0;
  return 1;
}

int SfSimplexLagrange::numDofsPerRidge()  const
{
  if (Self::degree() == 0)
    return 0;
  switch (Self::dim())
  {
    case 1: return 0;
    case 2: return 1;
    case 3: return Self::degree()+1;
  }
  throw std::runtime_error("SfSimplexLagrange::numDofsPerRidge: I should not be here");
}

int SfSimplexLagrange::numDofsPerFacet()   const
{
  if (Self::degree() == 0)
    return 0;
  switch (Self::dim())
  {
    case 1: return 1;
    case 2: return Self::degree()+1;
    case 3: return (Self::degree()+1)*(Self::degree()+2)/2;
  }
  throw std::runtime_error("SfSimplexLagrange::numDofsPerFacet: I should not be here");
}


SfSimplexLagrange* SfSimplexLagrange::clone() const
{
  return new SfSimplexLagrange(*this);
}

int SfSimplexLagrange::degree() const
{
  return m_degree;
}

} // end alelib namespace
