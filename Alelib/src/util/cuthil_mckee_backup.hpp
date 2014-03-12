// This file is part of FEPiC++, a toolbox for finite element codes.
//
// FEPiC++ is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// Alternatively, you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// FEPiC++ is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License and a copy of the GNU General Public License along with
// FEPiC++. If not, see <http://www.gnu.org/licenses/>.


#ifndef ALELIB_CUTHIL_MCKEE
#define ALELIB_CUTHIL_MCKEE

#include <vector>
#include <set>
#include <utility>

//#include <iostream>
//using namespace std;






class CuthilMckee
{
public:

  static const int PENALTY = 200;

  struct NodeT {
    int number;
    short pseudom_degree;
    NodeT(int n=0, short p=0) : number(n), pseudom_degree(p) {}
    //bool operator<(NodeT const& b)  const
    //{
    //	if (pseudom_degree != b.pseudom_degree)
    //		return pseudom_degree < b.pseudom_degree;
    //	else
    //		return number < b.number;
    //}
    bool operator<(NodeT const& b)  const
    {
    	if (number == b.number)
        return false;
    	else if (pseudom_degree == b.pseudom_degree)
        return number < b.number;
      else
    		return pseudom_degree < b.pseudom_degree;
    }    
    //bool operator<(NodeT const& b)  const
    //{
    //	if (number == b.number)
    //    return false;
    //  else
    //		return pseudom_degree < b.pseudom_degree;
    //}    
  };

  struct MarkT {
    enum {MARKED=1, PENALIZED=2};
    char m_mark;
    MarkT(char m=0) : m_mark(m) {};
    void mark() {m_mark |= MARKED;}
    void unmark() {m_mark &= (!MARKED);}
    bool marked() const {return m_mark & MARKED;}
    void penalize() {m_mark |= PENALIZED;}
    bool penalized() const {return m_mark & PENALIZED;}
  };

  typedef std::vector<std::set<int> >     TableT;
  typedef typename TableT::iterator       TableIter;
  typedef typename TableT::const_iterator TableConstIter;
  typedef std::set<NodeT>           Set;
  typedef std::set<NodeT>::iterator SetIter;

  /** @param[in] table sparse matrix.
   *  @param[in] first first node.
   *  @param[out] perm vector with size = number of nodes
   */ 
  void operator()(TableT const& table, int first, int* perm) const
  {
    
    
    const int n_nodes = table.size();
    std::vector<MarkT> marks(n_nodes); // also store if node is penalized 
    
    // penalized nodes are nodes that do not connect with themselves
    ALE_PRAGMA_OMP(parallel for)
    for (int i = 0; i < n_nodes; ++i)
    {
      if( table[i].find(i) == table[i].end() )
        marks[i].penalize();
    }
    
    first = findPeripheralNode(table, marks, first);
    
    //cout << "DEBUG peripheral node = " << first << endl;
    
    Set actual_lvl, next_lvl;
    
    NodeT temp_node;
    
    temp_node.number        = first;
    temp_node.pseudom_degree = table[first].size() - PENALTY*marks[first].penalized();
    
    actual_lvl.insert(temp_node);
    
    std::set<int>::const_iterator neib_it, neib_it_end;
    SetIter lvl_it, lvl_it_end;
    std::vector<int> iperm(n_nodes);
    int counter = 0;
    while (counter < n_nodes)
    {
      lvl_it = actual_lvl.begin();
      lvl_it_end = actual_lvl.end();
      for (; lvl_it!= lvl_it_end;  ++lvl_it)
      {
        iperm[counter++] = (*lvl_it).number;
        marks[(*lvl_it).number].mark();
      }
      
      // find next lvl
      lvl_it = actual_lvl.begin();
      lvl_it_end = actual_lvl.end();
      for (; lvl_it!= lvl_it_end;  ++lvl_it) // for each node of this level
      {
        neib_it = table[(*lvl_it).number].begin();
        neib_it_end = table[(*lvl_it).number].end();
        for (; neib_it != neib_it_end; ++neib_it)  // for each neighbor
        {
          if (marks[*neib_it].marked())
          {
            continue;
          }
          
          temp_node.number        = *neib_it;
          //temp_node.pseudom_degree = table[*neib_it].size() - PENALTY*marks[*neib_it].penalized();
          temp_node.pseudom_degree = std::distance(actual_lvl.begin(),lvl_it) - PENALTY*marks[*neib_it].penalized();
          next_lvl.insert(temp_node);
          //marks[*neib_it].mark();
        }
      }
      
      
      // or, if actual lvl is empty (independent nodes)
      if (actual_lvl.empty())
      {
        // find a node that is not marked
        for (int i = 0; i < n_nodes; ++i)
          if (!marks[i].marked())
          {
            temp_node.number        = i;
            temp_node.pseudom_degree = table[i].size() - PENALTY*marks[i].penalized();
            actual_lvl.insert(temp_node);
            break;
          }
        continue;
      }
      
      actual_lvl.clear();
      actual_lvl.swap(next_lvl);
      
    } // END WHILE
    
    ALE_PRAGMA_OMP(parallel for)
    for (int i = 0; i < n_nodes; ++i)
    {
      perm[iperm[i]] = n_nodes-i-1; // RCM
      //perm[iperm[i]] = i; //  CM
    }
  
  }// END OPERATOR()
  
  /** find pseudo-peripheral node
   *  return peripheral node
   */ 
  int findPeripheralNode(TableT const& table, std::vector<MarkT>& marks, int p0) const
  {
    int p1 = p0, lixo;
    int p0_length = findLength(table, marks, p1);
    lixo = p1;
    int p1_length = findLength(table, marks, lixo);
    
    if (p0_length < p1_length)
      return p1;
    else
      return p0;
    
  }

  // return length
  // modifica node
  int findLength(TableT const& table, std::vector<MarkT>& marks, int &node) const
  {
    const int n_nodes = table.size();
    
    int node_length=0;
    
    Set actual_lvl, next_lvl;
    
    NodeT temp_node;
    
    temp_node.number        = node;
    temp_node.pseudom_degree = table[node].size() - PENALTY*marks[node].penalized();
    
    actual_lvl.insert(temp_node);
    
    std::set<int>::const_iterator neib_it, neib_it_end;
    SetIter lvl_it, lvl_it_end;
    while (true)
    {
      ++node_length;
      
      lvl_it = actual_lvl.begin();
      lvl_it_end = actual_lvl.end();
      for (; lvl_it!= lvl_it_end;  ++lvl_it)
      {
        marks[(*lvl_it).number].mark();
      }
      
      // find next lvl
      lvl_it = actual_lvl.begin();
      lvl_it_end = actual_lvl.end();
      for (; lvl_it!= lvl_it_end;  ++lvl_it) // for each node of this level
      {
        neib_it = table[(*lvl_it).number].begin();
        neib_it_end = table[(*lvl_it).number].end();
        for (; neib_it != neib_it_end; ++neib_it)  // for each neighbor
        {
          if (marks[*neib_it].marked())
          {
            continue;
          }
          
          temp_node.number        = *neib_it;
          temp_node.pseudom_degree = table[*neib_it].size() - PENALTY*marks[*neib_it].penalized();
          next_lvl.insert(temp_node);
        }
      }
      
      // or, if actual lvl is empty (independent nodes)
      if (next_lvl.empty())
      {
        
        // find a node that is not marked
        for (int i = 0; i < n_nodes; ++i)
          if (!marks[i].marked())
          {
            temp_node.number        = i;
            temp_node.pseudom_degree = table[i].size() - PENALTY*marks[i].penalized();
            next_lvl.insert(temp_node);
            break;
          }
          
        // if still empty  
        if (next_lvl.empty())
          break;
      }
      
      actual_lvl.clear();
      actual_lvl.swap(next_lvl);
      
    } // END WHILE
    
    // restoring marks
    ALE_PRAGMA_OMP(parallel for)
    for (int i = 0; i < n_nodes; ++i)
    {
      marks[i].unmark();
    }
    
    node = (*actual_lvl.begin()).number;
    
    return node_length;
    
  }

  
};



#endif
