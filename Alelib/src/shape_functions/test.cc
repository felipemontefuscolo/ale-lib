#define DEBUG

#include <iostream>
#include "parametric_pts.hpp"
#include <cassert>
#include <limits>
#include <cmath>

using namespace std;

//#define MAX(a,b) ((a) > (b) ? (a) : (b))

typedef double Real;


double const FEP_EPS = std::numeric_limits<Real>::epsilon();
double const FEP_TOL = 500000*FEP_EPS; // ~ 1.1e-10 para double


// calculates the derivative a function f(x)
// f: callable object in form  "Real f(Real const*)"
template<class F>
Real diff_(F const& f, Real const*x, int c, int Dim);


std::vector<double> Line(int n)
{
  std::vector<double> list;
  genLineParametricPts(n, list);
  return list;
}

std::vector<double> Tri(int n)
{
  std::vector<double> list;
  genTriParametricPts(n, list);
  return list;
}

std::vector<double> Tet(int n)
{
  std::vector<double> list;
  genTetParametricPts(n, list);
  return list;
}

std::vector<double> TriReordered(int n, int anch)
{
  std::vector<double> list;
  std::vector<int> map;
  genTriParametricPts(n, list);
  mapTriInteriorPtsToOpp(n, anch, map);
  std::vector<double> list2(list);  
  
  for	(int i=0; i<(int)map.size(); ++i)
  	map[i] += 3*n;
  
  for (int I=3*n; I<(int)list.size()/2; ++I)
  {
  	int const i = I-3*n;
  	list2[2*I+0] = list[2*map[i]+0];
  	list2[2*I+1] = list[2*map[i]+1];
  }
  
  return list2;
}

inline
std::vector<int> TriSubcell(int n)
{
	std::vector<int> connectivity;
	
	subCellsTri(n, connectivity);
	
	return connectivity;
	
}

inline
std::vector<int> TetSubcell(int n)
{
	std::vector<int> connectivity;
	
	subCellsTet(n, connectivity);
	
	return connectivity;
	
}

void test_parametric_pts(bool print_geo)
{
  assert(Line(0).size()==1);
  assert(Line(1).size()==2);
  assert(Line(2).size()==3);
  assert(Line(3).size()==4);
  assert(Line(10).size()==11);

  assert(Tri(0).size() == 2*1);
  assert(Tri(1).size() == 2*3);
  assert(Tri(2).size() == 2*6);
  assert(Tri(3).size() == 2*10);
  assert(Tri(10).size() == 2*11*12/2);
  
  assert(Tet( 0).size() == 3*1);  
  assert(Tet( 1).size() == 3*4);  
  assert(Tet( 2).size() == 3*10);  
  assert(Tet( 3).size() == 3*20);
  assert(Tet( 4).size() == 3*35);  
  assert(Tet( 9).size() == 3*10*11*12/6);

	assert(TetSubcell(4).size() == 4* 4*4*4);
	assert(TetSubcell(9).size() == 4* 9*9*9);

  if (print_geo)
  {
    print_line_geo(Line( 0), "line0.geo");
    print_line_geo(Line( 1), "line1.geo");
    print_line_geo(Line( 2), "line2.geo");
    print_line_geo(Line( 3), "line3.geo");
    print_line_geo(Line(10), "line10.geo");    
    
    print_tri_geo(Tri( 0), "tri0.geo");
    print_tri_geo(Tri( 1), "tri1.geo");
    print_tri_geo(Tri( 2), "tri2.geo");
    print_tri_geo(Tri( 3), "tri3.geo");
    print_tri_geo(Tri(10), "tri10.geo");
    
    print_tet_geo(Tet( 0), "tet0.geo");
    print_tet_geo(Tet( 1), "tet1.geo");
    print_tet_geo(Tet( 2), "tet2.geo");
    print_tet_geo(Tet( 3), "tet3.geo");
    print_tet_geo(Tet( 4), "tet4.geo");
    print_tet_geo(Tet( 9), "tet9.geo");

    print_tri_geo(TriReordered( 3, 0), "triRe03A0.geo");    
    print_tri_geo(TriReordered(10, 0), "triRe10A0.geo");
    print_tri_geo(TriReordered(10, 1), "triRe10A1.geo");
    print_tri_geo(TriReordered(10, 2), "triRe10A2.geo");

		print_tri_subcell(Tri(2), TriSubcell(2), "trisubmesh.vtk");
    print_tet_subcell(Tet(4), TetSubcell(4), "tetsubmesh.vtk");

  }
  
  std::cout << "Everything seems ok\n";
}

int main(int , char *[])
{

  test_parametric_pts(false);
}



#if 0
template<class F>
Real diff_(F const& f, Real const*x, int c, int Dim)
{
	Real h = MAX(fabs(x[c]), 1.)*pow(FEP_EPS, 1./3.);
	volatile Real t = h + x[c];
	h = t - x[c];

	Real y[Dim];
	Real z[Dim];
	//Real zz[Dim] = {x[0], x[1]}; // high order
	//Real yy[Dim] = {x[0], x[1]};	

  for (int i = 0; i < Dim; ++i)
  {
    y[i] = x[i];
    z[i] = x[i];
    //zz[Dim] = x[i];
    //yy[Dim] = x[i];
  }
  
	y[c]  += h;
	z[c]  -= h;
	//yy[c] += 2.*h;
	//zz[c] -= 2.*h;

  return (f(y)-f(z))/(2.*h);
	//return (-f(yy) + 8.*(f(y) - f(z)) + f(zz))/(12.*h); // for higher order
}
#endif





