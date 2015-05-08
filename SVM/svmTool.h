/*=============================================================================
#     FileName: svmTool.h
#         Desc: some useful tools. Some of these methods are copied from
#               `svm-train.c`, `eval.cpp`
#       Author: jlpeng
#        Email: jlpeng1201@gmail.com
#     HomePage: 
#      Version: 0.0.1
#      Created: 2012-07-09 13:23:18
#   LastChange: 2013-05-02 08:02:08
#      History:
=============================================================================*/

#ifndef SVMTOOL_HPP
#define SVMTOOL_HPP

#include <cstdio>
#include <bitset>
#include <vector>
#include "svm.h"

#define Malloc(type,n) (type *)malloc((n)*sizeof(type))

//used in method `subset_svm_problem`
#ifndef SVM_BIT_SIZE
#define SVM_BIT_SIZE 2048
#endif

char *readline(FILE *input);
struct svm_problem* read_svm_problem(const char *infile); //no precomputed kernel allowed!!
void save_svm_problem(struct svm_problem *prob, const char *outfile);
void free_svm_problem(struct svm_problem *prob);
void free_svm_parameter(struct svm_parameter *param);

struct svm_problem* copy_svm_problem(const struct svm_problem *prob);
void copy_feature_value_from(struct svm_problem *to, const struct svm_problem *from, int feature);
struct svm_problem *set_feature_to_zero(const struct svm_problem *prob, int feature);
struct svm_problem *set_feature_to_mean(const struct svm_problem *prob,int feature);
struct svm_problem* shuffle_feature_of(const struct svm_problem *prob, int feature);
std::vector<int> svm_features(const struct svm_problem *prob);
//int num_of_features(const struct svm_problem *prob) {return svm_features(prob).size();}

std::vector<double> predict_svm_problem(const svm_model *model, const svm_problem *prob);

//struct used to construct svm_parameter
struct svm_parameter_construct
{
    struct svm_parameter *param;

    svm_parameter_construct();
    
    void setSVMType(int type);  //0 - C_SVC; 3 - EPSILON_SVR
    void setKernelType(int type); //0 - LINEAR; 1 - POLY; 2 - RBF; 3 - SIGMOID
    void setCost(double cost) {param->C = cost;}
    void setGamma(double gamma) {param->gamma = gamma;}
    void setDegree(int degree) {param->degree = degree;}
    void setCoef0(double coef0) {param->coef0 = coef0;}
    void setTolerance(double tolerance) {param->eps = tolerance;}
    void setEpsilon(double epsilon) {param->p = epsilon;}
    //void setNu(double nu) {param->p = nu}  //only used for svm type `nu-SVC` `one-class SVM` `nu-SVR`
    void setIfProbability(int prob);  //prob = either 0 or 1
    void setWeight(const int *label, const double *w, int num);
    void setWeight(const std::vector<int> &label, const std::vector<double> &weights);
    struct svm_parameter* copySVMParameter();  //return a deepcopy copy of *param

    ~svm_parameter_construct();
};

//stratified sampling if isClassify==true
//ATTENTION: if `remain` is passed and uninitizlized, then let remain->l be 0 !!!!!!!!!!!!!!!!!!!!!!
//before calling one of bellowing methods, you can set the rand seed by
//srand((unsigned)time(0));
struct svm_problem* subset_svm_problem(struct svm_problem *prob, 
        double proportion, bool isClassify, struct svm_problem *remian=NULL);
struct svm_problem* subset_svm_problem(struct svm_problem *prob, 
        int sample_size, bool isClassify, struct svm_problem *remain=NULL);
struct svm_problem* subset_svm_problem(struct svm_problem *prob, 
        std::bitset<SVM_BIT_SIZE> &sample_mark, struct svm_problem *remain=NULL);

// to remove one feature (with prob->x[i][n].index==id) from the original dataset
struct svm_problem* rm_feature(const struct svm_problem *prob, int id);

// to get a subset of features
// if pick==true, features specified by mark will be picked out,
// otherwise, they'll be abandoned.
struct svm_problem* pick_features(struct svm_problem *prob, 
        std::bitset<SVM_BIT_SIZE> &mark, bool pick=true);


#endif
