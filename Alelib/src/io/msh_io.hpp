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

#ifndef ALELIB_MSH_IO_HPP
#define ALELIB_MSH_IO_HPP

#include "../util/assert.hpp"
#include "../mesh/mesh.hpp"
#include "../util/timer.hpp"
#include "path_handle.hpp"
#include "../mesh/enums.hpp"

namespace alelib
{

template<ECellType ECT>
class MeshIoMsh : public iPathHandle
{
  Mesh<ECT> const* m_mesh;
public:
  
  typedef Mesh<ECT> MeshT;
  typedef MeshIoMsh<ECT> Self;
  typedef typename MeshT::VertexH VertexH;
  typedef typename MeshT::RidgeH RidgeH;
  typedef typename MeshT::FacetH FacetH;
  typedef typename MeshT::CellH CellH;  

  static const ECellType CellType = ECT;

  MeshIoMsh(MeshT const*) {}
  
  void readFileMsh(const char* filename, MeshT * mesh);
  ECellType identifiesMeshType(const char* filename, int* space_dim = NULL) const;
  
  Timer timer;
};



}

#endif




