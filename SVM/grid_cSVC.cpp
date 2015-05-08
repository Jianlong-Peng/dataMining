/*=============================================================================
#     FileName: grid.cpp
#         Desc: definition of function declared in `grid.h`
#       Author: jlpeng
#        Email: jlpeng1201@gmail.com
#     HomePage: 
#      Version: 0.0.1
#      Created: 2012-09-01 16:12:38
#   LastChange: 2012-09-03 19:56:52
#      History:
=============================================================================*/

#include <iostream>
#include <cmath>
#include <cstdlib>
#include "svmTool.h"
#include "eval.h"

using namespace std;

#ifndef GRID_RANGE
#define GRID_RANGE
#define C_BEGIN -5
#define C_END 15
#define C_STEP 2
#define G_BEGIN -15
#define G_END 3
#define G_STEP 2
#endif

int parse_option(int argc, char *argv[], struct svm_parameter *para, int *nr_fold, int *valid_method);
void exit_with_help();
static void print_null(const char *s) {}

double* grid(struct svm_problem *prob0, struct svm_parameter *para0, 
        int nr_fold, int valid_method, bool verbose);

extern const char *methods[];

int main(int argc, char *argv[])
{
    //const char *methods[] = {"precision","recall","fscore","BACC","AUC","ACC"};
    struct svm_parameter *para = Malloc(struct svm_parameter, 1);
    int nr_fold, valid_method;
    int i = parse_option(argc, argv, para, &nr_fold, &valid_method);
    struct svm_problem *prob = read_svm_problem(argv[i]);
    double *result = grid(prob,para,nr_fold,valid_method,true);
    cout << "=>best: c=" << result[0] << ", g=" << result[1] 
        << " " << methods[valid_method-1] << "=" << result[2] << endl;
    free_svm_problem(prob);
    free_svm_parameter(para);
    free(result);
    return 0;
}

double* grid(struct svm_problem *prob0, struct svm_parameter *para0, 
        int nr_fold, int valid_method, bool verbose)
{
    double c0(para0->C), g0(para0->gamma);
    double *current_result;  // se, sp, acc, mcc, ?
    double *best_result = Malloc(double,3);  // c,g,evalute
    //const char *methods[] = {"precision","recall","fscore","BACC","AUC","ACC"};
    int c,g;
    best_result[2] = 0.;
    for(c=C_BEGIN; c<=C_END; c+=C_STEP)
    {
        para0->C = pow(2.,c);
        for(g=G_BEGIN; g<=G_END; g+=G_STEP)
        {
            para0->gamma = pow(2.,g);
            current_result = binary_class_cross_validation(prob0, para0, nr_fold, valid_method, 0);
            if(current_result[4] > best_result[2]) {
                best_result[2] = current_result[4];
                best_result[0] = para0->C;
                best_result[1] = para0->gamma;
            }
            if(verbose) {
                std::cout << c << " " << g << " " << current_result[4] 
                    << " (best c=" << best_result[0] << " g=" << best_result[1] << " "
                    << methods[valid_method-1] << "=" << best_result[2] << ")" << std::endl;
            }
            free(current_result);
        }
    }
    para0->C = c0;
    para0->gamma = g0;
    return best_result;
}

int parse_option(int argc, char *argv[], struct svm_parameter *para, int *nr_fold, int *valid_method)
{
	//return the subscription from which non-option arguments begin!!!!!!
	int i;
	
	svm_set_print_string_function(print_null);			//quiet mode
	
	para->svm_type = C_SVC;
	para->kernel_type = RBF;
	para->degree = 3;
	para->gamma = 0;	// 1/num_features
	para->coef0 = 0;
	para->nu = 0.5;
	para->cache_size = 100;
	para->C = 1;
	para->eps = 1e-3;
	para->p = 0.1;
	para->shrinking = 1;
	para->probability = 0;
	para->nr_weight = 0;
	para->weight_label = NULL;
	para->weight = NULL;
	
    *valid_method = 6;
	*nr_fold = 5;
	
	for(i=1;i<argc;i++)
	{
		if(argv[i][0] != '-')
			break;
		if(++i>=argc)
			exit_with_help();
		switch(argv[i-1][1])
		{
			case 'v':
				*nr_fold = atoi(argv[i]);
				if(*nr_fold < 2)
				{
					cerr << "n-fold cross validation: n must >= 2" << endl;
					exit_with_help();
				}
				break;
			case 'w':
				++para->nr_weight;
				para->weight_label = (int *)realloc(para->weight_label,sizeof(int)*para->nr_weight);
				para->weight = (double *)realloc(para->weight,sizeof(double)*para->nr_weight);
				para->weight_label[para->nr_weight-1] = atoi(&argv[i-1][2]);
				para->weight[para->nr_weight-1] = atof(argv[i]);
				break;
			case 'E':
				*valid_method = atoi(argv[i]);
				if(*valid_method<0 || *valid_method>6)
					*valid_method = 6;
				break;
			default:
				cerr << "Unknown option: -" << argv[i-1][1] << endl;
				exit_with_help();
		}
	}
	
	if(i+1 != argc)
	{
		cerr << "needs 1 non-option argument, but " << argc-i << " given" << endl;
		exit_with_help();
	}
	
	return i;
}

void exit_with_help()
{
	cerr << "usage: grid.exe [options] input.svm" << endl
        << "[options]" << endl
        << "  -wi weight : set the parameter C of class i to weight*C, for C-SVC(default 1)" << endl
        << "  -v nr_fold: n-fold cross validation mode (default 5)" << endl
        << "  -E valid_method: evaluation method(default: 6)." << endl
        << "      1 -- precision" << endl
        << "      2 -- recall" << endl
        << "      3 -- fscore" << endl
        << "      4 -- BACC" << endl
        << "      5 -- auc" << endl
        << "      6 -- accuracy" << endl;
	exit(EXIT_FAILURE);
}
