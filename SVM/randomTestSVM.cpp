/*=============================================================================
#     FileName: randomTestSVM.cpp
#         Desc: 
#       Author: jlpeng
#        Email: jlpeng1201@gmail.com
#     HomePage: 
#      Version: 0.0.1
#      Created: 2012-07-09
#   LastChange: 2012-09-01 20:36:39
#      History:
=============================================================================*/

#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <algorithm>
#include <bitset>
#include <set>
#include <vector>
#include <ctime>
#include "randomTestSVM.h"
#include <evaluation/scoring.h>

using std::cout;
using std::cerr;
using std::endl;
using std::bitset;
using std::set;
using std::vector;

void print_null(const char *s) {}

RandomTestSVM::RandomTestSVM(char *infile, struct svm_parameter_construct &parameter)
{
    prob = read_svm_problem(infile); 
    param = parameter.copySVMParameter();
}

RandomTestSVM::RandomTestSVM(char *infile, struct svm_parameter *param1)
{
    prob = read_svm_problem(infile);
    param = Malloc(struct svm_parameter, 1);
    param->svm_type    = param1->svm_type;
    param->kernel_type = param1->kernel_type;
    param->degree      = param1->degree;
    param->gamma       = param1->gamma;
    param->coef0       = param1->coef0;
    param->nu          = param1->nu;
    param->cache_size  = param1->cache_size;
    param->C           = param1->C;
    param->eps         = param1->eps;
    param->p           = param1->p;
    param->shrinking   = param1->shrinking;
    param->probability = param1->probability;
    param->nr_weight   = param1->nr_weight;
    if(param->nr_weight && param->weight!=NULL)  //in case of uninitizlized!!!!
    {
        param->weight_label = Malloc(int, param->nr_weight);
        param->weight = Malloc(double, param->nr_weight);
        for(int i=0; i<param->nr_weight; i++) {
            param->weight_label[i] = param1->weight_label[i];
            param->weight[i] = param1->weight[i];
        }
    }
}
void getPickIds(const struct svm_problem *prob, int sample_size, bool isClassify, 
        bitset<SVM_BIT_SIZE> &mark)
{
    mark.reset();
    if(isClassify)
    {
        //to get unique labels
        set<double> unique_labels_set;
        for(int i=0; i<prob->l; i++)
            unique_labels_set.insert(prob->y[i]);
        vector<double> unique_labels(unique_labels_set.begin(), unique_labels_set.end());
        //to get IDs for each class
        vector<vector<int> > label_ids(unique_labels.size());
        for(int i=0; i<prob->l; i++)
        {
            vector<double>::const_iterator find_iter = 
                find(unique_labels.begin(), unique_labels.end(), prob->y[i]);
            //std::ptrdiff_t diff = find_iter - unique_labels.begin();
            label_ids[find_iter - unique_labels.begin()].push_back(i);
        }
        //to calculate sample size of each class
        vector<int> label_sample_size(unique_labels.size());
        for(unsigned int i=0; i<unique_labels.size(); i++) {
            label_sample_size[i] = int(1.0*label_ids[i].size()/prob->l*sample_size);
            if(label_sample_size[i] == 0)
                label_sample_size[i] = 1;  //at least one sample per class
            //std::cout << "class " << unique_labels[i] << " has " << label_ids[i].size() << " instances." << " I'll pick " << label_sample_size[i] << " items!" << std::endl;
        }
        //sampling for each class
        for(unsigned int i=0; i<unique_labels.size(); i++)
        {
            //permute ith class
            for(unsigned int j=0; j<label_ids[i].size(); j++) {
                int m = j+rand()%(label_ids[i].size() - j);
                std::swap(label_ids[i][j], label_ids[i][m]);
            }
            //get first label_sample_size[i] ids from permuted label_ids[i]
            for(int j=0; j<label_sample_size[i]; j++)
                mark.set(label_ids[i][j]);
        }
    }
    else
    {
        int *perm = Malloc(int, prob->l);
        int i;
        for(i=0; i<prob->l; i++) perm[i] = i;
        for(i=0; i<prob->l; i++) {
            int j = i+rand()%(prob->l - i);
            std::swap(perm[i], perm[j]);
        }
        for(i=0; i<sample_size; i++)
            mark.set(perm[i]);
        free(perm);
    }
}

void RandomTestSVM::run(double train_proportion, int times, const char *outfile, bool verbose)
{
    FILE *outf = fopen(outfile, "w");
    svm_set_print_string_function(&print_null);

    bool isClassify = (param->svm_type==C_SVC)?true:false;
    double mean[] = {0., 0., 0., 0., 0., 0., 0., 0.};
    bitset<SVM_BIT_SIZE> mark;
    int *testPred[2];
    testPred[0] = Malloc(int,prob->l);
    testPred[1] = Malloc(int,prob->l);
    //int testPred[2][prob->l];
    // testPred[0][j]: number of times being picked as test.
    // testPred[1][j]: number of times being predicted correctly.
    for(int j=0; j<prob->l; ++j) {
        testPred[0][j] = 0;
        testPred[1][j] = 0;
    }

    if(isClassify)
        fprintf(outf, "#round train_se train_sp train_bacc train_auc   test_se test_sp test_bacc test_auc\n");
    else
        fprintf(outf, "#round train_r2 train_mse test_r2 test_mse\n");

    srand((unsigned)time(0));
    for(int i=1; i<=times; i++)
    {
        if(verbose)
            cerr << "round #" << i << endl;
        struct svm_problem *test = Malloc(struct svm_problem, 1);
        test->l = 0;
        getPickIds(prob,(int)(prob->l*train_proportion),isClassify,mark);
        struct svm_problem *train = subset_svm_problem(prob, mark, test);
        if(train == NULL) {
            fclose(outf);
            free_svm_problem(test);
            free_svm_problem(prob);
            free_svm_parameter(param);
            exit(EXIT_FAILURE);
        }
        /* //used for testing!!!!!!!!!
        if(i == 1)
        {
            save_svm_problem(train, "train1.svm");
            save_svm_problem(test, "test1.svm");
        }
        */
        struct svm_model *model = svm_train(train, param);

        double *train_pred_val = Malloc(double, train->l);
        double *test_pred_val = Malloc(double, test->l);
        for(int j=0; j<train->l; j++)
            svm_predict_values(model, train->x[j], &train_pred_val[j]);
        for(int j=0; j<test->l; j++)
            svm_predict_values(model, test->x[j], &test_pred_val[j]);

        if(isClassify)
        {
            if(svm_get_nr_class(model) != 2) {
                cerr << "the number of class is not equal to 2!!!!" << endl;
                free_svm_problem(prob);
                free_svm_parameter(param);
                exit(1);
            }
            //to get unique labels
            int *labels = Malloc(int, svm_get_nr_class(model));
            svm_get_labels(model, labels);
            int posLabel, negLabel;
            if(labels[0] <= 0) {
                posLabel = labels[1];
                negLabel = labels[0];
                for(int j=0; j<train->l; j++)
                    train_pred_val[j] *= -1;
                for(int j=0; j<test->l; j++)
                    test_pred_val[j] *= -1;
            }
            else {
                posLabel = labels[0];
                negLabel = labels[1];
            }
            //convert predict value to label
            //value>=0  ==> posLabel
            int *train_pred_labels = Malloc(int, train->l);
            int *train_actual_labels = Malloc(int, train->l);
            int *test_pred_labels = Malloc(int, test->l);
            int *test_actual_labels = Malloc(int, test->l);
            for(int j=0; j<train->l; j++) {
                train_pred_labels[j] = (train_pred_val[j]>=0)?posLabel:negLabel;
                train_actual_labels[j] = int(train->y[j]);
            }
            int _j=0;
            for(int j=0; j<test->l; j++) {
                test_pred_labels[j] = (test_pred_val[j]>=0)?posLabel:negLabel;
                test_actual_labels[j] = int(test->y[j]);
                while(_j<prob->l && mark.test(_j)) ++_j;
                //cout << _j+1 << " ";
                testPred[0][_j] += 1;
                if(test_pred_labels[j]==test_actual_labels[j]) testPred[1][_j] += 1;
                ++_j;
            }
            //cout << endl;
            //calcualte se,sp,bacc,auc
            CM<int> cm_train = CM<int>(train_actual_labels, train_pred_labels, train->l, posLabel);
            AUC<double> auc_train = AUC<double>(train->y, train_pred_val, train->l, double(posLabel));
            CM<int> cm_test = CM<int>(test_actual_labels, test_pred_labels, test->l, posLabel);
            AUC<double> auc_test = AUC<double>(test->y, test_pred_val, test->l, double(posLabel));
            mean[0] += cm_train.se; mean[1] += cm_train.sp; mean[2] += cm_train.bacc;
            mean[3] += auc_train.area;
            mean[4] += cm_test.se; mean[5] += cm_test.sp; mean[6] += cm_test.bacc;
            mean[7] += auc_test.area;
            //output result
            fprintf(outf, "%6d %8.4lf %8.4lf %10.4lf %9.4lf   ", i,cm_train.se, cm_train.sp, cm_train.bacc, auc_train.area);
            fprintf(outf, "%7.4lf %7.4lf %9.4lf %8.4lf\n", cm_test.se, cm_test.sp, cm_test.bacc, auc_test.area);
            //free memory
            free(labels);
            free(train_pred_labels);
            free(train_actual_labels);
            free(test_pred_labels);
            free(test_actual_labels);
        }
        else
        {
            double train_r2 = R2(train_pred_val, train->y, train->l);
            double train_mse = mse(train_pred_val, train->y, train->l);
            double test_r2 = R2(test_pred_val, test->y, test->l);
            double test_mse = mse(test_pred_val, test->y, test->l);
            mean[0] += train_r2; mean[1] += train_mse; mean[2] += test_r2; mean[3] += test_mse;
            fprintf(outf, "%6d %8.4lf %9.4lf   %7.4lf %8.4lf\n", i,train_r2,train_mse,test_r2,test_mse);
        }

        free(train_pred_val);
        free(test_pred_val);
        svm_free_and_destroy_model(&model);
        free_svm_problem(train);
        free_svm_problem(test);
    }
    fprintf(outf, "==============================\n");
    if(isClassify) {
        fprintf(outf, "  mean %8.4lf %8.4lf %10.4lf %9.4lf   ", mean[0]/times,mean[1]/times,mean[2]/times,mean[3]/times);
        fprintf(outf, "%7.4lf %7.4lf %9.4lf %8.4lf\n", mean[4]/times, mean[5]/times, mean[6]/times, mean[7]/times);
    }
    else
        fprintf(outf, "  mean %8.4lf %9.4lf   %7.4lf %8.4lf\n", mean[0]/times,mean[1]/times,mean[2]/times,mean[3]/times);
    fclose(outf);

    if(verbose) {
        cout << "compound actualY #totPred #correct   %correct" << endl;
        for(int j=0; j<prob->l; ++j)
            printf("%8d %7d %8d %8d   %.4f\n",j+1,(int)(prob->y[j]),testPred[0][j],testPred[1][j],1.0*testPred[1][j]/testPred[0][j]);
    }
    
    free(testPred[0]);
    free(testPred[1]);
}

RandomTestSVM::~RandomTestSVM()
{
    free_svm_problem(prob);
    free_svm_parameter(param);
}
