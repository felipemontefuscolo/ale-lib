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

#ifndef ALELIB_ALGORITHM_HPP
#define ALELIB_ALGORITHM_HPP


namespace alelib {




// return only the first intersection
template <class InputIterator1, class InputIterator2, class OutputIterator>
  OutputIterator set_1_intersection (InputIterator1 first1, InputIterator1 last1,
                                     InputIterator2 first2, InputIterator2 last2,
                                     OutputIterator result)
{
  while (first1!=last1 && first2!=last2)
  {
    if (*first1<*first2) ++first1;
    else if (*first2<*first1) ++first2;
    else {
      *result = *first1;
      ++result;
      return result;
    }
  }
  return result;
}



// return only the first intersection
template <class InputIterator1, class InputIterator2, class InputIterator3, class OutputIterator>
  OutputIterator set_1_intersection (InputIterator1 first1, InputIterator1 last1,
                                     InputIterator2 first2, InputIterator2 last2,
                                     InputIterator3 first3, InputIterator3 last3,
                                     OutputIterator result)
{
  while (first1!=last1 && first2!=last2 && first3!=last3)
  {
    if      (*first1<*first2) ++first1;
    else if (*first2<*first1) ++first2;
    else if (*first3<*first1) ++first3;
    else if (*first1<*first3) {++first1;++first2;}
    else {
      *result = *first1;
      ++result;
      return result;
    }
  }
  return result;
}


// return three elements of the intersection
template <class InputIterator1, class InputIterator2, class OutputIterator>
  OutputIterator set_3_intersection (InputIterator1 first1, InputIterator1 last1,
                                     InputIterator2 first2, InputIterator2 last2,
                                     OutputIterator result)
{
  while (first1!=last1 && first2!=last2)
  {
    int counter = 0;
    if (*first1<*first2) ++first1;
    else if (*first2<*first1) ++first2;
    else {
      *result = *first1;
      ++result; ++first1; ++first2; ++counter;
      if (counter == 3)
        return result;
    }
  }
  return result;
}



// return two elements of the intersection
template <class InputIterator1, class InputIterator2, class OutputIterator>
  OutputIterator set_2_intersection (InputIterator1 first1, InputIterator1 last1,
                                     InputIterator2 first2, InputIterator2 last2,
                                     OutputIterator result)
{
  while (first1!=last1 && first2!=last2)
  {
    int counter = 0;
    if (*first1<*first2) ++first1;
    else if (*first2<*first1) ++first2;
    else {
      *result = *first1;
      ++result; ++first1; ++first2; ++counter;
      if (counter == 2)
        return result;
    }
  }
  return result;
}



}







#endif



