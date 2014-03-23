#ifndef PARAMETRIC_PTS_HPP
#define PARAMETRIC_PTS_HPP

#include <vector>
#include <map>
#include <tr1/tuple>
#include <cstdio>
#include <stdexcept>
#include <algorithm>


#ifdef DEBUG
#define PARAMETRIC_PTS_DEBUG
#endif

inline void genLineParametricPts(int n, std::vector<double> &);
inline void genLineParametricPtsINT(int n, std::vector<int> &);

inline void genTriParametricPts(int n, std::vector<double> &);
inline void genTriParametricPtsINT(int n, std::vector<int> &);

inline void genTetParametricPts(int n, std::vector<double> &);
inline void genTetParametricPtsINT(int n, std::vector<int> &);



/*
num :          0----------1 --> u      0-----2----1     0----2----3----1
               |          |
coord:        -1         +1

*/
/** Retorna um vetor com as coordenadas paramétricas dos pontos de uma
 *  reta de ordem n.
 */
inline void genLineParametricPts(int n, std::vector<double> & list)
{
  list.clear();
  
  if(n==0)
  {
    list.push_back(.0);
    return;
  }
  else
  {
    std::vector<int> temp;
    genLineParametricPtsINT(n,temp);
    
    list.resize(temp.size());
    
    for (unsigned i = 0; i < list.size(); ++i)
      list[i] = static_cast<double>(temp[i])/n;
    
  }
}

inline void genLineParametricPtsINT(int n, std::vector<int> & list)
{
  list.clear();

  if(n==0)
  {
    list.push_back(0);
    return;
  }
  if (n >= 1)
  {
    list.push_back(-n);
    list.push_back(+n);

    for (int i = 1; i <= n-1; i++)
      list.push_back(-n + i*2.);

    return;
  }
  return;
}



/*
     v
     ^
     |
     2 (0,1)
     |`\
     |  `\
     |    `\
     |      `\
     |        `\
     0----------1 --> u
   (0,0)       (1,0)

*/
/** Retorna um vetor com as coordenadas paramétricas dos pontos de um
 *  triângulo de ordem n.
 */
inline void genTriParametricPts(int n, std::vector<double> & list)
{
  list.clear();

  if (n<0)
  {
    throw std::invalid_argument("FEPIC ERROR: Invalid order");
  }

  if(n==0)
  {
    list.push_back(1./3.);
    list.push_back(1./3.);
    return;
  }
  else
  {
    /* intergers coordinates is used to reduce rounding erros */
    std::vector<int> temp;

    genTriParametricPtsINT(n, temp);

    for (int i = 0; i < (int) temp.size(); ++i)
      list.push_back( double(temp.at(i))/n );

    return;
  }


} // end genTriParametricPts

inline void genTriParametricPtsINT(int n, std::vector<int> & list) // list(2*i + j) = point i component j
{
  int a = 0;
  int b = n;

  if (n<0)
    throw std::invalid_argument("FEPIC ERROR: genTri...: Invalid order");

  list.clear();
  list.reserve((n+1)*(n+2)/2);

  while (n >= 0)
  {
    if (n==0)
    {
      list.push_back(a); list.push_back(a);
      return;
    }
    if(n>0)
    {
      list.push_back(a); list.push_back(a); // pt 0
      list.push_back(b); list.push_back(a); // pt 1
      list.push_back(a); list.push_back(b); // pt 2

      // aresta 0 (0-1)
      for (int i = 1; i <= n-1; ++i)
      {
        list.push_back(a+i);
        list.push_back(a);
      }

      // aresta 1 (1-2)
      for (int i = n-1; i > 0; --i)
      {
        list.push_back(a+i);
        list.push_back(b - i);
      }

      // aresta 2 (2-0)
      for (int i = 1; i <= n-1; ++i)
      {
        list.push_back(a);
        list.push_back(b - i);
      }

      // pontos interiores
      if (n>2)
      {
        a = a + 1;
        b = b - 2;
        n = n - 3;
      }
      else
      {
        return;
      }

    } // end if n>0

  } // end while

  return;
} // end genTriParametricPtsINT

/// INTERIOR points map
/// and triangle T1=(a,b,c) has anchor i to other triangle T2=(x,y,z)
/// if  rotate( reverse(T2), i ) == T1 is true
/// n order of the triangle
inline void mapTriInteriorPtsToOpp(int n, int anch, std::vector<int> &map)
{
  if (n<=0 || unsigned(anch)>2)
    throw std::invalid_argument("ALELIB ERROR: mapTriInteriorPtsToOpp invalid argument\n");
  
  n = n - 3; // order of the interior triangle
  
  map.clear();
  
  if (n<0) // no interior points
    return;
  
  map.resize((n+1)*(n+2)/2);
  for (int i = 0; i < (int)map.size(); ++i)
    map[i] = i;
  
  std::vector<int>::iterator it  = map.begin();
  std::vector<int>::iterator ite = it+3;
  
  while (n > 0)
  {
    // reordering vertices
    std::reverse(it, ite);
    std::rotate(it, it+anch, ite);
    
    // reordering edges
    int const nvwe = n-1; // n_vts_within_edge
    
    std::reverse(ite, ite + 3*nvwe);
    std::rotate(ite, ite + ((anch+1)%3)*nvwe, ite + 3*nvwe);
    
    it  = ite+3*nvwe;
    ite = it+3;
    n = n-3; // go to next interior triangle
  }  
  
}

/// connectivity(3*i+j) = j-th vertex of the cell i
inline void subCellsTri(int n, std::vector<int> &connectivity)
{
  std::vector<int> pts;
  genTriParametricPtsINT(n, pts); // pts(2*i + j) = point i component j

  connectivity.clear();
  connectivity.reserve(n*n);

  typedef std::pair<int, int> Coord;
  typedef int Id;

  std::map<Coord, Id> xy;
  
  for (int i = 0; i < (int)pts.size()/2; ++i)
    xy.insert(std::make_pair(Coord(pts[2*i], pts[2*i+1]), Id(i)));

  // mask
  for (int i = 0; i < n; ++i)
  {
    for (int j = 0; j < n-i-1; ++j)
    {
      // triangle a
      connectivity.push_back(xy[Coord(i,j)]);
      connectivity.push_back(xy[Coord(i+1,j)]);
      connectivity.push_back(xy[Coord(i,j+1)]);
      
      // triangle b
      connectivity.push_back(xy[Coord(i+1,j)]);
      connectivity.push_back(xy[Coord(i+1,j+1)]);
      connectivity.push_back(xy[Coord(i,j+1)]);      
      
    }
    // triangle a, when j=n-i-1
    connectivity.push_back(xy[Coord(i,n-i-1)]);
    connectivity.push_back(xy[Coord(i+1,n-i-1)]);
    connectivity.push_back(xy[Coord(i,n-i)]);
    
  }
  
}


inline void genTetParametricPts(int n, std::vector<double> & list)
{
  list.clear();

  if (n<0)
    throw std::invalid_argument("FEPIC ERROR: Invalid order");

  if(n==0)
  {
    list.push_back(1./4.);
    list.push_back(1./4.);
    list.push_back(1./4.);
    return;
  }
  else
  {
    /* intergers coordinates is used to reduce rounding erros */
    std::vector<int> temp;

    genTetParametricPtsINT(n, temp);

    for (int i = 0; i < (int) temp.size(); ++i)
      list.push_back( double(temp.at(i))/n );

    return;
  }


} // end genTriParametricPts

inline void genTetParametricPtsINT(int n, std::vector<int> & list)
{

  if (n<0)
    throw std::invalid_argument("FEPIC ERROR: Invalid order");

  int a = 0;
  int b = n;

  list.clear();
  list.reserve((n+1)*(n+2)*(n+3)/6);
  
  std::vector<int> face;

  while (n >= 0)
  {

    if(n==0)
    {
      list.push_back(a); list.push_back(a); list.push_back(a);
      return;
    }

    list.push_back(a); list.push_back(a); list.push_back(a);
    list.push_back(b); list.push_back(a); list.push_back(a);
    list.push_back(a); list.push_back(b); list.push_back(a);
    list.push_back(a); list.push_back(a); list.push_back(b);


    // aresta 0 (0-1)
    for (int i = 1; i <= n-1; ++i)
    {
      list.push_back(a+i); list.push_back(a); list.push_back(a);
    }

    // aresta 1 (1-2)
    for (int i = 1; i <= n-1; ++i)
    {
      list.push_back(b-i); list.push_back(a+i); list.push_back(a);
    }

    // aresta 2 (2-0)
    for (int i = 1; i <= n-1; ++i)
    {
      list.push_back(a); list.push_back(b-i); list.push_back(a);
    }

    // aresta 3 (3-0)
    for (int i = 1; i <= n-1; ++i)
    {
      list.push_back(a); list.push_back(a); list.push_back(b-i);
    }

    // aresta 4 (3-2)
    for (int i = 1; i <= n-1; ++i)
    {
      list.push_back(a); list.push_back(a+i); list.push_back(b-i);
    }

    // aresta 5 (3-1)
    for (int i = 1; i <= n-1; ++i)
    {
      list.push_back(a+i); list.push_back(a); list.push_back(b-i);
    }

    // faces
    if(n>2)
    {
      genTriParametricPtsINT(n-3, face);

      int vi[3], vj[3], vk[3]; // vértices da face reduzida {i,j,k}

      // face 0 {1,0,2}
      vi[0] = b-2; vi[1] = a+1; vi[2] = a;
      vj[0] = a+1; vj[1] = a+1; vj[2] = a;
      vk[0] = a+1; vk[1] = b-2; vk[2] = a;
      for (int i = 0; i < (int)face.size()/2; i++)
      {
        int N = n==3 ? 1 : (n-3);
        int x = face[2*i];
        int y = face[2*i+1];
        
        list.push_back( (x*(vj[0]-vi[0]) + y*(vk[0]-vi[0]))/N + vi[0] );
        list.push_back( (x*(vj[1]-vi[1]) + y*(vk[1]-vi[1]))/N + vi[1] );
        list.push_back( (x*(vj[2]-vi[2]) + y*(vk[2]-vi[2]))/N + vi[2] );
      }

      // face 1 {0,1,3}
      vi[0] = a+1; vi[1] = a; vi[2] = a+1;
      vj[0] = b-2; vj[1] = a; vj[2] = a+1;
      vk[0] = a+1; vk[1] = a; vk[2] = b-2;
      for (int i = 0; i < (int)face.size()/2; i++)
      {
        int N = n==3 ? 1 : (n-3);
        int x = face[2*i];
        int y = face[2*i+1];

        list.push_back( (x*(vj[0]-vi[0]) + y*(vk[0]-vi[0]))/N + vi[0] );
        list.push_back( (x*(vj[1]-vi[1]) + y*(vk[1]-vi[1]))/N + vi[1] );
        list.push_back( (x*(vj[2]-vi[2]) + y*(vk[2]-vi[2]))/N + vi[2] );
      }

      // face 2 {3,2,0}
      vi[0] = a; vi[1] = a+1; vi[2] = b-2;
      vj[0] = a; vj[1] = b-2; vj[2] = a+1;
      vk[0] = a; vk[1] = a+1; vk[2] = a+1;
      for (int i = 0; i < (int)face.size()/2; i++)
      {
        int N = n==3 ? 1 : (n-3);
        int x = face[2*i];
        int y = face[2*i+1];

        list.push_back( (x*(vj[0]-vi[0]) + y*(vk[0]-vi[0]))/N + vi[0] );
        list.push_back( (x*(vj[1]-vi[1]) + y*(vk[1]-vi[1]))/N + vi[1] );
        list.push_back( (x*(vj[2]-vi[2]) + y*(vk[2]-vi[2]))/N + vi[2] );
      }

      // face 3 {2,3,1}
      vi[0] = a+1; vi[1] = b-2; vi[2] = a+1;
      vj[0] = a+1; vj[1] = a+1; vj[2] = b-2;
      vk[0] = b-2; vk[1] = a+1; vk[2] = a+1;
      for (int i = 0; i < (int)face.size()/2; i++)
      {
        int N = n==3 ? 1 : (n-3);
        int x = face[2*i];
        int y = face[2*i+1];

        list.push_back( (x*(vj[0]-vi[0]) + y*(vk[0]-vi[0]))/N + vi[0] );
        list.push_back( (x*(vj[1]-vi[1]) + y*(vk[1]-vi[1]))/N + vi[1] );
        list.push_back( (x*(vj[2]-vi[2]) + y*(vk[2]-vi[2]))/N + vi[2] );
      }
    }

    // pontos interiores
    if (n>3)
    {
      a = a + 1;
      b = b - 3;
      n = n - 4;
    }
    else
    {
      return;
    }


  } // end `while n>= 0`

  return;
}


/// connectivity(4*i+j) = j-th vertex of the cell i
inline void subCellsTet(int n, std::vector<int> &connectivity)
{
  std::vector<int> pts;
  genTetParametricPtsINT(n, pts); // pts(2*i + j) = point i component j

  connectivity.clear();
  connectivity.reserve(n*n);

  typedef std::tr1::tuple<int, int, int> Coord;
  typedef int Id;

  std::map<Coord, Id> xyz;
  
  for (int i = 0; i < (int)pts.size()/3; ++i)
    xyz.insert(std::make_pair(Coord(pts[3*i], pts[3*i+1], pts[3*i+2]), Id(i)));

  // mask
  int i=0,j=0,k=0;
  for (i = 0; i < n; ++i)
  {
    for (j = 0; j < n-i; ++j)
    {
      for (k = 0; k < n-j-i; ++k)
      {

        connectivity.push_back(xyz[Coord(i,j,k)]);
        connectivity.push_back(xyz[Coord(i+1,j,k)]);
        connectivity.push_back(xyz[Coord(i,j+1,k)]);
        connectivity.push_back(xyz[Coord(i,j,k+1)]);
        
        if ((j==n-i-1) || (k==n-j-i-1))
          continue;
        
        // tet b
        connectivity.push_back(xyz[Coord(i+1,j+1,k)]);
        connectivity.push_back(xyz[Coord(i,j+1,k+1)]);
        connectivity.push_back(xyz[Coord(i,j,k+1)]);
        connectivity.push_back(xyz[Coord(i+1,j,k+1)]);
        
        // tet c
        connectivity.push_back(xyz[Coord(i+1,j+1,k)]);
        connectivity.push_back(xyz[Coord(i,j,k+1)]);
        connectivity.push_back(xyz[Coord(i+1,j,k)]);
        connectivity.push_back(xyz[Coord(i+1,j,k+1)]);
        
        // tet d
        connectivity.push_back(xyz[Coord(i,j,k+1)]);
        connectivity.push_back(xyz[Coord(i,j+1,k+1)]);
        connectivity.push_back(xyz[Coord(i+1,j+1,k)]);
        connectivity.push_back(xyz[Coord(i,j+1,k)]);
        
        // tet e
        connectivity.push_back(xyz[Coord(i+1,j,k)]);
        connectivity.push_back(xyz[Coord(i,j,k+1)]);
        connectivity.push_back(xyz[Coord(i+1,j+1,k)]);
        connectivity.push_back(xyz[Coord(i,j+1,k)]);
        
        if (n - (i+j+k+3) >= 0)
        {
          // tet i
          connectivity.push_back(xyz[Coord(i+1,j,k+1)]);
          connectivity.push_back(xyz[Coord(i+1,j+1,k)]);
          connectivity.push_back(xyz[Coord(i,j+1,k+1)]);
          connectivity.push_back(xyz[Coord(i+1,j+1,k+1)]);
        }
        
      } // end k
    } // end j
  } // end i
  
}



#ifdef PARAMETRIC_PTS_DEBUG

#include <iostream>
#include <fstream>

  inline
  void print_line_geo(std::vector<double> const& pts, const char* file_name)
  {
    std::ofstream out;

    out.open(file_name);

    for (int i = 0; i < (int) pts.size(); ++i)
    {
      out << "Point("<<i+1<<")={"<<pts.at(i)<<", 0, 0, 1.0};\n";
    }

    out.close();
  }

  inline
  void print_tri_geo(std::vector<double> const& pts, const char* file_name)
  {
    std::ofstream out;

    out.open(file_name);

    for (int i = 0; i < (int) pts.size()/2; ++i)
    {
      out << "Point("<<i+1<<")={"<<pts.at(2*i)<<", "<<pts.at(2*i+1)<<", 0, 1.0};\n";
    }

    out.close();
  }

  inline
  void print_tet_geo(std::vector<double> const& pts, const char* file_name)
  {
    std::ofstream out;

    out.open(file_name);

    for (int i = 0; i < (int) pts.size()/3; ++i)
    {
      out << "Point("<<i+1<<")={"<<pts.at(3*i)<<", "<<pts.at(3*i+1)<<", "<<pts.at(3*i+2)<<", 1.0};\n";
    }

    out.close();
  }

  inline
  void print_tri_subcell(std::vector<double> const& pts, std::vector<int> const& cells, const char* file_name)
  {
    std::ofstream out;
    out.open(file_name);    

    out << "# vtk DataFile Version 2.0\n";
    out << "Unstructured Grid Example\n";
    out << "ASCII\n";
    out << "DATASET UNSTRUCTURED_GRID\n";
    out << "POINTS " << pts.size()/2 << " double\n";
    for (int i = 0; i < (int)pts.size()/2; ++i)
      out << pts[2*i] << " " << pts[2*i+1] << " 0" << std::endl;
    
    out << "\nCELLS " << cells.size()/3 << " " << 4*cells.size()/3 << std::endl;
    for (int i = 0; i < (int)cells.size()/3; ++i)
      out << "3 " << cells[3*i] << " " << cells[3*i+1] << " " << cells[3*i+2] << std::endl;
    
    out << "\nCELL_TYPES " << cells.size()/3 << std::endl;
    for (int i = 0; i < (int)cells.size()/3; ++i)
      out << "5\n";
    
    out.close();
  }

  inline
  void print_tet_subcell(std::vector<double> const& pts, std::vector<int> const& cells, const char* file_name)
  {
    std::ofstream out;
    out.open(file_name);    

    int const Npts = (int)pts.size()/3;
    int const Ncs  = (int)cells.size()/4;

    out << "# vtk DataFile Version 2.0\n";
    out << "Unstructured Grid Example\n";
    out << "ASCII\n";
    out << "DATASET UNSTRUCTURED_GRID\n";
    out << "POINTS " << Npts<< " double\n";
    for (int i = 0; i < Npts; ++i)
      out << pts[3*i] << " " << pts[3*i+1] << " " << pts[3*i+2] << std::endl;
    
    out << "\nCELLS " << Ncs << " " << 5*Ncs << std::endl;
    for (int i = 0; i < Ncs; ++i)
      out << "4 " << cells[4*i] << " " << cells[4*i+1] << " " << cells[4*i+2] << " " << cells[4*i+3] << std::endl;
    
    out << "\nCELL_TYPES " << Ncs << std::endl;
    for (int i = 0; i < Ncs; ++i)
      out << "10\n";
    
    out.close();
  }


#endif


#endif



