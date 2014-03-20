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


#ifndef ALE_DEFAULT_MAP_HPP
#define ALE_DEFAULT_MAP_HPP

#include "parametric_pts.hpp"
#include "../mesh/enums.hpp"

namespace alelib
{

template<ECellType CT>
inline void genParametricPts(int, std::vector<double> &) {throw;}

template<ECellType CT>
inline void subCells(int, std::vector<int> &) {throw;}



template<> inline
void genParametricPts<POINT>(int, std::vector<double> &) { }

template<> inline
void genParametricPts<EDGE>(int n, std::vector<double> & list) {return genLineParametricPts(n,list);}

template<> inline
void genParametricPts<TRIANGLE>(int n, std::vector<double> & list) {return genTriParametricPts(n,list);}

template<> inline
void genParametricPts<TETRAHEDRON>(int n, std::vector<double> & list) {return genTetParametricPts(n,list);}



template<> inline
void subCells<TRIANGLE>(int n, std::vector<int> &connectivity) {return subCellsTri(n, connectivity); }

template<> inline
void subCells<TETRAHEDRON>(int n, std::vector<int> &connectivity) {return subCellsTet(n, connectivity); }



template<ECellType C> struct CTypeTraits {};

#define ALE_TAGTRAITS(CT, vtktag, ndpr, ndpf, ndpc, nppc, nsubc, FT )                 \
                      template<> struct CTypeTraits<CT> {                             \
                        static int const vtk_tag = vtktag;                            \
                        static ECellType const EFacetType = FT;                       \
                        static int np_in_ridge(int n) { return ndpr; }                \
                        static int np_in_facet(int n) { return ndpf; }                \
                        static int np_in_cell (int n) { return ndpc; }                \
                        static int np_per_cell(int n) { return nppc; }                \
                        static int n_sub_cells(int n) { return nsubc; }               \
                        static void master_to_real(int n_pts,                         \
                                                   Real const* ref_pts,               \
                                                   Point const* verts,                \
                                                   Point*       real_pts);            \
                      };


//            CellType     VtkTag   ridge  facet          cell                 cell(total)            n_sub_cells  facet type
ALE_TAGTRAITS(EDGE       ,  3,      0*n,   0*n          , n-1                , n+1                 ,  n         ,  POINT     )
ALE_TAGTRAITS(TRIANGLE   ,  5,      0*n,   n-1          , (n-1)*(n-2)/2      , (n+1)*(n+2)/2       ,  n*n       ,  EDGE      )
ALE_TAGTRAITS(QUADRANGLE ,  9,      0*n,   n-1          , (n-1)*(n-1)        , (n+1)*(n+1)         ,  n*n       ,  EDGE      )
ALE_TAGTRAITS(TETRAHEDRON, 10,      n-1,   (n-1)*(n-2)/2, (n-1)*(n-2)*(n-3)/6, (n+1)*(n+2)*(n+3)/6 ,  n*n*n     ,  TRIANGLE  )
ALE_TAGTRAITS(HEXAHEDRON , 12,      n-1,   (n-1)*(n-1)  , (n-1)*(n-1)*(n-1)  , (n+1)*(n+1)*(n+1)   ,  n*n*n     ,  QUADRANGLE)


// Mesh default interpolation

void CTypeTraits<EDGE>::master_to_real(int n_pts,
                                       Real const* L,          // pts in master cell
                                       Point const* verts,
                                       Point*       X)         // pts in real cell
{
  if(n_pts <= 0)
    return;
  for (int i = 0; i < n_pts; ++i)
  {
    Real const t = L[i];
    Real const s = 1. - t;

    X[i][0] = verts[0][0]*s + verts[1][0]*t;
    X[i][1] = verts[0][1]*s + verts[1][1]*t;
    X[i][2] = verts[0][2]*s + verts[1][2]*t;
  }
}

void CTypeTraits<TRIANGLE>::master_to_real(int n_pts,
                                      Real const* L,          // pts in master cell
                                      Point const* verts,
                                      Point*       X)         // pts in real cell
{
  if(n_pts <= 0)
    return;
  for (int i = 0; i < n_pts; ++i)
  {
    Real const t = L[2*i];
    Real const u = L[2*i + 1];
    Real const s = 1. - t - u;

    X[i][0] = verts[0][0]*s + verts[1][0]*t + verts[2][0]*u;
    X[i][1] = verts[0][1]*s + verts[1][1]*t + verts[2][1]*u;
    X[i][2] = verts[0][2]*s + verts[1][2]*t + verts[2][2]*u;
  }
}

void CTypeTraits<TETRAHEDRON>::master_to_real(int n_pts,
                                         Real const* L,          // pts in master cell
                                         Point const* verts,
                                         Point*       X)         // pts in real cell
{
  if(n_pts <= 0)
    return;
  for (int i = 0; i < n_pts; ++i)
  {
    Real const t = L[3*i];
    Real const u = L[3*i + 1];
    Real const v = L[3*i + 2];
    Real const s = 1. - t - u - v;

    X[i][0] = verts[0][0]*s + verts[1][0]*t + verts[2][0]*u + verts[3][0]*v;
    X[i][1] = verts[0][1]*s + verts[1][1]*t + verts[2][1]*u + verts[3][1]*v;
    X[i][2] = verts[0][2]*s + verts[1][2]*t + verts[2][2]*u + verts[3][2]*v;
  }
}

//
///
////
/////
//////
///////
////////
/////////
//////////
///////////
////////////
/////////////
//////////////
///////////////
////////////////
/////////////////
//////////////////
///////////////////
// ----------------------------------------
///////////////////
//////////////////
/////////////////
////////////////
///////////////
//////////////
/////////////
////////////
///////////
//////////
/////////
////////
///////
//////
/////
////
///
//





}

#endif

