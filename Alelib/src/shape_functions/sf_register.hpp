#ifndef ALELIB_SF_REGISTER_HPP
#define ALELIB_SF_REGISTER_HPP

#include <map>
#include <vector>
#include <string>

/*  !!!!!!~~~~~~~ Include your file here ~~~~~~~!!!!!!!!! */
#include "shape_types/sf_simplex_lagrange.hpp"



typedef ShapeFuncImpl* SfImplPtr;
typedef SfImplPtr (*SfStaticMemFn)(std::vector<std::string>  options);



// !!!!!!~~~~~~~ Register your Shape Function type here ~~~~~~~!!!!!!!!!
//
// Your Shape Function type must have a factory static function named `create`
// which must returns a pointer to `ShapeFuncImpl`. This pointer must points
// to the instantiated object, of course.
//
// Also, your Shape Function must have a static function named `nameId` which
// returns a string. This string is a unique identification (a key) of Shape Function
//
std::map<std::string, SfStaticMemFn> init_register()
{
  std::map<std::string, SfStaticMemFn> reg;
  
  // Add here
  reg[SfSimplexLagrange::nameId()] = SfSimplexLagrange::create;
  
  return reg;
}





#endif // ALELIB_SF_REGISTER_HPP
