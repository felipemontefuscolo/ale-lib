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

#ifndef ALELIB_SHAPE_FUNCS_HPP
#define ALELIB_SHAPE_FUNCS_HPP


namespace alelib
{

#ifndef ALELIB_SCALAR_TYPE
typedef double Real;
#endif


class ShapeFuncImpl; // implementation detail

/** @brief An object of this class is a set of shape functions \f$ \{\varphi^{i}\}_{i=1}^{n} \f$
 *         in the master element.
 */ 
class ShapeFunction
{
  ShapeFuncImpl* m_pimpl;
  bool           m_gdisc; // globally discontinuous
  
public:

  ShapeFunction();
  ~ShapeFunction();
  ShapeFunction(ShapeFunction const&);
  ShapeFunction& operator=(ShapeFunction const&);

  /** @brief Set the type of the shape function.
   * @param type A string corresponding to the type. Depending on its type, \b degree will be ignored. \n
   *             This string has the form: \code
   *             "type1, option1, option2, ... optionN + type2, option1, ... + type3, ..."
   *             \endcode
   *             Where the plus sign + concatenates shape functions \c types. \n
   *             Examples: 
   *                   - <c> "Lagrange_simplex"       </c> Lagrange shape functions on simplicies; \n
   *                   - <c> "Lagrange"               </c> Same as above; \n
   *                   - <c> "Lagrange_hcube"         </c> Lagrange shape functions on quads (2d) / hexahedrons (3d); \n
   *                   - <c> "Lagrange,discontinuous" </c> Discontinuous Lagrange shape functions; \n
   *                   - <c> "Lagrange + Bubble"      </c> Concatenates a bubble function to Lagrange functions; \n
   *                   - <c> "Bubble + Lagrange"      </c> Same as above, but the bubble function is numbered first; \n
   *                   - <c> "Hermite"                </c> Hermite functions for simplicies; \n
   *                   - <c> "iHermite"               </c> Incomplete Hermite functions for simplicies (without bubbles); \n
   *                   - <c> "Hermite,value_dofs"     </c> The Hermite functions corresponding to the value dofs,
   *                                                       i.e., \f$ \varphi^{i}(\bf{x^j}) = \delta_{ij} \f$; \n
   *                   - <c> "Hermite,grad_dofs"      </c> The Hermite functions corresponding to the grad dofs,
   *                                                       i.e., \f$ \partial_d \varphi^{i,c}(\bf{x^j}) = \delta_{ij}\delta_{cd} \f$; \n
   * @param dim The dimension of the master element where shape functions are defined, if needed.
   * @param degree The degree of shape functions, if needed.
   */ 
  void setType(const char* type, int dim, int degree = -1);

  /** @brief returns if \c setType() has been called. */ 
  bool isSet() const;

  /** @brief Returns a string with the name of the shape functions type and the options used. */ 
  const char* name() const;

  /** @brief Evaluates the shape function \f$ \varphi^{ith} \f$  at coordinate \f$ \bf{x} \f$ in the master element.
   *  @param x The coordinate where to evaluate.
   *  @param ith Which function to evaluate.
   *  @return the Value of \f$ \varphi^{ith}(\bf{x}) \f$.
   */
  Real value(Real const*x, unsigned ith) const;

  /** @brief Evaluates the component \c c of gradient of shape function, \f$ \partial_c \varphi^{ith} \f$, at coordinate \f$ \bf{x} \f$
             in the reference element.
   *  @param x The coordinate where to evaluate.
   *  @param ith Which function to evaluate.
   *  @param c c-th component of the gradient.
   *  @return the value of \f$ \partial_c \varphi^{ith}(\bf{x}) \f$.
   */
  Real grad(Real const*x, unsigned ith, unsigned c) const;

  bool isTauEquivalent() const;
  bool isLinear() const;
  
  /**  @brief Returns \c true if the globally counterpart of the shape functions are discontinuous
   *          anywhere between true elements or inside them, and false otherwise. When \c true, all dofs
   *          that were associated to vertices, facets and corners, will be associated to cell.
   */ 
  bool isGloballyDiscontinuous() const;

  /** @brief returns the number of degrees of freedom. */
  int numDofs() const;
  
  /** @brief returns the dimensions of the master cell */ 
  int dim()    const;

  int numDofsInsideCorner()  const;
  int numDofsInsideFacet()   const;
  int numDofsInsideCell()    const;

  int numDofsPerVertice() const;
  int numDofsPerCorner()  const;
  int numDofsPerFacet()   const;
  int numDofsPerCell()    const;

  
};


} // end namespace


#endif




