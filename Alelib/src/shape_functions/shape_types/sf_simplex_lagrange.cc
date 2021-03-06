#include "../parametric_pts.hpp" // probably you will need to include this

#include "sf_simplex_lagrange.hpp"
#include <stdexcept>

#define ALELIB_LAGRANGE_DEG_LIMIT 30


ShapeFuncImpl* SfSimplexLagrange::create(std::vector<std::string> options)
{
  return new Self(-1,-1,-1);
}


SfSimplexLagrange::SfSimplexLagrange(unsigned dim, unsigned degree, bool is_disc) : m_degree(degree), m_dim(dim)
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

void SfSimplexLagrange::computeDenominators()
{
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
  
  m_denominator.clear();
  
  int const dim = Self::dim();
  
  // compute `denominator` for each point i
  for (unsigned i = 0; i < m_integer_pts.size()/dim; ++i)
  {
    std::vector<long int> denom_term(dim+1, 1L); // denominator terms;
    std::vector<long int> sup(dim+1);      // final index in the products of sequences
    
    sup.at(dim) = Self::degree();
    for (int c = 0; c < dim; ++c)
    {
      sup.at(c) = m_integer_pts.at(dim*i + c);
      sup.at(dim) -= sup.at(c);
    }

    for (int c = 0; c < dim+1; ++c)    
      for (int k = 0; k < sup.at(c); ++k)
        denom_term.at(c) *= sup.at(c) - k;
    
    m_denominator.at(i) = 1;
    for (int c = 0; c < dim+1; ++c)
      m_denominator.at(i) *= denom_term.at(c);
    
  }
  
}

Real SfSimplexLagrange::value(Real const*x, unsigned ith) const
{
  const int n_dofs = Self::numDofs();
  const int degree = Self::degree();
  const int dim    = Self::dim();
  
  
  if (degree == 0)
    return 1.;
  
  if (ith >= (unsigned)n_dofs)
    throw std::out_of_range("SfSimplexLagrange::value: invalid index");
  
  std::vector<double> L(dim + 1); // barycentric coordinates
  L.back() = 1.;
  for (int k = 0; k < dim; ++k)
  {
    L.at(k) = x[k];
    L.back() -= L.at(k);
  }
  
  std::vector<int> sup(dim+1); // final index in the products of sequences
  sup.at(dim) = degree;
  for (int c = 0; c < dim; ++c)
  {
    sup.at(c) = m_integer_pts.at(dim*ith + c);
    sup.at(dim) -= sup.at(c);
  }  
  
  double numerator = 1;
  for (int c = 0; c < dim+1; ++c)
    numerator *= Self::numeratorAt(degree, sup.at(c), L.at(c));
  
  return numerator/m_denominator.at(ith);
}

Real SfSimplexLagrange::grad(Real const*x, unsigned ith, unsigned c) const
{
  const int n_dofs = Self::numDofs();
  const int degree = Self::degree();
  const int dim    = Self::dim();
  
  if (degree == 0)
    return 0.;
  
  if (ith >= (unsigned)n_dofs)
    throw std::out_of_range("SfSimplexLagrange::grad: invalid index");
  
  if (c >= (unsigned)dim)
    throw std::out_of_range("SfSimplexLagrange::grad: invalid dimension");
  
  std::vector<double> L(dim + 1);       // barycentric coordinates
  std::vector<double> Ld(dim + 1, 0.0); // derivative
  L.back() = 1.;
  for (int k = 0; k < dim; ++k)
  {
    L.at(k) = x[k];
    L.back() -= L.at(k);
  }
  Ld[c]   =  1.0;
  Ld[dim] = -1.0;
  
  std::vector<int> sup(dim+1); // final index in the products of sequences
  sup.at(dim) = degree;
  for (int c = 0; c < dim; ++c)
  {
    sup.at(c) = m_integer_pts.at(dim*ith + c);
    sup.at(dim) -= sup.at(c);
  }  
  
  double numerator = 1;
  double numeratord = 0.0;
  double temp1, temp1d;
  for (int c = 0; c < dim+1; ++c)
  {
    //numerator *= Self::numeratorAt(degree, sup.at(c), L.at(c));
    temp1d = numeratorAt_d(degree, sup.at(c), L.at(c), Ld.at(c), &temp1);
    numeratord = numeratord*temp1 + numerator*temp1d;
    numerator *= temp1;
  }
  
  return numeratord/m_denominator.at(ith);
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

int SfSimplexLagrange::numDofsInsideRidge()  const
{
  if (Self::degree() == 0)
    return 0;
  switch (Self::dim())
  {
    case 1: return 0;
    case 2: return 1;
    case 3: return Self::degree()-1;
  }
  throw std::runtime_error("SfSimplexLagrange::numDofsInsideRidge: I should not be here");
}

int SfSimplexLagrange::numDofsInsideFacet()   const
{
  if (Self::degree() == 0)
    return 0;  
  switch (Self::dim())
  {
    case 1: return 1;
    case 2: return Self::degree()-1;
    case 3: return (Self::degree()-1)*(Self::degree()-2)/2;
  }
  throw std::runtime_error("SfSimplexLagrange::numDofsInsideFacet: I should not be here");
}

int SfSimplexLagrange::numDofsInsideCell()    const
{
  if (Self::degree() == 0)
    return 1;  
  switch (Self::dim())
  {
    case 1: return Self::degree()-1;
    case 2: return (Self::degree()-1)*(Self::degree()-2)/2;
    case 3: return (Self::degree()-1)*(Self::degree()-2)*(Self::degree()-3)/6;
  }
  throw std::runtime_error("SfSimplexLagrange::numDofsInsideCell: I should not be here");  
}

int SfSimplexLagrange::numDofsPerVertice() const
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

//Compute:  product(n*x-k, k, 0, Q-1). See documentation.
double SfSimplexLagrange::numeratorAt(int n, int Q, double x) const
{
  double prod=1;
  for (int k = 0; k < Q; k++)
    prod *= n*x-k;
  return prod;
}

double SfSimplexLagrange::numeratorAt_d(int n, int Q, double x, double xd, double *numerator_val) const
{
    double prod = 1;
    double prodd;
    prodd = 0.0;
    for (int k = 0; k < Q; ++k) {
        prodd = prodd*(n*x-k) + prod*n*xd;
        prod *= n*x - k;
    }
    *numerator_val = prod;
    return prodd;
}

SfSimplexLagrange* SfSimplexLagrange::clone() const
{
  return new SfSimplexLagrange(*this);
}

int SfSimplexLagrange::degree() const
{
  return m_degree;
}

