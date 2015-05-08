/*
 * name: randomTestSVM.hpp
 * OBJ: implementation of random test of SVM(C-SVC, nu-SVR)
 *   OPTIONS FOR SVM_PARAMETER:
 *   svm-type (default 0)
 *     0 - C-SVC; 3 - epsilon-SVR
 *   kernel_type (default 2)
 *     0 - linear: u'*v;  1 - polynomial: (gamma*u'*v + coef0)^degree
 *     2 - radial basis function: exp(-gamma*|u-v|^2);
 *     3 - sigmoid: tanh(gamma*u'*v + coef0)
 *     4 - precomputed kernel (kernel values in training_set_file)
 *   other options:
 *     degree: set degree in kernel function (default: 3)
 *     gamma: set gamma in kernel function (default: 1/num_features)
 *     cost: set the parameter C of C-SVC, epsilon-SVR (default: 1)
 *     coef0: set coef0 in kernel function (default: 0)
 *     epsilon: set the epsilon in loss function of epsilon-SVR (default: 0.1)
 *     tolerance: set tolerance of termination criterion (default: 0.001)
 *     wi: set the parameter C of class i to weight*C, for C-SVC (default: 1)
 * author: jlpeng
 * create: 2012-07-09
 * update: 2012-07-12
 * dependence:
 *   <SVM/svm.h>
 *   <SVM/svmTool.hpp>
 */
#ifndef RANDOMTESTSVM_HPP
#define RANDOMTESTSVM_HPP

#include "svm.h"
#include "svmTool.h"

class RandomTestSVM
{
public:
    //constructor: read svm problem
    RandomTestSVM(char *infile, struct svm_parameter_construct &parameter);
    RandomTestSVM(char *infile, struct svm_parameter *param1);
    //core method: do random split validation
    void run(double train_proportion, int times, const char *outfile, bool verbose=false);
    //destructor
    ~RandomTestSVM();

private:
    struct svm_problem *prob;
    struct svm_parameter *param;
};

#endif
