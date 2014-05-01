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


#ifndef ALELIB_SPARSE_TABLE_HPP
#define ALELIB_SPARSE_TABLE_HPP

#include <vector>
#include <stdexcept>

/**
*  A minimal implementation of a sparse table. Essentially, it is a vector of vectors.
*  Int: internal index
*/
template<typename T, typename Int = int, typename A = std::allocator< T > >
class SparseTable
{
public:

  typedef std::vector<T>   DataContainer;
  typedef std::vector<Int> IndexContainer;  

  typedef          A                              allocator_type;
  typedef typename A::reference                   reference;
  typedef typename A::const_reference             const_reference;
  typedef typename DataContainer::size_type       size_type;
  typedef typename DataContainer::difference_type difference_type;
  typedef typename A::pointer                     pointer;
  typedef typename A::const_pointer               const_pointer;


private:
  DataContainer  m_data;
  IndexContainer m_offsets; // size = n_rows + 1

public:

  SparseTable(): m_data(), m_offsets() {  }

  // reserve memory for DataContainer
  void reserveData(size_type s)
  { m_data.reserve(s); }

  // reserve memory for IndexContainer
  void reserveIndices(size_type s)
  { m_offsets.reserve(s); }

  size_type capacityData() const
  { return m_data.capacity(); }

  size_type capacityIndices() const
  { return m_offsets.capacity(); }
  
  T* data()
  { return m_data.data(); }

  T const* data() const
  { return m_data.data(); }
  
  // returns the total size
  size_type size() const
  { return m_data.size(); }

  size_type numRows() const
  { return m_offsets.size()-1; }

  // returns the size of the row i
  size_type size(Int i) const
  { return m_offsets.at(i+1) - m_offsets[i]; }

  void getData(T * dat, Int row) const
  {
    size_type const ss = size(row);
    for (size_type j=0; j<ss; ++j)
      dat[j] = m_data[j + m_offsets[row] ];
  }

  std::vector<T> getData(size_type row) const
  {
    typename std::vector<T>::iterator beg = m_data.begin() + m_offsets[row];
    typename std::vector<T>::iterator end = beg + size(row);
    return std::vector<T> (beg,end);
  }

  /// @warning all elements are destroyed
  void resize(size_type nrows)
  {
    m_data.clear();
    //m_data.reserve(1); // this is ti define m_data.begin()
    m_offsets.resize(nrows+1, Int(0));
  }

  /// @warning all elements are destroyed
  void resize(size_type nrows, size_type ncols, T const& val)
  {
    m_data.resize(nrows*ncols, val);
    m_offsets.resize(nrows+1);
    for (size_type i = 0; i < nrows+1; ++i)
    {
      m_offsets[i] = i*ncols;
    }
  }

  void shrink()
  {
    std::vector<T>(m_data).swap(m_data);
    std::vector<Int>(m_offsets).swap(m_offsets);
  }

  // insert new column if necessary
  T& operator() (size_type row, size_type col)
  {
    size_type const sr = size(row);
    if (col < sr)
      return m_data[ m_offsets[row] + col];
    else
    {
      Int const n_new = col - sr + 1;
      typename std::vector<T>::iterator pos = m_data.begin() + m_offsets[row+1];
      
      if (m_data.end()-pos < 0) // pos never can be greater than m_data.end(), otherwise m_offsets is wrong
        throw;
      m_data.insert(pos, n_new, T());

      for (size_type k = row+1; k < m_offsets.size(); ++k)
        m_offsets[k] += n_new;
      return m_data[ m_offsets[row] + col];
    }
  }

  T const& operator() (size_type row, size_type col) const
  {
    return at(row,col);
  }

  // insert new column if necessary
  T& at(size_type row, size_type col)
  {
    size_type const sr = size(row);
    if (col < sr)
      return m_data[ m_offsets[row] + col];
    else
      throw std::out_of_range("invalid index");
  }

  T const& at(size_type row, size_type col) const
  {
    size_type const sr = size(row);
    if (col < sr)
      return m_data[ m_offsets[row] + col];
    else
      throw std::out_of_range("invalid index");
  }

  // remove all elements = val
  void erase(T const& val)
  {
    size_type counter;
    typename std::vector<T>::iterator pos;
    for (difference_type i = m_offsets.size()-2; i>=0; --i)
    {
      for (difference_type j = m_offsets[i+1]-m_offsets[i]-1; j>=0; --j)
      {
        pos = m_data.begin() + m_offsets[i] + j;
        if (*pos == val)
        {
          m_data.erase(pos);
          for (int k = i+1; k < m_offsets.size(); ++k)
            --m_offsets[k];
        }

      }
    }
  }

  //

};







#endif


