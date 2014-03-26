// This file is part of FEPiC++, a toolbox for finite element codes.
//
// FEPiC++ is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// Alternatively, you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// FEPiC++ is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License and a copy of the GNU General Public License along with
// FEPiC++. If not, see <http://www.gnu.org/licenses/>.

#ifndef ALELIB_DOF_TRAITS_HPP
#define ALELIB_DOF_TRAITS_HPP

#include "../shapefunctions/shape_declarations.hpp"


template<class ShapeClass>
struct DofTraits_T;


#define FEP_FORM_DOF_TRAITS(Shape, ndpv, ndpr, ndpf, ndpc, nv, nr, nf, di)     \
template<>                                              \
struct DofTraits_T<Shape>                                \
{                                                       \
  enum{ n_dof_per_vertice  = ndpv,    /*interior*/      \
        n_dof_per_corner   = ndpr,    /*interior*/      \
        n_dof_per_facet    = ndpf,    /*interior*/      \
        n_dof_per_cell     = ndpc,    /*interior*/      \
                                                        \
        n_vertices = nv,                                \
        n_corners  = nr,                                \
        n_facets   = nf,                                \
                                                        \
        dim = di,                                       \
                                                        \
        n_dof = n_dof_per_cell +                        \
                n_facets  * n_dof_per_facet +           \
                n_corners * n_dof_per_corner +          \
                n_vertices* n_dof_per_vertice           \
                                                        \
      };                                                \
                                                            \
                                                            \
                                                            \
                                                            \
}



// not account DOFs for twice
// nao contabiliza os graus de liberdade duas vezes
//                        Shape            , ndpv, ndpr, ndpf, ndpc, nv, nr, nf, di)
/* 1*/FEP_FORM_DOF_TRAITS(Shape_CONST      ,    0,    0,    0,    1,  0,  0,  0,  0);
/* 2*/FEP_FORM_DOF_TRAITS(Shape_EDGE_P1    ,    1,    0,    0,    0,  2,  0,  1,  1);
/* 3*/FEP_FORM_DOF_TRAITS(Shape_EDGE_P2    ,    1,    0,    0,    1,  2,  0,  1,  1);
/* 4*/FEP_FORM_DOF_TRAITS(Shape_TRI_P1     ,    1,    0,    0,    0,  3,  3,  3,  2);
/* 5*/FEP_FORM_DOF_TRAITS(Shape_TRI_P2     ,    1,    0,    1,    0,  3,  3,  3,  2);
/* 6*/FEP_FORM_DOF_TRAITS(Shape_TRI_BUBBLE ,    0,    0,    0,    1,  3,  3,  3,  2);
/* 7*/FEP_FORM_DOF_TRAITS(Shape_TRI_P1ph   ,    1,    0,    0,    1,  3,  3,  3,  2);
/* 8*/FEP_FORM_DOF_TRAITS(Shape_TRI_P2ph   ,    1,    0,    1,    1,  3,  3,  3,  2);
/* 9*/FEP_FORM_DOF_TRAITS(Shape_TRI_Pm1    ,    0,    0,    0,    3,  3,  3,  3,  2);
/*10*/FEP_FORM_DOF_TRAITS(Shape_QUAD_Q1    ,    1,    0,    0,    0,  4,  4,  4,  2);
/*11*/FEP_FORM_DOF_TRAITS(Shape_QUAD_Q2    ,    1,    0,    1,    1,  4,  4,  4,  2);
/*12*/FEP_FORM_DOF_TRAITS(Shape_QUAD_BUBBLE,    0,    0,    0,    1,  4,  4,  4,  2);
/*13*/FEP_FORM_DOF_TRAITS(Shape_QUAD_Q1ph  ,    1,    0,    0,    1,  4,  4,  4,  2);
/*14*/FEP_FORM_DOF_TRAITS(Shape_QUAD_Pm1   ,    0,    0,    0,    3,  4,  4,  4,  2);
/*15*/FEP_FORM_DOF_TRAITS(Shape_QUAD_Q2ser ,    1,    0,    1,    0,  4,  4,  4,  2);
/*16*/FEP_FORM_DOF_TRAITS(Shape_TET_P1     ,    1,    0,    0,    0,  4,  6,  4,  3);
/*17*/FEP_FORM_DOF_TRAITS(Shape_TET_P2     ,    1,    1,    0,    0,  4,  6,  4,  3);
/*18*/FEP_FORM_DOF_TRAITS(Shape_TET_BUBBLE ,    0,    0,    0,    1,  4,  6,  4,  3);
/*19*/FEP_FORM_DOF_TRAITS(Shape_TET_P1ph   ,    1,    0,    0,    1,  4,  6,  4,  3);
/*20*/FEP_FORM_DOF_TRAITS(Shape_TET_P2ph   ,    1,    1,    0,    1,  4,  6,  4,  3);
/*21*/FEP_FORM_DOF_TRAITS(Shape_TET_Pm1    ,    0,    0,    0,    4,  4,  6,  4,  3);
/*22*/FEP_FORM_DOF_TRAITS(Shape_HEX_Q1     ,    1,    0,    0,    0,  8, 12,  6,  3);
/*23*/FEP_FORM_DOF_TRAITS(Shape_HEX_Q2     ,    1,    1,    1,    1,  8, 12,  6,  3);
/*24*/FEP_FORM_DOF_TRAITS(Shape_HEX_BUBBLE ,    0,    0,    0,    1,  8, 12,  6,  3);
/*25*/FEP_FORM_DOF_TRAITS(Shape_HEX_Q1ph   ,    1,    0,    0,    1,  8, 12,  6,  3);
/*26*/FEP_FORM_DOF_TRAITS(Shape_HEX_Pm1    ,    0,    0,    0,    4,  8, 12,  6,  3);
/*27*/FEP_FORM_DOF_TRAITS(Shape_HEX_Q2ser  ,    1,    1,    0,    0,  8, 12,  6,  3);

#undef FEP_FORM_DOF_TRAITS

#endif






