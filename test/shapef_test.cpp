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
#include <Alelib/Quadrature>
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

  if (0) // two points
  {
    Real h = 0.41*pow(ALE_EPS, 1./3.); // for 2-points method
    volatile Real t = h + x[c];
    h = t - x[c];

    Real y[Dim];
    Real z[Dim];

    for (int i = 0; i < Dim; ++i)
    {
      y[i] = x[i];
      z[i] = x[i];
    }

    y[c]  += h;
    z[c]  -= h;

    return (f(y)-f(z))/(2.*h);
  }
  else // four points
  {

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

    return (-f(yy) + 8.*(f(y) - f(z)) + f(zz))/(12.*h); // for higher order
  }
}

// calculates the hessian a function f(x)
// f: callable object in form  "Real f(Real const*)"
template<class F>
Real diff2_(F const& f, Real const*x, int c, int d, int Dim)
{

  if (0) // four points
  {
    Real hx = 83.530*pow(ALE_EPS, 1./3.), hy=hx; // for 2-points method
    volatile Real t = hx + x[c];
    volatile Real u = hy + x[d];
    hx = t - x[c];
    hy = u - x[d];

    Real xpp[Dim]; // p = plus
    Real xmm[Dim]; // m = minus
    Real xpm[Dim]; //
    Real xmp[Dim]; //

    for (int i = 0; i < Dim; ++i)
    {
      xpp[i] = x[i];
      xmm[i] = x[i];
      xpm[i] = x[i];
      xmp[i] = x[i];
    }

    xpp[c] += hx;
    xpp[d] += hy;
    
    xmm[c] -= hx;
    xmm[d] -= hy;
        
    xpm[c] += hx; 
    xpm[d] -= hy; 
    
    xmp[c] -= hx;
    xmp[d] += hy;

    return (f(xpp) + f(xmm) - f(xpm) - f(xmp))/(4.*hx*hy);
  }
  else
  {
    Real h = 3*pow(ALE_EPS, 1./3.); // for 2-points method
    volatile Real t = h + x[c];
    h = t - x[c];

    Real y[Dim];
    Real z[Dim];

    for (int i = 0; i < Dim; ++i)
    {
      y[i] = x[i];
      z[i] = x[i];
    }

    y[c]  += h;
    z[c]  -= h;

    return (diff_(f, y, d, Dim)-diff_(f, z, d, Dim))/(2.*h);
  }
}


TEST(ShapeEdge1Tests, LagrangeDeltaProperty)
{
  //Real x[3] = {0.0,0.0,0.0};

  std::vector<double> xyz;
  ShapeFunction sf;

  for (int k = 1; k < 10; ++k)
  {
    sf.setType("Lagrange", /*dim*/1, /*degree*/k);
    genLineParametricPts(k, xyz);

    ASSERT_EQ((int)xyz.size(), sf.numDofs());

    for (int i = 0; i < (int)xyz.size(); ++i)
    {
      for (int j = 0; j < (int)xyz.size(); ++j)
      {
        if (i==j)
          ASSERT_NEAR(1, sf.value(listOf(xyz[j]), j), ALE_TOL) << "i=" << i << "\nj=" << j << "\nxyz[i]=" << xyz[i];
        else
          ASSERT_NEAR(0, sf.value(listOf(xyz[i]), j), ALE_TOL) << "i=" << i << "\nj=" << j << "\nxyz[i]=" << xyz[i];
        //cout << sf.value(&xyz[i], j) << " pp:" << xyz[i] <<endl;
      }
      //cout << endl;
    }
  }

  sf.setType("Lagrange", /*dim*/1, /*degree*/0);
  xyz[0] = 0.;
  ASSERT_EQ(1, sf.value(listOf(xyz[0]), 0));

}

TEST(ShapeEdge1Tests, LagrangeGradient)
{

  std::vector<double> xyz;
  ShapeFunction sf;
  int const sdim = 1;

  // Shape function placeholder
  std::tr1::function<Real (Real const*)> Func;

  for (int degree = 1; degree < 8; ++degree)
  {
    sf.setType("Lagrange", /*dim*/1, degree);
    genLineParametricPts(degree, xyz);

    for (int dof = 0; dof < (int)xyz.size(); ++dof)
    {
      for (int i = 0; i < (int)xyz.size(); ++i)
      {
        Real x[] = {xyz[i]};

        Func = std::tr1::bind(std::tr1::mem_fn(&ShapeFunction::value), sf, _1, dof);

        ASSERT_NEAR(diff_( Func, x, 0, sdim ),  sf.grad(x, dof, 0), ALE_TOL)
        << "degree: " << degree << "\npoint: " << i << "\ndof: " << dof << "\ncomp:" << 0 << "\npt:" << x[0] <<  endl;

        //cout << diff_( Func, x, 0, sdim ) << " ppp:" << x[0] << endl;
        //cout << sf.grad(x, dof, 0) << "\tppp:" << x[0] << endl;
      }
    }
    //for (int k = 0; k < xyz.size(); ++k)
    //  cout << xyz[k] << " ";
    //cout << endl;
  }



}

TEST(ShapeEdge1Tests, LagrangeHessian)
{

  std::vector<double> xyz;
  ShapeFunction sf;
  int const sdim = 1;

  // Shape function placeholder
  std::tr1::function<Real (Real const*)> Func;

  for (int degree = 1; degree < 8; ++degree)
  {
    sf.setType("Lagrange", /*dim*/1, degree);
    genLineParametricPts(degree, xyz);

    for (int dof = 0; dof < (int)xyz.size(); ++dof)
    {
      for (int i = 0; i < (int)xyz.size(); ++i)
      {
        Real x[] = {xyz[i]};

        Func = std::tr1::bind(std::tr1::mem_fn(&ShapeFunction::value), sf, _1, dof);

        ASSERT_NEAR(diff2_( Func, x, 0, 0, sdim ),  sf.hessian(x, dof, 0, 0), 3.e-6)
        << "degree: " << degree << "\npoint: " << i << "\ndof: " << dof << "\ncomp:" << 0 << "\npt:" << x[0] <<  endl;

        //cout << diff_( Func, x, 0, sdim ) << " ppp:" << x[0] << endl;
        //cout << sf.grad(x, dof, 0) << "\tppp:" << x[0] << endl;
      }
    }
    //for (int k = 0; k < xyz.size(); ++k)
    //  cout << xyz[k] << " ";
    //cout << endl;
  }



}

TEST(ShapeEdge1Tests, BubbleVGH) // value, gradient and hessian
{
  std::vector<double> xyz;
  ShapeFunction sf;

  // Shape function placeholder
  std::tr1::function<Real (Real const*)> Func;

  sf.setType("Bubble", /*dim*/1);

  Func = std::tr1::bind(std::tr1::mem_fn(&ShapeFunction::value), sf, _1, 0);
  
  Real x = 0;
  
  ASSERT_EQ(1, sf.numDofs());
  
  ASSERT_NEAR(1.0,  sf.value(&x, /*dof*/0), 1e-14);
  x = -1;
  ASSERT_NEAR(0.0,  sf.value(&x, /*dof*/0), 1e-14);
  x = +1;
  ASSERT_NEAR(0.0,  sf.value(&x, /*dof*/0), 1e-14);
  x = 0.5;
  ASSERT_NEAR(3./4,  sf.value(&x, /*dof*/0), 1e-14);
  x = -0.5;
  ASSERT_NEAR(3./4,  sf.value(&x, /*dof*/0), 1e-14);
  
  Real xx[] = {-1.,-.5,0.,.5,1.};
  
  for (unsigned i = 0; i < sizeof(xx)/sizeof(Real); ++i)
  {
    ASSERT_NEAR(diff_( Func, xx+i, 0, /*sdim*/1 ),  sf.grad(xx+i, /*dof*/0, /*c*/0), 1e-12) << "X=" << xx[i] << endl;
    ASSERT_NEAR(diff2_( Func, xx+i, 0, 0, /*sdim*/1 ),  sf.hessian(xx+i, /*dof*/0, /*c*/0, /*d*/0), 2e-8) << "X=" << xx[i] << endl;
  }
  
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
          ASSERT_NEAR(1, sf.value(listOf(xyz[2*j], xyz[2*j+1]), j), ALE_TOL);
        else
          ASSERT_NEAR(0, sf.value(listOf(xyz[2*i], xyz[2*i+1]), j), ALE_TOL);
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

TEST_F(ShapeTri1Tests, LagrangeHessian)
{

  std::vector<double> xyz;
  ShapeFunction sf;
  int const sdim = 2;

  // Shape function placeholder
  std::tr1::function<Real (Real const*)> Func;

  for (int degree = 1; degree < 6; ++degree)
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
          for (int d = 0; d < sdim; ++d)
            ASSERT_NEAR(diff2_( Func, x, c, d, sdim ),  sf.hessian(x, dof, c, d), 2e-6)
            << "degree: " << degree << "\npoint: " << i << "\ndof: " << dof << "\ncomp:" << c << "\nh:" << pow(ALE_EPS, 1./3.) <<  endl;
        }
      }
    }
  }



}

TEST_F(ShapeTri1Tests, BubbleVGH)
{

  std::vector<double> xyz;
  ShapeFunction sf;
  int const sdim = 2;

  // Shape function placeholder
  std::tr1::function<Real (Real const*)> Func;

  sf.setType("Bubble", /*dim*/2);
  
  genTriParametricPts(3, xyz);

  for (int i = 0; i < (int)xyz.size()/2; ++i)
  {
    Real x[] = {xyz[2*i], xyz[2*i+1]};

    int dof = 0;

    Func = std::tr1::bind(std::tr1::mem_fn(&ShapeFunction::value), sf, _1, dof);

    for (int c = 0; c < sdim; ++c)
    {
      ASSERT_NEAR(diff_( Func, x, c, sdim ),  sf.grad(x, dof, c), ALE_TOL);
      for (int d = 0; d < sdim; ++d)
        ASSERT_NEAR(diff2_( Func, x, c, d, sdim ),  sf.hessian(x, dof, c, d), 4e-8);
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

  for (int k = 1; k < 9; ++k)
  {
    sf.setType("Lagrange", /*dim*/sdim, /*degree*/k);
    genTetParametricPts(k, xyz);

    ASSERT_EQ((int)xyz.size()/sdim, sf.numDofs());

    for (int i = 0; i < (int)xyz.size()/sdim; ++i)
    {
      for (int j = 0; j < (int)xyz.size()/sdim; ++j)
      {
        if (i==j)
          ASSERT_NEAR(1, sf.value(listOf(xyz[3*j], xyz[3*j+1], xyz[3*j+2]), j), ALE_TOL);
        else
          ASSERT_NEAR(0, sf.value(listOf(xyz[3*i], xyz[3*i+1], xyz[3*i+2]), j), ALE_TOL)
          << "degree= "<< k <<  "\ni=" << i << "\nj=" << j << "\nxyz=" << xyz[3*i]<<" "<<xyz[3*i+1]<<" "<<xyz[3*i+2];
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

TEST_F(ShapeTet1Tests, LagrangeHessian)
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
          for (int d = 0; d < sdim; ++d)
            ASSERT_NEAR(diff2_( Func, x, c, d, sdim ),  sf.hessian(x, dof, c, d), 2e-6)
            << "degree: " << degree << "\npoint: " << i << "\ndof: " << dof << "\ncomp:" << c << "\nh:" << pow(ALE_EPS, 1./3.) <<  endl;
        }
      }
    }
  }


}

TEST_F(ShapeTet1Tests, BubbleVGH)
{

  std::vector<double> xyz;
  ShapeFunction sf;
  int const sdim = 3;

  // Shape function placeholder
  std::tr1::function<Real (Real const*)> Func;

  sf.setType("Bubble", /*dim*/3);
  genTetParametricPts(4, xyz);

  for (int i = 0; i < (int)xyz.size()/3; ++i)
  {
    Real x[] = {xyz[3*i], xyz[3*i+1], xyz[3*i+2]};

    int dof = 0;

    Func = std::tr1::bind(std::tr1::mem_fn(&ShapeFunction::value), sf, _1, dof);

    for (int c = 0; c < sdim; ++c)
    {
      ASSERT_NEAR(diff_( Func, x, c, sdim ),  sf.grad(x, dof, c), ALE_TOL);
      for (int d = 0; d < sdim; ++d)
        ASSERT_NEAR(diff2_( Func, x, c, d, sdim ),  sf.hessian(x, dof, c, d), 4e-8);
    }
  }


}

TEST_F(ShapeTet1Tests, LagrangeBubble)
{

  std::vector<double> xyz;
  ShapeFunction sf;
  //int const sdim = 3;

  // Shape function placeholder
  std::tr1::function<Real (Real const*)> Func;

  sf.setType("Lagrange+Bubble", /*dim*/3, 1);
  genTetParametricPts(4, xyz);

  ASSERT_EQ(5, sf.numDofs());

  for (int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < sf.numDofs()-1; ++j)
    {
      if (i==j)
        ASSERT_NEAR(1, sf.value(listOf(xyz[3*j], xyz[3*j+1], xyz[3*j+2]), j), ALE_TOL);
      else
        ASSERT_NEAR(0, sf.value(listOf(xyz[3*i], xyz[3*i+1], xyz[3*i+2]), j), ALE_TOL);
    }
  }

  ASSERT_NEAR(1., sf.value(&xyz.back()-2, 4), 1e-14); // xyz.back()-2 is the mid point .. bubble value 1 there


}


//
//
// Testing the quadrature points
//
//

TEST(ShapeEdge1Tests, LagrangeIntegration)
{
  Quadrature Q;
  ShapeFunction sf;

  for (int deg = 0; deg < 10; ++deg)
  {
    Q.setType(EDGE,deg);
    sf.setType("Lagrange", /*dim*/1, /*degree*/deg);

    Real integ = 0;
    Real d_integ = 0;
    for (int qp = 0; qp < Q.numPoints(); ++qp)
    {
      for (int dof = 0; dof < sf.numDofs(); ++dof)
      {
        integ += sf.value(Q.point(qp), dof) * Q.weight(qp);
        d_integ += sf.grad(Q.point(qp), dof, 0) * Q.weight(qp);
      }

    }
    ASSERT_NEAR(2., integ, ALE_TOL);
    ASSERT_NEAR(0., d_integ, ALE_TOL);
  }

}


TEST_F(ShapeTri1Tests, LagrangeIntegration)
{
  Quadrature Q;
  ShapeFunction sf;

  for (int deg = 0; deg < 11; ++deg)
  {
    Q.setType(TRIANGLE,deg);
    sf.setType("Lagrange", /*dim*/2, /*degree*/deg);

    Real integ = 0;
    Real d_integ[2] = {0,0};
    for (int qp = 0; qp < Q.numPoints(); ++qp)
    {
      for (int dof = 0; dof < sf.numDofs(); ++dof)
      {
        integ += sf.value(Q.point(qp), dof) * Q.weight(qp);
        for (int i = 0; i < 2; ++i)
          d_integ[i] += sf.grad(Q.point(qp), dof, i) * Q.weight(qp);
      }

    }
    ASSERT_NEAR(0.5, integ, ALE_TOL);
    ASSERT_NEAR(0.0, d_integ[0], ALE_TOL);
    ASSERT_NEAR(0.0, d_integ[1], ALE_TOL);

  }

}

TEST_F(ShapeTet1Tests, LagrangeIntegration)
{
  Quadrature Q;
  ShapeFunction sf;

  for (int deg = 0; deg < 9; ++deg)
  {
    Q.setType(TETRAHEDRON,deg);
    sf.setType("Lagrange", /*dim*/3, /*degree*/deg);

    Real integ = 0;
    Real d_integ[3] = {0,0,0};
    for (int qp = 0; qp < Q.numPoints(); ++qp)
    {
      for (int dof = 0; dof < sf.numDofs(); ++dof)
      {
        integ += sf.value(Q.point(qp), dof) * Q.weight(qp);
        for (int i = 0; i < 3; ++i)
          d_integ[i] += sf.grad(Q.point(qp), dof, i) * Q.weight(qp);
      }

    }
    ASSERT_NEAR(1./6., integ, ALE_TOL);
    ASSERT_NEAR(0.0, d_integ[0], ALE_TOL);
    ASSERT_NEAR(0.0, d_integ[1], ALE_TOL);
    ASSERT_NEAR(0.0, d_integ[2], ALE_TOL);
  }

}


