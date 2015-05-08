/*=============================================================================
#     FileName: featureImportance.cpp
#         Desc: each time after removing one feature, a libsvm model is built 
#               based on the remaining features, and BACC of 10-fold CV is calculated.
#               To see if significant difference occurs when centern descriptor was 
#               removed from dataset!
#       Author: jlpeng
#        Email: jlpeng1201@gmail.com
#     HomePage: 
#      Version: 0.0.1
#      Created: 2012-11-10 15:19:56
#   LastChange: 2013-05-03 11:24:16
#      History:
=============================================================================*/

#include <iostream>
#include <vector>
#include <cstdlib>
#include "svmTool.h"
#include "svm.h"
#include "eval.h"

using namespace std;

int parse_args(int argc, char *argv[], int *nfold, struct svm_parameter_construct &param, int *method);
void exit_with_help(char *progname);
void print_null(const char *s) {}
void nCV(struct svm_problem *problem, struct svm_parameter *parameter, int nfold, double init_value,
        struct svm_problem *(*modify)(const struct svm_problem *, int));
void predict_test(const struct svm_model *model, const struct svm_problem *test_prob, double init_value,
        struct svm_problem *(*modify)(const struct svm_problem *, int));
double calcBACC(const vector<double> &predYs, const double *actualYs);

int main(int argc, char *argv[])
{
    if(argc < 3)
        exit_with_help(argv[0]);
    
    int nfold, method;
    struct svm_parameter_construct param;
    int i = parse_args(argc, argv, &nfold, param, &method);
    if(argc-i!=1 && argc-i!=2)
        exit_with_help(argv[0]);
    struct svm_parameter *parameter = param.copySVMParameter();
    struct svm_problem *problem = read_svm_problem(argv[i]);

    svm_set_print_string_function(&print_null);

    struct svm_problem *(*func[])(const struct svm_problem *, int) = 
        {rm_feature,set_feature_to_zero,set_feature_to_mean,shuffle_feature_of};

    if(argc-i == 1) {
        // CV on all features
        double *result = binary_class_cross_validation(problem,parameter,nfold,4,0);
        cout << nfold << "-fold CV using all features, BACC=" << result[4] << ", OBJ_VALUE=" << OBJ_VALUE << endl;
        double init_value = result[4];
        free(result);
        printf("%-7s %-8s %-9s %-s\n","feature","BACC","delta","OBJ_VALUE");
        nCV(problem,parameter,nfold,init_value,func[method-1]);
        /*
        if(method == 1)
            nCV(problem,parameter,nfold,init_value,rm_feature);
        else if(method == 2)
            nCV(problem,parameter,nfold,init_value,set_feature_to_zero);
        else if(method == 3)
            nCV(problem,parameter,nfold,init_value,set_feature_to_mean);
        else if
        else
            cerr << "Error: currently '-m 3/4' is not supported when '-v n' is passed!" << endl;
        */
    }
    else {
        struct svm_problem *test_prob = read_svm_problem(argv[i+1]);
        struct svm_model *model = svm_train(problem,parameter);
        vector<double> pred_result = predict_svm_problem(model,test_prob);
        double test_bacc = calcBACC(pred_result,test_prob->y);
        cout << "BACC of original test set: " << test_bacc << endl;
        printf("%-7s %-8s %-9s %-s\n","feature","BACC","delta","OBJ_VALUE");
        predict_test(model,test_prob,test_bacc,func[method-1]);
        /*
        if(method == 2)
            predict_test(model,test_prob,test_ys,test_bacc,set_feature_to_zero);
        else if(method == 4)
            predict_test(model,test_prob,test_ys,test_bacc,predict_test_by_shuffle);
        else
            cerr << "Error: currently only '-m 2/4' is supported when [test.svm] is given!" << endl;
        */
    }

    free_svm_problem(problem);
    free_svm_parameter(parameter);

    return 0;
}

double calcBACC(const vector<double> &predYs, const double *actualYs)
{
    int tp=0,fp=0,fn=0,tn=0;
    for(vector<double>::size_type i=0; i<predYs.size(); ++i) {
        if(predYs[i] >= 0) {
            if(actualYs[i] == 1)
                ++tp;
            else
                ++fp;
        }
        else {
            if(actualYs[i] == 1)
                ++fn;
            else
                ++tn;
        }
    }
    double specificity = 1.0*tn/(tn+fp);
    double sensitivity = 1.0*tp/(tp+fn);
    return (specificity+sensitivity)/2.;
}

void predict_test(const struct svm_model *model, const struct svm_problem *test_prob, double init_value,
        struct svm_problem *(*modify)(const struct svm_problem *, int))
{
    vector<int> features = svm_features(test_prob);
    for(vector<int>::iterator iter=features.begin(); iter!=features.end(); ++iter) {
        struct svm_problem *prob = modify(test_prob,*iter);
        vector<double> pred_result = predict_svm_problem(model,prob);
        double test_bacc = calcBACC(pred_result,prob->y);
        printf("%-7d %-8.6f %-9.6f %-g\n",*iter,test_bacc,test_bacc-init_value,OBJ_VALUE);
        free_svm_problem(prob);
    }
}

void nCV(struct svm_problem *problem, struct svm_parameter *parameter, int nfold, double init_value,
        struct svm_problem *(*modify)(const struct svm_problem *, int))
{
    vector<int> features = svm_features(problem);
    for(vector<int>::iterator iter=features.begin(); iter!=features.end(); ++iter) {
        struct svm_problem *prob = modify(problem,*iter);
        double *result = binary_class_cross_validation(prob,parameter,nfold,4,0);
        printf("%-7d %-8.6f %-9.6f %-g\n",*iter,result[4],result[4]-init_value,OBJ_VALUE);
        free(result);
        free_svm_problem(prob);
    }
}

void print_problem(struct svm_problem *prob)
{
    for(int i=0; i<prob->l; ++i) {
        cout << prob->y[i];
        int k = 0;
        while(prob->x[i][k].index != -1) {
            cout << " " << prob->x[i][k].index << ":" << prob->x[i][k].value;
            ++k;
        }
        cout << endl;
    }
    cout << "==================END=============" << endl;
}


int parse_args(int argc, char *argv[], int *nfold, struct svm_parameter_construct &param, int *method)
{
    vector<int> labels;
    vector<double> weights;
    int i;
    
    *nfold = 10;
    for(i=1; i<argc;)
    {
        if(argv[i][0] != '-')
            break;
        i += 1;
        if(i >= argc) {
            cerr << "invalid options" << endl;
            exit_with_help(argv[0]);
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
            case 'v': *nfold=atoi(argv[i]);break;
            case 'm': *method=atoi(argv[i]);break;
            default: cerr << "Invalid option " << argv[i-1] << endl; exit_with_help(argv[0]);
        }
        i += 1;
    }
    if(*method<1 || *method>4) {
        cerr << "Error: '-m' needs either 1 or 2, but " << *method << " is given!" << endl;
        exit(EXIT_FAILURE);
    }
    if(labels.size())
        param.setWeight(labels, weights);

    return i;
}

void exit_with_help(char *progname)
{
    cerr << "Usage: " << progname << " [options] train.svm [test.svm]" << endl
         << "[options]" << endl
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
         << "-v n: do n-fold CV (default 10)" << endl
         << "-m int: which method to be used to evaluate importance" << endl
         << "   1 - each time one feature is removed" << endl
         << "   2 - each time values of one feature is set equal to 0" << endl
         << "   3 - each time values of one feature is set equal to {mean}" << endl
         << "   4 - each time values of one feature is randomly shuffled" << endl
         << "[test.svm]" << endl
         << "   if it's given, it'll be used to estimate model!" << endl
         << "   '-v n' will be ignored!" << endl;
    exit(EXIT_FAILURE);
}

