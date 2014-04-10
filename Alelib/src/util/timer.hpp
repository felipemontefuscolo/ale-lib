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

#ifndef ALELIB_TIMER_HPP
#define ALELIB_TIMER_HPP


#include <cstdio>
#include <cstring>
#include <list>
#include <iostream>
#include <string>

#ifdef ALE_HAS_OPENMP
//#if 0
#  define TIMER_HAS_OPENMP ALE_HAS_OPENMP
#endif

#ifdef TIMER_HAS_OPENMP
#  include <omp.h>
#else
#  include <ctime>
#endif

namespace fi_FepTimer
{
  struct Item {
    Item(double e, const char* n) : elapsed(e) {
      strcpy(fname, n);
    }

    Item() : elapsed(0) {
      //fname[0] = '\n';
    };
    double elapsed;
    char fname[256];
  };

}

class Timer
{
public:
  typedef fi_FepTimer::Item Item;
  typedef std::list<Item> List;
  typedef List::iterator Iter;
  typedef List::const_iterator CIter;

  enum {LimitPushBacks = 64};

  explicit Timer()
  {
    #ifdef TIMER_HAS_OPENMP
    m_method = "OpenMp";
    #else
    m_method = "ctime";
    #endif
  }

  void restart()
  {
    #ifdef TIMER_HAS_OPENMP
    m_elapsed = omp_get_wtime();
    #else
    m_temp = clock();
    #endif
  }

  double elapsed(const char* fname = "", bool print_now = false)
  {
    char  buff[256];

    #ifdef TIMER_HAS_OPENMP
    m_elapsed = omp_get_wtime() - m_elapsed;
    #else
    m_elapsed = static_cast<double>( clock()  - m_temp)/(1.*CLOCKS_PER_SEC);
    #endif
    sprintf(buff, "elapsed %8.3fs in : %s\n", static_cast<float>(m_elapsed), fname);

    if (print_now)
      printf("%s", buff);

    if (m_list.size() >= LimitPushBacks)
      m_list.pop_front();
    m_list.push_back(Item(m_elapsed, buff));
  
    return m_elapsed;
  }

  void printTimes() const
  {
    for (CIter it= m_list.begin(); it != m_list.end(); ++it)
    {
      printf("%s", it->fname);
    }
  }

  void addItems(Timer const& other)
  {
    if (other.m_list.empty())
      return;
    for (CIter it= other.m_list.begin(); it != other.m_list.end(); ++it)
      (this->m_list).push_back( *it );
  }
  
  void printMethod() const
  {
    printf("%s\n", m_method.c_str());
  }

protected:
  double      m_elapsed;
  std::string m_method;
  List	      m_list;
  #ifndef TIMER_HAS_OPENMP
  clock_t     m_temp;
  #endif


};


#endif

