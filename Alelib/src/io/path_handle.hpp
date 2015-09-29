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

#ifndef ALELIB_PATH_HANDLE_HPP
#define ALELIB_PATH_HANDLE_HPP

#include <string>

namespace alelib
{

class iPathHandle
{
public:

  iPathHandle() : m_is_family(false), m_name_padd(5)
  {
    this->setOutputFileName("untitled");
  }

  void setFamily(bool is_family)
  {
    m_is_family = is_family;
  }

  bool isFamily() const
  {
    return m_is_family;
  }

  void setNamePadding(int i)
  {
    m_name_padd = i;
  }

  /** @param a file name (without extension) or a path in the form <tt>foo/bar/</tt>
   *  @example if <tt>foo</tt> and <tt>bar</tt> are two directories:
   *           -foo/file  (ok)
   *           -file.vtk  (bad)
   *           -foo/bar   (error)
   *           -foo/bar/  (ok)
   *           -foo/bar/jow (ok only if <tt>bar</tt> exists)
   */
  void setOutputFileName(const char* name);


protected:
  /// TODO
  void printPathInfo();

protected:

  /*
   * Before read a mesh, this function must be called
   * @param filename file name
   * @param extension expected extension
   * @param is_family output
   */
  bool registerFile(std::string filename, std::string const& extension);

  std::string paddedName(int filenum, std::string const& ext, int padding = 5);


  std::string m_in_meshfile;  // eg.   /home/user/test.msh
  std::string m_in_basename;  // eg.   test
  std::string m_in_extension; // eg.   .msh
  std::string m_in_path;      // eg.   /home/user/
  std::string m_out_basename; // eg.   result_file
  std::string m_out_path;     // eg.   /home/user/result/
  std::string m_out_extension;// eg.
  bool        m_is_family; // se o output sera impresso como familia
  int         m_name_padd;

};

}

#endif
