#ifndef ALELIB_CELL_HPP
#define ALELIB_CELL_HPP

#include <algorithm>    // std::fill
#include "enums.hpp"
#include "labelable.hpp"


namespace alelib
{

// dummy
template<ECellType CT>
class Cell;


#define ALE_CELL_CONSTRUCTOR                                                                                       \
      public:                                                                                                      \
      inline Cell(int8_t tag, Flags flags) : Labelable(tag,flags)                                                  \
      {                                                                                                            \
        reset();                                                                                                   \
      }                                                                                                            \
                                                                                                                   \
      inline Cell() : Labelable() {}                                                                               \
      private:                                                                                                     \
                                                                                                                   \
      inline void reset()                                                                                          \
      {                                                                                                            \
        resetVertices()    ;                                                                                       \
        resetRidges()      ;                                                                                       \
        resetFacets()      ;                                                                                       \
      }                                                                                                                                     \
                                                                                                                                            \
      inline void resetVertices()                                                                                                           \
      {                               std::fill(verts,           verts          +sizeof(verts          )/sizeof(index_t),  NULL_IDX); }     \
      inline void resetRidges()                                                                                                             \
      { if(dim>1)                     std::fill(ridges,          ridges         +sizeof(ridges         )/sizeof(index_t),  NULL_IDX); }     \
      inline void resetFacets()                                                                                                             \
      {                               std::fill(facets,          facets         +sizeof(facets         )/sizeof(index_t),  NULL_IDX); }
      





// 1D Cells members
#define ALE_DEF_1D_CELLS_MEMBERS                                 \
                                                                 \
  union                                                          \
  {                                                              \
    index_t verts[n_verts]; /* verts id; N=order */              \
    index_t facets[n_verts]; /* alias to verts */                \
    index_t ridges[n_verts]; /* dummy */                         \
  };

// 2D Cells members
#define ALE_DEF_2D_CELLS_MEMBERS                                 \
                                                                 \
  index_t facets[n_facets];  /* facets id  */                    \
  union                                                          \
  {                                                              \
    index_t verts[n_verts];   /* verts id */                     \
    index_t ridges[n_verts]; /* alias for verts */               \
  };


// 3D Cells members
#define ALE_DEF_3D_CELLS_MEMBERS                                 \
  index_t  facets[n_facets];   /* facets id */                   \
  index_t  ridges[n_ridges];  /* edges id */                     \
  index_t  verts[n_verts]; /* verts id */






template<>
class Cell<POINT>  : private Labelable
{
  template<class,class> friend class SeqList;
  template<typename> friend class Mesh;

  static const unsigned dim                = 0;
  static const unsigned n_verts            = 0;
  static const unsigned n_verts_p_facet    = 0;
  static const unsigned n_verts_p_ridge    = 0;
  static const unsigned n_facets           = 0;
  static const unsigned n_ridges           = 0;
  static const unsigned n_ridges_p_facet   = 0;

};

template<>
class Cell<EDGE>  : private Labelable
{
  template<class,class> friend class SeqList;
  template<typename> friend class Mesh;
  
  static const unsigned dim                = 1;
  static const unsigned n_verts            = 2;
  static const unsigned n_verts_p_facet    = 1;
  static const unsigned n_verts_p_ridge    = 0;
  static const unsigned n_facets           = 2;
  static const unsigned n_ridges           = 0;
  static const unsigned n_ridges_p_facet   = 0;

  ALE_DEF_1D_CELLS_MEMBERS

  ALE_CELL_CONSTRUCTOR

};

template<>
class Cell<TRIANGLE>  : private Labelable
{
  template<class,class> friend class SeqList;
  template<typename> friend class Mesh;

  static const unsigned dim                = 2;
  static const unsigned n_verts            = 3;
  static const unsigned n_verts_p_facet    = 2;
  static const unsigned n_verts_p_ridge    = 1;
  static const unsigned n_facets           = 3;
  static const unsigned n_ridges           = 0;
  static const unsigned n_ridges_p_facet   = 0;

  ALE_DEF_2D_CELLS_MEMBERS

  ALE_CELL_CONSTRUCTOR

};

template<>
class Cell<QUADRANGLE>  : private Labelable
{
  template<class,class> friend class SeqList;
  template<typename> friend class Mesh;

  static const unsigned dim                = 2;
  static const unsigned n_verts            = 4;
  static const unsigned n_verts_p_facet    = 2;
  static const unsigned n_verts_p_ridge    = 1;
  static const unsigned n_facets           = 4;
  static const unsigned n_ridges           = 0;
  static const unsigned n_ridges_p_facet   = 0;

  ALE_DEF_2D_CELLS_MEMBERS

  ALE_CELL_CONSTRUCTOR

};

template<>
class Cell<TETRAHEDRON>  : private Labelable
{
  template<class,class> friend class SeqList;
  template<typename> friend class Mesh;

  static const unsigned dim                = 3;
  static const unsigned n_verts            = 4;
  static const unsigned n_verts_p_facet    = 3;
  static const unsigned n_verts_p_ridge    = 2;
  static const unsigned n_facets           = 4;
  static const unsigned n_ridges           = 6;
  static const unsigned n_ridges_p_facet   = 3;

  ALE_DEF_3D_CELLS_MEMBERS

  ALE_CELL_CONSTRUCTOR

};

template<>
class Cell<HEXAHEDRON>  : private Labelable
{
  template<class,class> friend class SeqList;
  template<typename> friend class Mesh;

  static const unsigned dim                = 3 ;
  static const unsigned n_verts            = 8 ;
  static const unsigned n_verts_p_facet    = 4 ;
  static const unsigned n_verts_p_ridge    = 2 ;
  static const unsigned n_facets           = 6 ;
  static const unsigned n_ridges           = 12;
  static const unsigned n_ridges_p_facet   = 4 ;

  ALE_DEF_3D_CELLS_MEMBERS

  ALE_CELL_CONSTRUCTOR

};



#undef ALE_CELL_CONSTRUCTOR
#undef ALE_DEF_1D_CELLS_MEMBERS
#undef ALE_DEF_2D_CELLS_MEMBERS
#undef ALE_DEF_3D_CELLS_MEMBERS

//class internalDummyclass
//{
//  //index_t  adj_cells[1];
//  index_t  facets[1];
//  index_t  ridges[1];
//  index_t  verts[1];
//};

} // end namespace alelib


#endif






