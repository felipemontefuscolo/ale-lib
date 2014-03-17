#include "sf_concatenated.hpp"
#include <stdexcept>



SfConcatenated::SfConcatenated(SfConcatenated const& cp)
{
  
}

const char* SfConcatenated::name() const
{
  return m_name.c_str();
}

Real SfConcatenated::value(Real const*x, unsigned ith) const
{
  // find which function will be called (nth function)
  int nth;
  int local_ith;
  
  if (ith >= (unsigned)m_ndofs)
    throw std::out_of_range("SfConcatenated::value: invalid index");  
  
  whichShape(ith, local_ith, nth);
  
  return m_parts[nth]->value(x, local_ith);
}

Real SfConcatenated::grad(Real const*x, unsigned ith, unsigned c) const;
{
  // find which function will be called (nth function)
  int nth;
  int local_ith;
  
  if (ith >= (unsigned)n_dofs)
    throw std::out_of_range("SfConcatenated::value: invalid index");  
  
  whichShape(ith, local_ith, nth);
  
  return m_parts[nth]->grad(x, local_ith, c);
}

bool SfConcatenated::isTauEquivalent() const
{
  bool b = true;
  
  for (int i = 0; i < m_parts[i].size(); ++i)
    b = b & m_parts[i]->isTauEquivalent();
  
  return b;
}

bool SfConcatenated::isLinear() const
{
  bool b = true;
  
  for (int i = 0; i < m_parts[i].size(); ++i)
    b = b & (m_parts[i]->isLinear() || (m_parts[i]->numDofs()==1));
  
  return b;
}

bool SfConcatenated::isConforming() const
{
  bool b = true;
  
  for (int i = 0; i < m_parts[i].size(); ++i)
    b = b & m_parts[i]->isConforming();
  
  return b;
}

int SfConcatenated::numDofs() const
{
  return m_ndofs;
}

int SfConcatenated::dim() const
{
  return m_dim;
}

int SfConcatenated::numDofsInsideCorner()  const
{
  return m_ndir;
}

int SfConcatenated::numDofsInsideFacet()   const
{
  return m_ndif;
}

int SfConcatenated::numDofsInsideCell()    const
{
  return m_ndic;
}

int SfConcatenated::numDofsPerVertice() const
{
  return m_ndpv;
}

int SfConcatenated::numDofsPerCorner()  const
{
  return m_ndpr;
}

int SfConcatenated::numDofsPerFacet()   const
{
  return m_ndpf;
}

SfConcatenated* clone() const
{
  return new SfConcatenated(*this);
}

void SfConcatenated::whichShape(int ith, int & local_ith, int & nth) const
{
  for (nth = 0; ; ++nth)
  {
    if (ith < (int)m_parts.at(nth)->numDofs())
    {
      local_ith = ith;
      return;
    }
    else
      ith -= m_parts.at(nth)->numDofs();
  }
}
