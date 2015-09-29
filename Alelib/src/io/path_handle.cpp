#include <cstdio>
#include <sys/stat.h>

#include "io_aux.hpp"
#include "path_handle.hpp"
#include "../util/macros.hpp"
#include "../util/assert.hpp"

namespace alelib
{

/** @param a file name (without extension) or a path in the form <tt>foo/bar/</tt>
 *  @example if <tt>foo</tt> and <tt>bar</tt> are two directories:
 *           -foo/file  (ok)
 *           -file.vtk  (bad)
 *           -foo/bar   (error)
 *           -foo/bar/  (ok)
 *           -foo/bar/jow (ok only if <tt>bar</tt> exists)
 */
void iPathHandle::setOutputFileName(const char* name)
{
  m_out_basename = getBaseName(name);
  m_out_path     = getRelativePath(name);
  m_out_extension= getExtension(name);

  if (m_out_basename.empty())
    m_out_basename = "untitled";

  struct stat  dir_stat;

  /* check if path passed by user exists */
  bool exists = static_cast<bool>(!lstat(m_out_path.data(), &dir_stat));
  bool isdir  = S_ISDIR(dir_stat.st_mode);

  if (exists && isdir)
    return;
  else if (!exists)
  {
    // try to create
    if (mkdir(m_out_path.data(), S_IRWXU | S_IRGRP | S_IXGRP | S_IXOTH))
    {
      printf("ERROR: can not create directory `%s'\n", m_out_path.c_str());
      throw;
    }
    return;
  }
  else // so m_out_path_ exists but it's not a path ... strange, ahn?
  {
    printf("ERROR: can not guess path name, sorry ...\n");
    throw;
  }

}

std::string iPathHandle::paddedName(int filenum, std::string const& ext, int padding)
{
  // filenum : a suffix to basename; the series number
  if (m_is_family)
    return m_out_path+m_out_basename+itoafill0(filenum, padding)+ext;
  else
    return m_out_path+m_out_basename+ext;
}

/*
 * Before reading a mesh, this function must be called
 * @param filename file name
 * @param extension expected extension
 * @param is_family output
 */
bool iPathHandle::registerFile(std::string filename, std::string const& extension)
{

  FILE *file_ptr = fopen(filename.c_str(), "r");
  ALELIB_ASSERT(file_ptr!=NULL, "can not find mesh file", std::invalid_argument);
  fclose(file_ptr);

  m_in_meshfile  = stripTrailingSpaces(filename);
  m_in_extension = getExtension(filename);
  m_in_basename  = getBaseName(filename);
  m_in_path      = getRelativePath(filename);

  if (m_in_extension.empty())
    printf("WARNING: mesh file without extension\n");
  else if ( m_in_extension != extension)
  {
    printf("WARNING: wrong file extension: %s expected, got %s", extension.c_str(), m_in_extension.c_str());
  }

  /* default values */
  m_out_path     = m_in_path;
  m_out_basename = m_in_basename;

  return 0;
}




void iPathHandle::printPathInfo()
{
  printf("Input file: \n");
  printf("meshfile:    %s\n",m_in_meshfile.c_str());
  printf("basename:    %s\n",m_in_basename.c_str());
  printf("suffix:      %s\n",m_in_extension.c_str());
  printf("path:        %s\n",m_in_path.c_str());
  printf("Output file:\n");
}


}
