/*
 * name: scoring.cpp
 * OBJ: definition of methods declared in `scoring.hpp`
 * author: jlpeng
 * create: 2012-07-09
 * update: 2012-07-12
 */

#include <evaluation/scoring.h>
#include <iostream>
#include <vector>
#include <cmath>
using std::vector;
using std::cout;
using std::cerr;
using std::endl;

//to check if all values are identical
bool check_if_same(const double *x, int n)
{
    int count_same = 1;
    for(int i=1; i<n; i++) {
        if(x[i]-x[0] <= 1e-8)
            count_same += 1;
    }
    return (count_same==n)?true:false;
}
bool check_if_same(const vector<double> &x)
{
    unsigned int count_same = 1;
    for(vector<double>::size_type i=1; i<x.size(); i++) {
        if(x[i] - x[0] <= 1e-8)
            count_same += 1;
    }
    return (count_same==x.size())?true:false;
}

//pearson correlation coefficient
double R2(const double *x, const double *y, int n)
{
    double mean_x, mean_y;
    double sum_xy=0.0, sum_xx=0.0, sum_yy=0.0, sum_x=0.0, sum_y=0.0;
    double r;
    
    if(check_if_same(x,n) || check_if_same(y,n)) {
        cerr << "Warrning: at least one of the array has identical values!!!!" << endl;
        r = 0.0;
    }
    else
    {
        for(int i=0; i<n; i++)
        {
            sum_x += x[i];
            sum_y += y[i];
            sum_xy += x[i]*y[i];
            sum_xx += x[i]*x[i];
            sum_yy += y[i]*y[i];
        }
        mean_x = sum_x*1.0/n;
        mean_y = sum_y*1.0/n;
        r = (sum_xy - n*mean_x*mean_y)/sqrt((sum_xx - n*pow(mean_x,2))*(sum_yy - n*pow(mean_y,2)));
    }

    return r*r;
}

double R2(const vector<double> &x, const vector<double> &y)
{
    double mean_x, mean_y;
    double sum_xy=0.0, sum_xx=0.0, sum_yy=0.0, sum_x=0.0, sum_y=0.0;
    double r;
    
    if(check_if_same(x) || check_if_same(y)) {
        cerr << "Warrning: at least one of the array has identical values!!!!" << endl;
        r = 0.0;
    }
    else
    {
        for(vector<double>::size_type i=0; i<x.size(); i++)
        {
            sum_x += x[i];
            sum_y += y[i];
            sum_xy += x[i]*y[i];
            sum_xx += x[i]*x[i];
            sum_yy += y[i]*y[i];
        }
        mean_x = sum_x*1.0/x.size();
        mean_y = sum_y*1.0/x.size();
        r = (sum_xy - x.size()*mean_x*mean_y)/sqrt((sum_xx - x.size()*pow(mean_x,2))*(sum_yy - x.size()*pow(mean_y,2)));
    }

    return r*r;
}

//root mean square error
//(x-y)^2/n
double mse(const double *x, const double *y, int n)
{
    double error = 0.0;
    for(int i=0; i<n ;i++)
        error += pow(x[i]-y[i], 2);
    return error/n;
}
double mse(const vector<double> &x, const vector<double> &y)
{
    double error = 0.0;
    for(vector<double>::size_type i=0; i<x.size(); i++)
        error += pow(x[i]-y[i], 2);
    return error/x.size();
}


