#ifndef SIMINT_TEST_COMMON_HPP
#define SIMINT_TEST_COMMON_HPP

#include <array>
#include <vector>
#include <string>
#include <map>
#include <fstream>
    
#include "simint/shell/shell.h"

#define RND_ZERO 1.0e-15

#define NCART(am) ((am>=0)?((((am)+2)*((am)+1))>>1):0)

//! Just a vector of gaussian shells
typedef std::vector<gaussian_shell> GaussianVec;

//! Maps a vector of gaussians to their am
typedef std::map<int, GaussianVec> ShellMap;


/*! \brief Checks to see if a quartet is valid for calculation
 *
 * ie, checks that am[0] >= am[1], am[2] >= am[3], and (am[1]+am[2] >= am[3]+am[4])
 */
bool ValidQuartet(std::array<int, 4> am);


/*! \brief Checks to see if a quartet is valid for calculation
 *
 * ie, checks that i < j, k < l, and (i+j < k+l)
 */
bool ValidQuartet(int i, int j, int k, int l);


/*! \brief Iterate over cartesian components of a gaussian
 *
 * The elements of the array represent the exponents on x, y, and z.
 * This increments \p g to be the next gaussian. If the next
 * gaussian is valid, the function returns true. Otherwise,
 * it returns false.
 */
bool IterateGaussian(std::array<int, 3> & g);


/*! \brief Deep copies a vector of gaussians
 */
GaussianVec CopyGaussianVec(const GaussianVec & v);

/*! \brief Frees memory associated with a vector of gaussians
 */
void FreeGaussianVec(GaussianVec & agv);

/*! \brief Deep copies a map of gaussian vectors
 */
ShellMap CopyShellMap(const ShellMap & m);

/*! \brief Frees memory associated with a map of gaussian vectors
 */
void FreeShellMap(ShellMap & m);


/*! \brief Reads a basis set from a file into a ShellMap
 */
ShellMap ReadBasis(const std::string & file);


/*! \brief Find some maximum parameters from a basis set (shell map)
 *
 * Returns an array containing
 *   0.) maximum angular momentum
 *   1.) maximum number of primitives found in a shell
 *   2.) maximum storage needed for cartesians of a shell of a given angular momentum
 */
std::array<int, 3> FindMapMaxParams(const ShellMap & m);




/*! \brief Chop very small values to zero
 *
 * \param [in] calc Array of doubles to chop
 * \param [in] ncalc Length of the array
 */
void Chop(double * const restrict calc, int ncalc);

/*! \brief Calculate the error relative to a reference
 *
 * \param [in] calc Calculated values
 * \param [in] ref Reference values
 * \param [in] ncalc Number of values in \p calc and \p ref
 * \return A pair representing the maximum absolute and relative error found
 */
std::pair<double, double> CalcError(double const * const restrict calc,
                                    double const * const restrict ref,
                                    int ncalc);




#endif