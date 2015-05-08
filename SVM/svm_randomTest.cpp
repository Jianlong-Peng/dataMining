/*
 * name: svm_randomTest.cpp
 * OBJ: to do random test
 * author: jlpeng
 * create: 2012-07-10
 * update: 2012-07-12
 */

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include "randomTestSVM.h"

using namespace std;

void exit_with_help();
int parse_args(int argc, char *argv[], struct svm_parameter_construct &param, double *proportion, int *repeat, bool *verbose);

int main(int argc, char *argv[])
{
    if(argc < 3)
        exit_with_help();

    struct svm_parameter_construct param;
    double proportion;
    int repeat;
    bool verbose;
    int i = parse_args(argc, argv, param, &proportion, &repeat, &verbose);
    if(argc-i != 2)
        exit_with_help();

    RandomTestSVM rt(argv[i],param);
    rt.run(proportion, repeat, argv[i+1], verbose);

    return 0;
}

int parse_args(int argc, char *argv[], struct svm_parameter_construct &param, double *proportion, int *repeat, bool *verbose)
{
    vector<int> labels;
    vector<double> weights;
    int i;

    *proportion = 0.7;
    *repeat = 100;
    *verbose = false;

    for(i=1; i<argc;)
    {
        if(argv[i][0] != '-')
            break;
        i += 1;
        if(i >= argc) {
            cerr << "invalid options" << endl;
            exit_with_help();
        }

        switch(argv[i-1][1])
        {
            case 's': param.setSVMType(atoi(argv[i])); break;
            case 't': param.setKernelType(atoi(argv[i])); break;
            case 'd': param.setDegree(atoi(argv[i])); break;
            case 'g': param.setGamma(atof(argv[i])); break;
            case 'c': param.setCost(atof(argv[i])); break;
            case 'r': param.setCoef0(atof(argv[i])); break;
            case 'e': param.setTolerance(atof(argv[i])); break;
            case 'p': param.setEpsilon(atof(argv[i])); break;
            case 'b': param.setIfProbability(atoi(argv[i])); break;
            case 'w': labels.push_back(atoi(&argv[i-1][2])); weights.push_back(atof(argv[i])); break;
            case 'l': *proportion = atof(argv[i]); break;
            case 'n': *repeat = atoi(argv[i]); break;
            case 'v': *verbose=true; i-=1;break;
            default: cerr << "Invalid option " << argv[i-1] << endl; exit_with_help();
        }
        i += 1;
    }
    if(labels.size())
        param.setWeight(labels, weights);
    if(*proportion<=0. || *proportion>=1.) {
        cerr << "`-l learn-proportion` should be among (0., 1.), but " << proportion << " is given!" << endl;
        exit(EXIT_FAILURE);
    }

    return i;
}

void exit_with_help()
{
    cerr << "Usage: svm_randomTest.exe [options] train.svm result.txt" << endl
         << "options:" << endl
         << "-s svm_type: set type of SVM (default 0)" << endl
         << "   0 -- C-SVC" << endl
         << "   3 -- epsilon-SVR" << endl
         << "-t kernel_type: set type of kernel function (default 2)" << endl
         << "   0 -- linaer: u'*v" << endl
         << "   1 -- polynomial: (gamma*u'*v + coef0)^degree" << endl
         << "   2 -- radial basis function: exp(-gamma*|u-v|^2)" << endl
         << "   3 -- sigmoid: tanh(gamma*u'*v + coef0)" << endl
         << "-d degree: set degree in kernel function (default 3)" << endl
         << "-g gamma: set gamma in kernel function (default 1/num_features)" << endl
         << "-r coef0: set coef0 in kernel function (default 0)" << endl
         << "-c cost: set the parameter C of C-SVC, epsilon-SVR (default 1)" << endl
         << "-p epsilon: set the epsilon in loss function of epsilon-SVR (default 0.1)" << endl
         << "-e epsilon: set tolerance of termination criterion (default 0.001)" << endl
         << "-b probability_estimates: whether to train a SVC or SVR model for probability estimates, o or 1 (default 0)" << endl
         << "-wi weight: set the parameter C of class i to weight*C, for C-SVC (default 1)" << endl
         << "-l learn-proportion: to pick this proportion samples, should be among (0., 1.) (default: 0.7)" << endl
         << "-n times: to repeat doing random test for `times` times (default: 100)" << endl
         << "-v: if passed, I'll tell you which test is being done now! (default: no such info given)" << endl;
    exit(EXIT_FAILURE);
}
