/*=============================================================================
#     FileName: SVM_SBS.h
#         Desc: using SBS to select features for SVM
#       Author: jlpeng
#        Email: jlpeng1201@gmail.com
#     HomePage: 
#      Version: 0.0.1
#      Created: 2012-09-01 15:54:47
#   LastChange: 2012-09-03 10:07:14
#      History:
=============================================================================*/


#ifndef  SVM_SBS_H
#define  SVM_SBS_H

#include <vector>
#include <SVM/svm.h>

std::vector<int> svm_sbs(struct svm_problem *prob, struct svm_parameter *para, 
        int nr_fold, int valid_method, bool verbose=true);

#endif   /* ----- #ifndef SVM_SBS_H  ----- */

