/*
 * name: scoring.hpp
 * OBJ: some scor functions
 * author: jlpeng
 * create: 2012-07-09
 * update: 2012-07-11
 * dependence:
 *   None
 */
#ifndef SCORING_HPP
#define SCORING_HPP

#include <iostream>
#include <vector>
#include <algorithm>
using std::vector;
using std::sort;

//ONLY for two-class problem, that is set(actual).size() == 2
//actual[i] = posValue  ==> positive class
template <typename T>
struct CM
{
    //actual, predict should be a pointer pointing to list of n values
    CM(const T* actual, const T* predict, int n, const T &posValue);
    CM(const vector<T> &actual, const vector<T> &predict, const T &posValue);
    int tp, fn, tn, fp;
    double se, sp, acc, bacc;
};

template <typename T>
CM<T>::CM(const T* actual, const T* predict, int n, const T &posValue)
{
    tp=0; fn=0; tn=0; fp=0;
    for(int i=0; i<n; i++)
    {
        if(actual[i] == posValue) {
            if(predict[i] == actual[i])
                ++tp;
            else
                ++fn;
        }
        else {
            if(predict[i] == actual[i])
                ++tn;
            else
                ++fp;
        }
    }
    se = 1.0*tp/(tp+fn);
    sp = 1.0*tn/(tn+fp);
    acc = 1.0*(tp+tn)/n;
    bacc = (se+sp)/2.;
}

template <typename T>
CM<T>::CM(const vector<T> &actual, const vector<T> &predict, const T &posValue)
{
    tp=0; fn=0; tn=0; fp=0;
    for(typename vector<T>::size_type i=0; i!=actual.size(); i++)
    {
        if(actual[i] == posValue) {
            if(predict[i] == actual[i])
                ++tp;
            else
                ++fn;
        }
        else
        {
            if(predict[i] == actual[i])
                ++tn;
            else
                ++fp;
        }
    }
    se = 1.0*tp/(tp+fn);
    sp = 1.0*tn/(tn+fp);
    acc = 1.0*(tp+tn)/actual.size();
    bacc = (se+sp)/2.;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// only for auc
class Comp{
	const double *dec_val;
	public:
	Comp(const double *ptr): dec_val(ptr){}
	bool operator()(int i, int j) const{
		return dec_val[i] > dec_val[j];
	}
};

//to calculate AUC
template <typename T>
struct AUC
{
    AUC(const T *actual, const double *predict, int n, const T &posValue);
    AUC(const vector<T> &actual, const vector<double> &predict, const T &posValue);
    double area;
};

template <typename T>
AUC<T>::AUC(const T *actual, const double *predict, int n, const T &posValue)
{
	int i;
	std::vector<int> indices(n);

	for(i = 0; i < n; ++i) indices[i] = i;

	sort(indices.begin(), indices.end(), Comp(predict));

	int tp = 0,fp = 0;
    area = 0.;
	for(i = 0; i < n; i++) {
		if(actual[indices[i]] == posValue) tp++;
		else {
			area += tp;
			fp++;
		}
	}

	if(tp == 0 || fp == 0)
	{
        std::cerr << "warning: Too few postive true labels or negative true labels" << std::endl;
		area = 0;
	}
	else
		area = area / tp / fp;
}

template <typename T>
AUC<T>::AUC(const vector<T> &actual, const vector<double> &predict, const T &posValue)
{
    size_t i, n=actual.size();
	vector<size_t> indices(n);

	for(i = 0; i < n; ++i) indices[i] = i;

	std::sort(indices.begin(), indices.end(), Comp(predict));

	int tp = 0,fp = 0;
    area = 0.;
	for(i = 0; i < n; i++) {
		if(actual[indices[i]] == posValue) tp++;
		else {
			area += tp;
			fp++;
		}
	}

	if(tp == 0 || fp == 0)
	{
        std::cerr << "warning: Too few postive true labels or negative true labels" << std::endl;
		area = 0;
	}
	else
		area = area / tp / fp;
}


//to check if all values are identical!!!
bool check_if_same(const double *x, int n);
bool check_if_same(const vector<double> &x);

//pearson correlation coefficient
double R2(const double *x, const double *y, int n);
double R2(const vector<double> &x, const vector<double> &y);
//root mean square error
double mse(const double *x, const double *y, int n);
double mse(const vector<double> &x, const vector<double> &y);
#endif
