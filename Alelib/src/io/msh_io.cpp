#include "msh_io.hpp"
#include <cstdio>
#include "../util/assert.hpp"
#include "msh_tags.hpp"
#include "alelib_tags.hpp"

#include <ciso646>  // detect std::lib
#ifdef _LIBCPP_VERSION
// using libc++
#define MULTI_HAVE_TYPE_TRAITS
#else
// using libstdc++
#define MULTI_HAVE_TR1_TYPE_TRAITS
#endif

#ifdef MULTI_HAVE_TYPE_TRAITS
#include <memory>
namespace Tr1 = std;
#else
#include <tr1/memory>
namespace Tr1 = std::tr1;
#endif

namespace alelib
{




} // end namespace alelib

