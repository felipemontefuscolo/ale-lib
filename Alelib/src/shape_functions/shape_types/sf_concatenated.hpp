#ifndef ALELIB_SF_CONCATENATED_HPP
#define ALELIB_SF_CONCATENATED_HPP

#include "shape_impl.hpp"  // the interface
#include <vector>
#include <string>

namespace alelib {


class SfConcatenated : public ShapeFuncImpl
{
  int m_ndofs;
  //int m_dim;
  
  // ndi = num dofs inside
  int m_ndir;
  int m_ndif;
  int m_ndic;
  
  // ndp = num dofs per
  int m_ndpv;
  int m_ndpr;
  int m_ndpf;
  
  std::vector<ShapeFuncImpl*> m_parts;
  std::string m_name;

  SfConcatenated(SfConcatenated const&);
public:
  
  SfConcatenated() : m_ndofs(0), m_ndir(0), m_ndif(0), m_ndic(0), m_ndpv(0), m_ndpr(0), m_ndpf(0) {};
  
  virtual const char* name() const;

  void appendSf(ShapeFuncImpl* sf);
	

  virtual Real value(Real const*x, unsigned ith) const;
  virtual Real grad(Real const*x, unsigned ith, unsigned c) const;

  virtual bool isTauEquivalent() const;
  virtual bool isLinear() const;
  virtual bool isConforming() const;
  virtual bool isInterpolator() const;
  virtual int numDofs() const;
  virtual int dim()    const;

  virtual int numDofsInsideRidge()  const;
  virtual int numDofsInsideFacet()   const;
  virtual int numDofsInsideCell()    const;

  virtual int numDofsPerVertice() const;
  virtual int numDofsPerRidge()  const;
  virtual int numDofsPerFacet()   const;

  virtual SfConcatenated* clone() const;

  virtual ~SfConcatenated();

private:

  // @param[in]  ith Number of the dof in the concatenated Shape Function
  // @param[out] local_ith Number of the dof in the correspondent Shape Function
  // @param[out] nth For which shape function `ith` corresponds:
  void whichShape(unsigned ith, int & local_ith, int & nth) const;

};


} // end namespace


#endif


