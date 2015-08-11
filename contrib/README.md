generic_array
=============

A lightweight generic N-dimensional array library


This library is only a single header file (array.hpp) written
in C++. Although it is supposed to be generic, it supports
at most 10-dimensional arrays. However, there is c++11 version
which supports any rank.


Features:

- generic array dimension (at most 10 with c++03 standard);
- can be chosen row or col major order (by defining MA_DEFAULT_MAJOR or by template arguments, see below);
- there are wrappers for pre-existing datas;


This library has/is

- a very simple API;
- lightweight;
- tested
- fast (for the moment only one comparison to boost::multi_array is made in benchmark directory. I'll do more later)

Check `test.cpp` file to learn how to use it.

Some comments:

- You can change the numbering order defining the macro MA_DEFAULT_MAJOR:
  `\#define MA_DEFAULT_MAJOR RowMajor`
or 
  `\#define MA_DEFAULT_MAJOR ColMajor`
before the including the library.
- You can change the numbering order by setting the third Array template argument, for instance
  `marray::Array<double, 3, RowMajor>`


- For debug version (with range cheking, etc.) define the macro DEBUG before including the library;



