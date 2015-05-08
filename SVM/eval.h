#ifndef _EVAL_H
#define _EVAL_H

#include <stdio.h>
#include "svm.h"

#ifdef __cplusplus
extern "C" {
#endif

/* cross validation function */
//double binary_class_cross_validation(const struct svm_problem *prob, const struct svm_parameter *param, int nr_fold);
double *binary_class_cross_validation(const struct svm_problem *prob, 
        const struct svm_parameter *param, int nr_fold, int valid_method, int verbose);
/*
possible values for valid_method
1<->precision
2<->recall
3<->fscore
4<->bac
5<->auc
6<->accuracy
others<->accuracy
*/

/* predict function */
//void binary_class_predict(FILE *input, FILE *output); 
double *binary_class_predict(FILE *input, FILE *output, struct svm_model* model, 
        int valid_method, int predict_probability); 

//self added at 2011-09-26
double binary_svm_predict(struct svm_model* model, struct svm_node *x, double *labels);

//originally extern struct svm_model* model;
void exit_input_error_eval(int line_num);

#ifdef __cplusplus
}
#endif


#endif

