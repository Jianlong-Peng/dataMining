/*=============================================================================
#     FileName: SVM_SBS.cpp
#         Desc: 
#       Author: jlpeng
#        Email: jlpeng1201@gmail.com
#     HomePage: 
#      Version: 0.0.1
#      Created: 2012-09-01 19:15:40
#   LastChange: 2012-09-03 12:46:25
#      History:
=============================================================================*/

#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <featureSelection/SVM_SBS.h>
#include <SVM/grid.h>
#include <SVM/svmTool.hpp>

using std::vector;
using std::cout;
using std::endl;
using std::find;

vector<int> svm_sbs(struct svm_problem *prob0, struct svm_parameter *para0, 
        int nr_fold, int valid_method, bool verbose)
{
    vector<int> features = svm_features(prob0);
    struct svm_problem *prob1 = copy_svm_problem(prob0);
    int round(0);
    double *init_result = grid(prob1,para0,nr_fold,valid_method);
    if(verbose) {
        cout << "initial: c=" << init_result[0] << " g=" << init_result[1] 
            << " rate=" << init_result[2] << endl;
    }
    double best_result = init_result[2];
    double *best_result_each_round = Malloc(double,3);
    // core part
    while(features.size() > 1)
    {
        ++round;
        best_result_each_round[2] = -1e8;
        int best_rm_feature(-2);
        if(verbose)
            cout << "round #" << round << endl;
        // try to remove each available feature (one per time)
        for(vector<int>::const_iterator iter=features.begin(); iter!=features.end(); ++iter) {
            struct svm_problem *tmp_prob = rm_feature(prob1, *iter);
            double *result = grid(tmp_prob, para0, nr_fold, valid_method);
            if(verbose) {
                cout << " feature removed: " << *iter << ", c=" << result[0]
                    << " g=" << result[1] << " rate=" << result[2] << endl;
            }
            if(result[2] > best_result_each_round[2]) {
                best_result_each_round[2] = result[2];
                best_result_each_round[0] = result[0];
                best_result_each_round[1] = result[1];
                best_rm_feature = (int)(*iter);
            }
            free_svm_problem(tmp_prob);
            free(result);
        }
        // remove the feature from dataset if possible
        if(best_result_each_round[2] > best_result) {
            best_result = best_result_each_round[2];
            struct svm_problem *tmp_prob = rm_feature(prob1,best_rm_feature);
            free_svm_problem(prob1);
            prob1 = tmp_prob;
            features.erase(find(features.begin(),features.end(),best_rm_feature));
            if(verbose) {
                cout << "==> remove feature #" << best_rm_feature << ", c="
                    << best_result_each_round[0] << " g=" << best_result_each_round[1]
                    << " rate=" << best_result << endl;
            }
        }
        else{
            if(verbose)
                cout << "==> It's not better than previous result!!" << endl;
            break;
        }
    }
    
    if(verbose) {
        cout << "====" << endl << "features selected:" << endl;
        for(vector<int>::const_iterator iter=features.begin(); iter!=features.end(); ++iter)
            cout << *iter << " ";
        cout << endl;
    }

    free(init_result);
    free(best_result_each_round);
    return features;
}


