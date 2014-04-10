// This file is part of Alelib, a toolbox for finite element codes.
//
// Alelib is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// Alternatively, you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// Alelib is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License and a copy of the GNU General Public License along with
// Alelib. If not, see <http://www.gnu.org/licenses/>.

#ifndef FEPIC_MESHIOMSH_HPP
#define FEPIC_MESHIOMSH_HPP

#include "../mesh.hpp"
#include "../../util/timer.hpp"
#include "meshnamehandler.hpp"

namespace alelib
{

class MeshIoMsh : public iPathHandle
{
public:
  
  void readFileMsh(const char* filename, Mesh * mesh);
  ECellType identifiesMeshType(const char* filename, int &space_dim) const;
  
  Timer timer;
};



}

#endif




