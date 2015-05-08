/*
 * name: similarity.hpp
 * OBJ: to implement distance like methods to evaluate similarity
 * author: jlpeng
 * create: 2012-07-11
 * update: 2012-07-13
 * dependence:
 *   None
 */

#ifndef DISTANCE_HPP
#define DISTANCE_HPP

/*
 * you can set the `SIM_BIT_SIZE` in your header(or source file if no header file) at the very begining
 * e.g. in `example.hpp`
 * #define SIM_BIT_SIZE 215
 * ...
 * #include <statistics/similarity.hpp>
 * ...
 */
#ifndef SIM_BIT_SIZE
#define SIM_BIT_SIZE 2048
#endif

#include <vector>
#include <bitset>
#include <cmath>

template <typename T>
inline double euclidean(const T *x, const T *y, int n)
{
    double sum = 0.;
    for(int i=0; i<n; i++)
        sum += pow(x[i]-y[i],2);
    return sqrt(sum);
}

template <typename T>
inline double euclidean(const std::vector<T> &x, const std::vector<T> &y)
{
    double sum = 0.;
    for(typename std::vector<T>::size_type i=0; i<x.size(); i++)
        sum += pow(x[i]-y[i],2);
    return sqrt(sum);
}

inline double tanimoto(const std::bitset<SIM_BIT_SIZE> &x, const std::bitset<SIM_BIT_SIZE> &y)
{
    return 1.0*(x&y).count()/((x|y).count());
}
#endif
