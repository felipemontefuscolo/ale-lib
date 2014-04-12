// Copyright 2005, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// --gtest_catch_exceptions=0



#include <gtest/gtest.h>
#include <Alelib/Mesh>
#include <Alelib/IO>
#include <Alelib/ShapeFunction>
#include <Alelib/src/shape_functions/parametric_pts.hpp>
#include <tr1/array>
#include <tr1/tuple>
#include <algorithm>
#include <tr1/memory>
#include <limits> // for std::numeric_limits<Real>::epsilon()
#include <tr1/functional>
//#include <functional>
#include <tr1/array>
#include <tr1/tuple>
#include <cmath>

using namespace std::tr1::placeholders;


using namespace alelib;
using namespace std;

#define MAX(a,b) ((a) > (b) ? (a) : (b))

Real const ALE_EPS = std::numeric_limits<Real>::epsilon();
Real const ALE_TOL = 500000*ALE_EPS; // ~ 1.1e-10 para double

static const int dim1 = 1;
static const int dim2 = 2;
static const int dim3 = 3;

typedef Mesh<EDGE>        MeshEdg;
typedef Mesh<TRIANGLE>    MeshTri;
typedef Mesh<QUADRANGLE>  MeshQua;
typedef Mesh<TETRAHEDRON> MeshTet;
typedef Mesh<HEXAHEDRON>  MeshHex;

// calculates the derivative a function f(x)
// f: callable object in form  "Real f(Real const*)"
template<class F>
Real diff_(F const& f, Real const*x, int c, int Dim)
{
	//Real h = MAX(fabs(x[c]), 1.)*pow(ALE_EPS, 1./3.);
  //Real h = 0.41*pow(ALE_EPS, 1./3.); // for 2-points method
  Real h = 11*pow(ALE_EPS, 1./3.); // for 4-points method
	volatile Real t = h + x[c];
	h = t - x[c];

	Real y[Dim];
	Real z[Dim];
	Real zz[Dim]; // high order
	Real yy[Dim];

  for (int i = 0; i < Dim; ++i)
  {
    y[i] = x[i];
    z[i] = x[i];
    yy[i] = x[i];
    zz[i] = x[i];
  }

	y[c]  += h;
	z[c]  -= h;
	yy[c] += 2.*h;
	zz[c] -= 2.*h;

  //return (f(y)-f(z))/(2.*h);
	return (-f(yy) + 8.*(f(y) - f(z)) + f(zz))/(12.*h); // for higher order
}


class ShapeTri1Tests : public testing::Test
{
 protected:  // You should make the members protected s.t. they can be
             // accessed from sub-classes.

  typedef MeshTri MeshT;
  typedef typename MeshT::VertexH VertexH;
  typedef typename MeshT::CellH CellH;
  typedef typename MeshT::FacetH FacetH;
  typedef typename MeshT::RidgeH RidgeH;
  typedef MeshIoVtk<MeshT::CellType> MeshWriter;

  ShapeTri1Tests() : m(dim2) {}

  MeshT m;

  static void addMesh1(MeshTri &m, int n)
  {
    std::vector<double> xyz;
    std::vector<int> tris;

    genTriParametricPts(n, xyz);
    subCellsTet(n, tris);

    for (int i = 0; i < (n+1)*(n+2)/2; ++i)
      m.addVertex(Point(xyz.at(2*i + 0),xyz.at(2*i + 1)));

    for (int i = 0; i < n*n; ++i)
      m.addCell(listOf(VertexH(tris.at(3*i)), VertexH(tris.at(3*i+1)), VertexH(tris.at(3*i+2))));
  }

  // virtual void SetUp() will be called before each test is run.  You
  // should define it if you need to initialize the varaibles.
  // Otherwise, this can be skipped.
  virtual void SetUp() {

  }

  // virtual void TearDown() will be called after each test is run.
  // You should define it if there is cleanup work to do.  Otherwise,
  // you don't have to provide it.
  //
  virtual void TearDown() {
  }




};





TEST_F(ShapeTri1Tests, LagrangeDeltaProperty)
{
  //Real x[3] = {0.0,0.0,0.0};

  std::vector<double> xyz;
  ShapeFunction sf;

  for (int k = 1; k < 10; ++k)
  {
    sf.setType("Lagrange", /*dim*/2, /*degree*/k);
    genTriParametricPts(k, xyz);

    ASSERT_EQ((int)xyz.size()/2, sf.numDofs());

    for (int i = 0; i < (int)xyz.size()/2; ++i)
    {
      for (int j = 0; j < (int)xyz.size()/2; ++j)
      {
        if (i==j)
          EXPECT_NEAR(1, sf.value(listOf(xyz[2*j], xyz[2*j+1]), j), ALE_TOL);
        else
          EXPECT_NEAR(0, sf.value(listOf(xyz[2*i], xyz[2*i+1]), j), ALE_TOL);
      }
    }
  }
}

TEST_F(ShapeTri1Tests, LagrangeGradient)
{

  std::vector<double> xyz;
  ShapeFunction sf;
  int const sdim = 2;

  // Shape function placeholder
  std::tr1::function<Real (Real const*)> Func;

  for (int degree = 1; degree < 9; ++degree)
  {
    sf.setType("Lagrange", /*dim*/2, degree);
    genTriParametricPts(degree, xyz);

    for (int i = 0; i < (int)xyz.size()/2; ++i)
    {
      Real x[] = {xyz[2*i], xyz[2*i+1]};
      
      for (int dof = 0; dof < (int)xyz.size()/2; ++dof)
      {
        Func = std::tr1::bind(std::tr1::mem_fn(&ShapeFunction::value), sf, _1, dof);
        
        for (int c = 0; c < sdim; ++c)
        {
          ASSERT_NEAR(diff_( Func, x, c, sdim ),  sf.grad(x, dof, c), ALE_TOL)
          << "degree: " << degree << "\npoint: " << i << "\ndof: " << dof << "\ncomp:" << c << "\nh:" << pow(ALE_EPS, 1./3.) <<  endl;
        }
      }
    }
  }



}





class ShapeTet1Tests : public testing::Test
{
 protected:  // You should make the members protected s.t. they can be
             // accessed from sub-classes.

  typedef MeshTet MeshT;
  typedef typename MeshT::VertexH VertexH;
  typedef typename MeshT::CellH CellH;
  typedef typename MeshT::FacetH FacetH;
  typedef typename MeshT::RidgeH RidgeH;
  typedef MeshIoVtk<MeshT::CellType> MeshWriter;

  ShapeTet1Tests() : m(dim3) {}

  MeshT m;

  static void addMesh1(MeshTet &m, int n)
  {
    std::vector<double> xyz;
    std::vector<int> tets;

    genTetParametricPts(n, xyz);
    subCellsTet(n, tets);

    for (int i = 0; i < (n+1)*(n+2)*(n+3)/6; ++i)
      m.addVertex(Point(xyz.at(3*i + 0),xyz.at(3*i + 1),xyz.at(3*i + 2)));

    for (int i = 0; i < n*n*n; ++i)
      m.addCell(listOf(VertexH(tets.at(4*i)), VertexH(tets.at(4*i+1)), VertexH(tets.at(4*i+2)), VertexH(tets.at(4*i+3))));
  }

  // virtual void SetUp() will be called before each test is run.  You
  // should define it if you need to initialize the varaibles.
  // Otherwise, this can be skipped.
  virtual void SetUp() {

  }

  // virtual void TearDown() will be called after each test is run.
  // You should define it if there is cleanup work to do.  Otherwise,
  // you don't have to provide it.
  //
  virtual void TearDown() {
  }




};




TEST_F(ShapeTet1Tests, LagrangeDeltaProperty)
{
  //Real x[3] = {0.0,0.0,0.0};

  std::vector<double> xyz;
  ShapeFunction sf;
  int const sdim = 3;

  for (int k = 1; k < 10; ++k)
  {
    sf.setType("Lagrange", /*dim*/sdim, /*degree*/k);
    genTetParametricPts(k, xyz);

    ASSERT_EQ((int)xyz.size()/sdim, sf.numDofs());

    for (int i = 0; i < (int)xyz.size()/sdim; ++i)
    {
      for (int j = 0; j < (int)xyz.size()/sdim; ++j)
      {
        if (i==j)
          EXPECT_NEAR(1, sf.value(listOf(xyz[3*j], xyz[3*j+1], xyz[3*j+2]), j), ALE_TOL);
        else
          EXPECT_NEAR(0, sf.value(listOf(xyz[3*i], xyz[3*i+1], xyz[3*j+2]), j), ALE_TOL);
      }
    }
  }
}

TEST_F(ShapeTet1Tests, LagrangeGradient)
{

  std::vector<double> xyz;
  ShapeFunction sf;
  int const sdim = 3;

  // Shape function placeholder
  std::tr1::function<Real (Real const*)> Func;

  for (int degree = 1; degree < 6; ++degree)
  {
    sf.setType("Lagrange", /*dim*/3, degree);
    genTetParametricPts(degree, xyz);

    for (int i = 0; i < (int)xyz.size()/3; ++i)
    {
      Real x[] = {xyz[3*i], xyz[3*i+1], xyz[3*i+2]};
      
      for (int dof = 0; dof < (int)xyz.size()/3; ++dof)
      {
        Func = std::tr1::bind(std::tr1::mem_fn(&ShapeFunction::value), sf, _1, dof);
        
        for (int c = 0; c < sdim; ++c)
        {
          ASSERT_NEAR(diff_( Func, x, c, sdim ),  sf.grad(x, dof, c), ALE_TOL)
          << "degree: " << degree << "\npoint: " << i << "\ndof: " << dof << "\ncomp:" << c << "\nh:" << pow(ALE_EPS, 1./3.) <<  endl;
        }
      }
    }
  }



}






