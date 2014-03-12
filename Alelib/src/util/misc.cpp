#include "misc.hpp"
#include <cstring>
#include <cstdlib>
#include <algorithm>

namespace alelib
{

/** @brief Searches the stream for a key-word and return the position after
 *  the key-word. If the key-word was not found, return ULONG_MAX.
 *  @param word the key-word.
 *  @param word_size size of the key-word.
 *  @param fp the stream.
 *  @note a key-word is a word that is at the beginning of a line.
 */ 
long int find_keyword(const char* word, int word_size, FILE *fp)
{
  char buff[256];
  
  while (fgets(buff, sizeof(buff), fp) && !feof(fp))
  {
    if(!strncmp(buff, word, word_size)) // if they are equal
    {
      return ftell(fp);
    }
  }
  
  return -1;
}



/**
 * C++ version 0.4 std::string style "itoa":
 * Contributions from Stuart Lowe, Ray-Yuan Sheu,
 * Rodrigo de Salvo Braz, Luc Gallant, John Maloney
 * and Brian Hunt
 * 
 * modified: Felipe Montefuscolo
 */
std::string itos(int value)
{

	std::string buf;

	enum { kMaxDigits = 35, base=10 };
	buf.reserve( kMaxDigits ); // Pre-allocate enough space.

	int quotient = value;

	// Translating number to string with base:
	do {
		buf += "0123456789"[ std::abs( quotient % base ) ];
		quotient /= base;
	} while ( quotient );

	// Append the negative sign
	if ( value < 0) buf += '-';

	std::reverse( buf.begin(), buf.end() );
	return buf;
}

std::string itoafill0(int value, int fill)
{
	std::string buf;

	enum { kMaxDigits = 35, base=10 };
	buf.reserve( kMaxDigits ); // Pre-allocate enough space.

  fill = fill < kMaxDigits ? fill : kMaxDigits;

	int quotient = value;

	// Translating number to string with base:
	do {
		buf += "0123456789"[ std::abs( quotient % base ) ];
		quotient /= base;
	} while ( quotient );
  
  int i(fill - buf.size());
  
  while(i>0)
  {
    buf += '0';
    i=fill - buf.size();
  }
  
  std::reverse( buf.begin(), buf.end() );
  return buf;
}

// remove espaços finais de strings
std::string stripTrailingSpaces(std::string name)
{
  //if (name.empty())
  //  return name;
  while(!name.empty() && *(name.end()-1) == ' ')
    name.erase(name.end()-1);
  return name;
}

/** @example <tt>user/file.dat</tt> returns <tt>user/</tt>
 *  @note aassumes that is a valid file name.
 */ 
std::string getRelativePath(std::string const& name)
{
  size_t bar = name.rfind("/");
  
  if (bar==std::string::npos)
    return "./";
  else
    return name.substr(0,bar+1);
}

/** @example <tt>user/file.dat</tt> returns <tt>.dat</tt>
 *  @note assumes that is a valid file name.
 */ 
std::string getExtension(std::string const& name)
{
  int size = name.size();
  if (size==0)
    return name;
  
  size_t dot = name.rfind(".");
  size_t bar = name.rfind("/");
  if (dot==std::string::npos)
    return "";
  else if ((bar==dot-1) || (dot==0) || (name.at(dot-1)=='.')  || 
           ((bar!=std::string::npos) && (bar>dot)) || (dot==name.size()-1) )
    return "";
  else 
    return name.substr(dot);
}

/** @example <tt>user/file.dat</tt> returns <tt>file</tt>
 *  @note assumes that is a valid file name.
 */ 
std::string getBaseName(std::string name)
{
  size_t bar = name.rfind("/");
  int npath;
  
  if (bar==std::string::npos)
    npath=0;
  else
    npath = name.substr(0,bar+1).size();

  int nexte = getExtension(name).size();
  
  for (int i = 0; i < nexte; ++i)
    name.erase(name.end()-1);
  for (int i = 0; i < npath; ++i)
    name.erase(name.begin());
  
  return name;
}

} // end namespace alelib

