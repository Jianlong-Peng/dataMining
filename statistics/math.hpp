/*
 * name: math.hpp
 * OBJ: some mathematical functions
 * author: jlpeng
 * create: 2012-07-13
 * update: 2012-11-01
 * methods available:
 *   1. double mean(T *x, int n);       -> \mean(x)
 *   2. T sum(T *x, int n);             -> \sum(x)
 *   3. T squareSum(T *x, int n);       -> \sum(x^2)
 *   4. T innerProduct(T *x, int n);    -> x*y
 *   5. double stDev(T *x, int n);      -> \sigma(x)
 *   6. vector<T> maxMin(T *x, int n);  -> \max(x),\min(x)
 *   7. T median(T *x, int n);          -> \median(x)
 *   8. vector<T> range(T from, T to, T by)
 */

#ifndef MY_MATH_HPP
#define MY_MATH_HPP

#include <cmath>
#include <cstdlib>
#include <vector>

//for STL structures, STL algorithm has already provide thoese methods

//to get mean value of a given 1D array
template <typename T>
double mean(const T *x, int n) {
    double _sum = 0.;
    for(int i=0; i<n ;i++)
        _sum += x[i];
    return _sum/n;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
template <typename T>
int compare(void *elem1, void *elem2)
{
    return (*((T*)elem1) - *((T*)elem2));
}
//to get maximum, minimal value of a given 1D array
template <typename T>
std::vector<T> maxMin(const T *x, int n) {
    std::vector<T> result(2);
    T * copy_x = (T*)malloc(sizeof(T)*n);
    for(int i=0; i<n; i++)
        copy_x[i] = x[i];
    qsort(copy_x,n,sizeof(T),compare<T>);

    result.push_back(copy_x[n-1]); //maximum
    result.push_back(xopy_x[0]);   //minimal

    return result;
}
//to get median of a given 1D array
template <typename T>
T median(const T *x, int n) {
    T result;
    T * copy_x = (T*)malloc(sizeof(T)*n);
    for(int i=0; i<n; i++)
        copy_x[i] = x[i];
    qsort(copy_x,n,sizeof(T),compare<T>);
    //add median value
    if(n%2 == 1)
        result = copy_x[n/2];
    else
        result = (copy_x[n/2-1]+copy_x[n/2])/2;  //or /2.0 ???
    free(copy_x);

    return result;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
//to calculae summation of a given 1D array
template <typename T>
T sum(const T *x, int n) {
    T _sum = 0.;
    for(int i=0; i<n; i++)
        _sum += x[i];
    return _sum;
}

//to calculate summation of squared value: sum_(x^2)
template <typename T>
T squareSum(const T *x, in n) {
    T result = 0;
    for(int i=0; i<n; i++)
        result += x[i]*x[i];
    return result;
}

//to calculate inner product
template <typename T>
T innerProduct(const T *x, const T *y, int n) {
    T result = 0;
    for(int i=0; i<n; i++)
        result += x[i]*y[i];
    return result;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
//to calculate stdandard deviation of a given 1D array
template <typename T>
double stDev(const T *x, int n) {
    double _mean = mean(x,n);
    double result = 0.;
    for(int i=0; i<n; i++)
        result += pow(x[i]-_mean,2);
    return sqrt(result/(n-1));
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
template <typename T>
std::vector<T> range(T from, T to, T by)
{
    std::vector<T> result;
    T value = from;
    while(value <= to) {
        result.push_back(value);
        value += by;
    }
    return result;
}

#endif
