/*=============================================================================
#     FileName: grid_eSVR.cpp
#         Desc: 
#       Author: jlpeng
#        Email: jlpeng1201@gmail.com
#     HomePage: 
#      Created: 2013-04-23 15:32:06
#   LastChange: 2013-04-23 21:13:25
#      History:
=============================================================================*/

#include <iostream>
#include <cstdlib>
#include <cmath>
#include "svm.h"
#include "svmTool.h"

using namespace std;

const double BASE_P = 0.5;
const int C_BEGIN = -5;
const int C_END = 15;
const int C_STEP = 2;
const int G_BEGIN = -15;
const int G_END = 3;
const int G_STEP = 2;

void initialize_svm_parameter(struct svm_parameter *para);
double calcR2(const double *x, const double *y, int n);
double calcMAE(const double *x, const double *y, int n);
void grid_epsilonSVR(const struct svm_problem *prob, struct svm_parameter *para,
        int k, double variable[4],
        double (*evaluate)(const double *x, const double *y, int n),
        bool (*better)(double x, double y));

void exit_with_help(const char *name)
{
    cerr << endl << "  OBJ  : to select parameters(-p,-c,-g) for epsilon-SVR" << endl
        << endl << "  Usage: " << name << " [options] train.svm" << endl
        << "  [options]" << endl
        << "  -m int: method to evaluate model <default 1>" << endl
        << "   1 - R^2" << endl
        << "   2 - MAE" << endl
        << "  -v int: n-fold validation <default 10>" << endl;
    exit(EXIT_FAILURE);
}

inline bool my_greater(double x, double y) {
    return (x>y)?true:false;
}

inline bool my_less(double x, double y) {
    return (x<y)?true:false;
}

static void print_null(const char *s) {}

int main(int argc, char *argv[])
{
    if(argc<2 || argc>6)
        exit_with_help(argv[0]);
    
    int nr_fold = 10;
    int method = 1;
    int i;
    for(i=1; i<argc; i+=2) {
        if(argv[i][0] != '-')
            break;
        if(argv[i][1] == 'm')
            method = atoi(argv[i+1]);
        else if(argv[i][1] == 'v')
            nr_fold = atoi(argv[i+1]);
        else {
            cerr << "Error: invalid option <" << argv[i] << ">" << endl;
            exit(EXIT_FAILURE);
        }
    }
    if(method<1 || method>2) {
        cerr << "Error: invalid value for '-m' <" << method << ">" << endl;
        exit(EXIT_FAILURE);
    }
    if(nr_fold <= 0) {
        cerr << "Error: invalid value for '-v' <" << method << ">" << endl;
        exit(EXIT_FAILURE);
    }

    // initialize 'svm_parameter'
    struct svm_parameter *para = Malloc(struct svm_parameter,1);
    initialize_svm_parameter(para);
    svm_set_print_string_function(print_null);
    struct svm_problem *prob = read_svm_problem(argv[i]);
    double result[4];  // p,c,g,value
    if(method == 1)
        grid_epsilonSVR(prob,para,nr_fold,result,calcR2,my_greater);
    else if(method == 2)
        grid_epsilonSVR(prob,para,nr_fold,result,calcMAE,my_less);
    else
        ;
    cout << "==> Best: p=" << result[0] << ", c=" << result[1] << ", g=" << result[2];
    if(method == 1)
        cout << ", R^2=" << result[3] << endl;
    else if(method == 2)
        cout << ", MAE=" << result[3] << endl;
    else
        ;
    free_svm_problem(prob);
    free_svm_parameter(para);

    return 0;
}

void initialize_svm_parameter(struct svm_parameter *para)
{
    para->svm_type    = EPSILON_SVR;
    para->kernel_type = RBF;
    para->degree      = 3;            // degree in kernel function (polynomial)
    para->gamma       = 0;
    para->coef0       = 0;            // coef0 in kernel function (polynomial, sigmoid)
    para->nu          = 0.5;          // parameter nu of nu-SVC, one-class SVM, and nu-SVR
    para->cache_size  = 100;          // cache memory size in MB
    para->C           = 1;            // parameter C of C-SVC, epsilon-SVR, and nu-SVR
    para->eps         = 1e-3;         // tolerance of termination criterion
    para->p           = 0.1;          // epsilon in loss function of epsilon-SVR
    para->shrinking   = 1;            // whether to use the shrinking heuristics
    para->probability = 0;            // probability estimation
    para->nr_weight   = 0;
    para->weight_label = NULL;
    para->weight      = NULL;
}

double calcR2(const double *x, const double *y, int n)
{
    double mean_x=0., mean_y=0.;
    double sum_xy=0., sum_xx=0., sum_yy=0.;
    for(int i=0; i<n; ++i) {
        sum_xy += (x[i]*y[i]);
        mean_x += x[i];
        mean_y += y[i];
        sum_xx += pow(x[i],2);
        sum_yy += pow(y[i],2);
    }
    mean_x /= n;
    mean_y /= n;
    double r = (sum_xy-n*mean_x*mean_y) / sqrt((sum_xx-n*pow(mean_x,2))*(sum_yy-n*pow(mean_y,2)));
    return pow(r,2);
}

double calcMAE(const double *x, const double *y, int n)
{
    double result = 0.;
    for(int i=0; i<n; ++i)
        result += fabs(x[i]-y[i]);
    result /= n;
    return result;
}

// variable: [p,c,g,value]
// evaluate: to evaluate the model
// better  : to compare two evaluation results, and determine which one is better
//   true  - if x is better than y
//   false - otherwise
void grid_epsilonSVR(const struct svm_problem *prob, struct svm_parameter *para,
        int k, double variable[4],
        double (*evaluate)(const double *x, const double *y, int n),
        bool (*better)(double x, double y))
{
    double *target = Malloc(double,prob->l);
    double best_value = -1E8;
    double curr_value;
    for(int i=1; i<=5; ++i) {
        para->p = BASE_P*i;
        for(int c=C_BEGIN; c<=C_END; c+=C_STEP) {
            para->C = pow(2.,c);
            for(int g=G_BEGIN; g<=G_END; g+=G_STEP) {
                para->gamma = pow(2.,g);
                cout << "p=" << para->p << ", c=" << para->C << ", g=" << para->gamma;
                svm_cross_validation(prob,para,k,target);
                curr_value = evaluate(prob->y,target,prob->l);
                cout << "  " << curr_value;
                if(better(curr_value,best_value)) {
                    cout << "   best until now" << endl;
                    best_value = curr_value;
                    variable[0] = para->p;
                    variable[1] = para->C;
                    variable[2] = para->gamma;
                }
                else
                    cout << endl;
            }
        }
    }
    variable[3] = best_value;
}

