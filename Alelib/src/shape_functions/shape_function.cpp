#include "shape_function.hpp"
#include "sf_register.hpp"
#include <stdexcept> 
#include <sstream>
#include <algorithm>
#include "../util/assert.hpp"



#include <iostream>







#ifndef ALELIB_DOXYGEN_DEV


namespace alelib
{


ShapeFunction::ShapeFunction() : m_pimpl(0), m_gdisc(false)
{
}

ShapeFunction::~ShapeFunction()
{
  if (m_pimpl)
  {
    delete m_pimpl;
    m_pimpl = NULL;
  }
}

bool ShapeFunction::isSet() const
{
  return m_pimpl != NULL;
}

const char* ShapeFunction::name() const
{
  return m_pimpl->name();
}

ShapeFunction::ShapeFunction(ShapeFunction const& cp)
{
  if (cp.isSet())
    m_pimpl = cp.m_pimpl->clone();
  else
    m_pimpl = 0;
}

ShapeFunction& ShapeFunction::operator=(ShapeFunction const& cp)
{
  if (&cp != this)
  {
    delete m_pimpl;
    m_pimpl = cp.m_pimpl->clone();
  }
  return *this;
}

void ShapeFunction::setType(const char* type, int dim, int degree)
{
  static const std::map<std::string, SfStaticMemFn> registry = init_register();
  std::map<std::string, SfStaticMemFn>::const_iterator it;
  
  std::string input(type);
  
  // remove white spaces
  input.erase(  std::remove(input.begin(), input.end(), ' '), input.end() );
  
  std::stringstream ss(input);
  std::vector<std::string> shapes;
  std::string stmp;
  std::string type_i;
  std::vector<std::string> options_i;

  // split user input if it's a concatenation of Shape Functions
  while(std::getline(ss, stmp, '+'))
    shapes.push_back(stmp);

  if (m_pimpl)
    delete m_pimpl;
  
  m_pimpl = new SfConcatenated();

  for (int i = 0; i < (int) shapes.size(); ++i)
  {
    // clear ss
    ss.str(std::string());
    ss.clear();
    ss << shapes[i];

    // clear options
    options_i.clear();

    // splits `shapes` into type and options
    while(std::getline(ss, stmp, ','))
      options_i.push_back(stmp);
    type_i = options_i[0];
    options_i.erase(options_i.begin());
    
    it = registry.find(shapes[i]);
    ALELIB_CHECK(it != registry.end(), "invalid shape function", std::invalid_argument);
    
    // create
    static_cast<SfConcatenated*>(m_pimpl)->appendSf( (*it).second(options_i, dim, degree) );
    
    // TODO
    
  }
  
  
  
  
  //m_pimpl = new SfSimplexLagrange(2,1,false);
}

Real ShapeFunction::value(Real const*x, unsigned ith) const
{ return m_pimpl->value(x, ith); }

Real ShapeFunction::grad(Real const*x, unsigned ith, unsigned c) const
{ return m_pimpl->grad(x, ith, c); }

Real ShapeFunction::hessian(Real const*x, unsigned ith, unsigned c, unsigned d) const
{ return m_pimpl->hessian(x, ith, c, d); }

bool ShapeFunction::isTauEquivalent() const
{ return m_pimpl->isTauEquivalent(); }

bool ShapeFunction::isLinear() const
{ return m_pimpl->isLinear(); }

bool ShapeFunction::isGloballyDiscontinuous() const
{ return m_gdisc; }

int  ShapeFunction::numDofs() const
{ return m_pimpl->numDofs(); }

int  ShapeFunction::dim()    const
{ return m_pimpl->dim(); }

int  ShapeFunction::numDofsInsideRidge()  const
{ return m_pimpl->numDofsInsideRidge(); }

int  ShapeFunction::numDofsInsideFacet()   const
{ return m_pimpl->numDofsInsideFacet(); }

int  ShapeFunction::numDofsInsideCell()    const
{ return m_pimpl->numDofsInsideCell(); }

int  ShapeFunction::numDofsPerVertice() const
{ return m_pimpl->numDofsPerVertice(); }

int  ShapeFunction::numDofsPerRidge()  const
{ return m_pimpl->numDofsPerRidge(); }

int  ShapeFunction::numDofsPerFacet()   const
{ return m_pimpl->numDofsPerFacet(); }

//int  ShapeFunction::numDofsPerCell()   const
//{ return m_pimpl->numDofs(); }


}

#endif


