/*=============================================================================
#     FileName: main_svmsbs.cpp
#         Desc: 
#       Author: jlpeng
#        Email: jlpeng1201@gmail.com
#     HomePage: 
#      Version: 0.0.1
#      Created: 2012-09-03 09:50:32
#   LastChange: 2012-09-03 10:12:54
#      History:
=============================================================================*/

#include <iostream>
#include <vector>
#include <cstdlib>
#include <SVM/svm.h>
#include <SVM/svmTool.hpp>
#include <featureSelection/SVM_SBS.h>

using namespace std;

int parse_option(int argc, char *argv[], struct svm_parameter *para, int *nr_fold, int *valid_method);
void exit_with_help();
static void print_null(const char *s) {}

int main(int argc, char *argv[])
{
    struct svm_parameter *para = Malloc(struct svm_parameter, 1);
    int nr_fold, valid_method;
    int i = parse_option(argc, argv, para, &nr_fold, &valid_method);
    struct svm_problem *prob = read_svm_problem(argv[i]);
    vector<int> select_features = svm_sbs(prob,para,nr_fold,valid_method);

    return 0;
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
	cerr << "Sequential backward selection!" << endl
        << "usage: grid.exe [options] input.svm" << endl
        << "[options]" << endl
        << "  -wi weight : set the parameter C of class i to weight*C, for C-SVC(default 1)" << endl
        << "  -v nr_fold: n-fold cross validation mode (default 5)" << endl
        << "  -E valid_method: evaluation method(default: 6)." << endl
        << "      1 -- precision" << endl
        << "      2 -- recall" << endl
        << "      3 -- fscore" << endl
        << "      4 -- BACC" << endl
        << "      5 -- auc" << endl
        << "      6 -- accuracy" << endl
        << "WARNING: It's really time-consuming!!!!" << endl;
	exit(EXIT_FAILURE);
}

