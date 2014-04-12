#include "sf_concatenated.hpp"
#include <stdexcept>

namespace alelib {


SfConcatenated::SfConcatenated(SfConcatenated const& c)
{
  m_ndofs = c.m_ndofs;
  m_ndir  = c.m_ndir ;
  m_ndif  = c.m_ndif ;
  m_ndic  = c.m_ndic ;
  m_ndpv  = c.m_ndpv ;
  m_ndpr  = c.m_ndpr ;
  m_ndpf  = c.m_ndpf ;
  
  m_name = c.m_name;  
  
  m_parts.resize(c.m_parts.size());
  for (int i = 0; i < (int)m_parts.size(); ++i)
    m_parts[i] = c.m_parts[i]->clone();
  
}

SfConcatenated:: ~SfConcatenated()
{
  for (int i = 0; i < (int)m_parts.size(); ++i)
  {
    if (m_parts[i])
      delete m_parts[i];
    m_parts[i] = NULL;
  }
}

void SfConcatenated::appendSf(ShapeFuncImpl* sf)
{
  m_parts.push_back(sf);
  m_name += '+';
  m_name += sf->name();

  m_ndofs += sf->numDofs()           ;
  m_ndir  += sf->numDofsInsideRidge();
  m_ndif  += sf->numDofsInsideFacet();
  m_ndic  += sf->numDofsInsideCell() ;
  m_ndpv  += sf->numDofsPerVertice() ;
  m_ndpr  += sf->numDofsPerRidge()   ;
  m_ndpf  += sf->numDofsPerFacet()   ;

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

Real SfConcatenated::grad(Real const*x, unsigned ith, unsigned c) const
{
  // find which function will be called (nth function)
  int nth;
  int local_ith;

  if (ith >= (unsigned)m_ndofs)
    throw std::out_of_range("SfConcatenated::value: invalid index");

  whichShape(ith, local_ith, nth);

  return m_parts[nth]->grad(x, local_ith, c);
}

bool SfConcatenated::isTauEquivalent() const
{
  bool b = true;

  for (int i = 0; i < (int)m_parts.size(); ++i)
    b = b & m_parts[i]->isTauEquivalent();

  return b;
}

bool SfConcatenated::isLinear() const
{
  bool b = true;

  for (int i = 0; i < (int)m_parts.size(); ++i)
    b = b & (m_parts[i]->isLinear() || (m_parts[i]->numDofs()==1));

  return b;
}

bool SfConcatenated::isConforming() const
{
  bool b = true;

  for (int i = 0; i < (int)m_parts.size(); ++i)
    b = b & m_parts[i]->isConforming();

  return b;
}

int SfConcatenated::numDofs() const
{
  return m_ndofs;
}

int SfConcatenated::dim() const
{
  return m_parts[0]->dim();
}

int SfConcatenated::numDofsInsideRidge()  const
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

int SfConcatenated::numDofsPerRidge()  const
{
  return m_ndpr;
}

int SfConcatenated::numDofsPerFacet()   const
{
  return m_ndpf;
}

SfConcatenated* SfConcatenated::clone() const
{
  return new SfConcatenated(*this);
} 

void SfConcatenated::whichShape(unsigned ith, int & local_ith, int & nth) const
{
  for (nth = 0; ; ++nth)
  {
    if (ith < (unsigned)m_parts.at(nth)->numDofs())
    {
      local_ith = ith;
      return;
    }
    else
      ith -= m_parts.at(nth)->numDofs();
  }
}


bool SfConcatenated::isInterpolator() const
{
  throw;
  return false;
}

} // end namespace



