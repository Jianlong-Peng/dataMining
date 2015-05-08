/*=============================================================================
#     FileName: svmTool.cpp
#         Desc: definition of methods declared in svmTool.hpp
#       Author: jlpeng
#        Email: jlpeng1201@gmail.com
#     HomePage: 
#      Version: 0.0.1
#      Created: 2012-07-09 13:33:52
#   LastChange: 2013-05-03 11:10:32
#      History:
=============================================================================*/

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <fstream>
#include <bitset>
#include <vector>
#include <set>
#include <algorithm>
#include "svmTool.h"
#include "svm.h"

using std::bitset;
using std::vector;
using std::set;
using std::find;
using std::cerr;
using std::endl;

char* readline(FILE *input)
{
    int len;
    int max_line_len = 1024;
    char *line = Malloc(char, max_line_len);
    
    if(fgets(line,max_line_len,input) == NULL)
        return NULL;

    while(strrchr(line,'\n') == NULL)
    {
        max_line_len *= 2;
        line = (char *) realloc(line,max_line_len);
        len = (int) strlen(line);
        if(fgets(line+len,max_line_len-len,input) == NULL)
            break;
    }
    return line;
}

svm_problem* read_svm_problem(const char *infile)
{
    int max_index, inst_max_index, i, j, errno;
    FILE *fp = fopen(infile,"r");
    char *endptr;
    char *idx, *val, *label, *line;
    struct svm_problem *prob = Malloc(struct svm_problem, 1);

    if(fp == NULL)
    {
        fprintf(stderr,"can't open input file %s\n",infile);
        exit(1);
    }

    prob->l = 0;
    vector<int> elements;  //modified by jlpeng

    while((line=readline(fp))!=NULL)
    {
        int nr_features = 0;
        char *p = strtok(line," \t"); // label

        // features
        while(1)
        {
            p = strtok(NULL," \t");
            if(p == NULL || *p == '\n') // check '\n' as ' ' may be after the last feature
                break;
            ++nr_features;
        }
        ++nr_features;
        ++prob->l;
        elements.push_back(nr_features);
    }
    rewind(fp);

    prob->y = Malloc(double,prob->l);
    prob->x = Malloc(struct svm_node *,prob->l);
    //struct svm_node *x_space = Malloc(struct svm_node,elements);

    max_index = 0;
    //j=0;
    for(i=0;i<prob->l;i++)
    {
        inst_max_index = -1; // strtol gives 0 if wrong format, and precomputed kernel has <index> start from 0
        line = readline(fp);
        //prob->x[i] = &x_space[j];
        prob->x[i] = Malloc(struct svm_node, elements[i]);
        label = strtok(line," \t");
        prob->y[i] = strtod(label,&endptr);
        if(endptr == label) {
            std::cerr << "Wrong input format at line " << i+1 << std::endl;
            fclose(fp);
            exit(1);
        }

        j = 0;
        while(1)
        {
            idx = strtok(NULL,":");
            val = strtok(NULL," \t");

            if(val == NULL)
                break;

            errno = 0;
            //x_space[j].index = (int) strtol(idx,&endptr,10);
            prob->x[i][j].index = (int) strtol(idx,&endptr,10);
            if(endptr == idx || errno != 0 || *endptr != '\0' || prob->x[i][j].index <= inst_max_index) {
                std::cerr << "Wrong input format at line " << i+1 << std::endl;
                fclose(fp);
                exit(1);
            }
            else
                inst_max_index = prob->x[i][j].index;

            errno = 0;
            //x_space[j].value = strtod(val,&endptr);
            prob->x[i][j].value = strtod(val,&endptr);
            if(endptr == val || errno != 0 || (*endptr != '\0' && !isspace(*endptr))) {
                std::cerr << "Wrong input format at line " << i+1 << std::endl;
                fclose(fp);
                exit(1); 
            }

            ++j;
        }

        if(inst_max_index > max_index)
            max_index = inst_max_index;
        //x_space[j++].index = -1;
        prob->x[i][j].index = -1;
    }
    fclose(fp);
    return prob;
}

void save_svm_problem(struct svm_problem *prob, const char *outfile)
{
    std::ofstream outf(outfile);
    for(int i=0; i<prob->l; i++)
    {
        outf << prob->y[i];
        int j = 0;
        while((prob->x[i][j]).index != -1)
        {
            outf << " " << (prob->x[i][j]).index << ":" << (prob->x[i][j]).value;
            ++j;
        }
        outf << std::endl;
    }
    outf.close();
}

void free_svm_problem(struct svm_problem *prob)
{
    free(prob->y);
    for(int i=0; i<prob->l; i++)
        free(prob->x[i]);
    free(prob->x);
    free(prob);
}

void free_svm_parameter(struct svm_parameter *param)
{
    if(param->nr_weight) {
        free(param->weight_label);
        free(param->weight);
    }
    free(param);
}

struct svm_problem* copy_svm_problem(const struct svm_problem *prob)
{
    struct svm_problem *new_prob = Malloc(struct svm_problem,1);
    new_prob->l = prob->l;
    new_prob->y = Malloc(double, new_prob->l);
    for(int i=0; i<new_prob->l; ++i)
        new_prob->y[i] = prob->y[i];
    new_prob->x = Malloc(struct svm_node*, new_prob->l);
    for(int i=0; i<new_prob->l; ++i)
    {
        int n(0);
        while(prob->x[i][n].index != -1)
            ++n;
        ++n;
        new_prob->x[i] = Malloc(struct svm_node, n);
        n = 0;
        do
        {
            new_prob->x[i][n].index = prob->x[i][n].index;
            new_prob->x[i][n].value = prob->x[i][n].value;
        }while(prob->x[i][n++].index != -1);
    }
    return new_prob;
}

void copy_feature_value_from(struct svm_problem *to, const struct svm_problem *from, int feature)
{
    int count = 0;
    for(int i=0; i<from->l; ++i) {
        double from_value=0.;
        int j = 0;
        while(from->x[i][j].index != -1) {
            if(from->x[i][j].index == feature) {
                from_value = from->x[i][j].value;
                break;
            }
            ++j;
        }
        // update feautre of 'to' with 'from_value'
        ++count;
        int k = 0;
        while(to->x[i][k].index != -1) {
            if(to->x[i][k].index == feature) {
                to->x[i][j].value = from_value;
                break;
            }
            ++k;
        }
        if(from_value!=0. && to->x[i][k].index==-1) {
            struct svm_node *tmp = (struct svm_node*)realloc(to->x[i],sizeof(struct svm_node)*(k+2));
            if(tmp == NULL) {
                cerr << "Error: in 'copy_feature_value_from', out of memory!" << endl;
                exit(EXIT_FAILURE);
            }
            to->x[i] = tmp;
            to->x[i][k+1].index = -1;
            int m = k-1;
            while(m>=0 && to->x[i][m].index>feature) {
                to->x[i][m+1].index = to->x[i][m].index;
                to->x[i][m+1].value = to->x[i][m].value;
                --m;
            }
            to->x[i][m+1].index = feature;
            to->x[i][m+1].value = from_value;
        }
    }
    
    if(count == 0)
        cerr << "Warning: 'from' doesn't contain feature " << feature << endl;
}

struct svm_problem *set_feature_to_zero(const struct svm_problem *prob, int feature)
{
    struct svm_problem *new_prob = copy_svm_problem(prob);
    for(int i=0; i<new_prob->l; ++i) {
        int j = 0;
        while(new_prob->x[i][j].index != -1) {
            if(new_prob->x[i][j].index == feature) {
                new_prob->x[i][j].value = 0.;
                break;
            }
            ++j;
        }
    }
    return new_prob;
}

struct svm_problem *set_feature_to_mean(const struct svm_problem *prob,int feature)
{
    struct svm_problem *new_prob = copy_svm_problem(prob);
    double value = 0.;
    int *indices = (int*)malloc(sizeof(int)*(prob->l));
    if(indices == NULL) {
	cerr << "Error: in 'set_feature_to_mean' failed to allocate memory!" << endl;
	exit(EXIT_FAILURE);
    }
    for(int i=0; i<new_prob->l; ++i) {
	int j = 0;
	while(new_prob->x[i][j].index != -1) {
	    if(new_prob->x[i][j].index == feature) {
		value += new_prob->x[i][j].value;
		break;
	    }
	    ++j;
	}
	if(new_prob->x[i][j].index == -1)
	    indices[i] = -1;
	else
	    indices[i] = j;
    }
    value /= new_prob->l;
    for(int i=0; i<new_prob->l; ++i) {
	if(indices[i] != -1)
	    new_prob->x[i][indices[i]].value = value;
	else {
	    int k = 0;
	    while(new_prob->x[i][k].index != -1)
		++k;
	    struct svm_node *tmp = (struct svm_node*)realloc(new_prob->x[i],sizeof(struct svm_node)*(k+2));
	    if(tmp == NULL) {
		cerr << "Error: in 'set_feature_to_mean', out of memory!" << endl;
		exit(EXIT_FAILURE);
	    }
	    new_prob->x[i] = tmp;
	    new_prob->x[i][k+1].index = -1;
	    int m = k-1;
	    while(m>=0 && new_prob->x[i][m].index>feature) {
		new_prob->x[i][m+1].index = new_prob->x[i][m].index;
		new_prob->x[i][m+1].value = new_prob->x[i][m].value;
		--m;
	    }
	    new_prob->x[i][m+1].index = feature;
	    new_prob->x[i][m+1].value = value;
	}
    }
    free(indices);
    return new_prob;
}

struct svm_problem* shuffle_feature_of(const struct svm_problem *prob, int feature)
{
    vector<int> no_nodes(prob->l);
    vector<double> values(prob->l,0.);
    vector<int> index(prob->l,-1);
    for(int i=0; i<prob->l; ++i) {
        int j = 0;
        while(prob->x[i][j].index != -1) {
            if(prob->x[i][j].index == feature) {
                values[i] = prob->x[i][j].value;
                index[i] = j;
            }
            ++j;
        }
        if(index[i] != -1)
            no_nodes[i] = j+1;
        else
            no_nodes[i] = j+2;
    }
    // shuffle 'values' of 'feature'
    srand(time(NULL));
    for(int i=0; i<values.size(); ++i) {
        int rand_index = rand()%(values.size()-1);
        if(rand_index != i) {
            double temp = values[i];
            values[i] = values[rand_index];
            values[rand_index] = temp;
        }
    }
    struct svm_problem *new_prob = Malloc(struct svm_problem,1);
    new_prob->l = prob->l;
    new_prob->y = Malloc(double,prob->l);
    memcpy(new_prob->y,prob->y,sizeof(double)*(prob->l));
    new_prob->x = Malloc(struct svm_node*,prob->l);
    for(int i=0; i<new_prob->l; ++i) {
        if(index[i] != -1) {
            new_prob->x[i] = Malloc(struct svm_node,no_nodes[i]);
            if(new_prob->x[i] == NULL) {
                cerr << __FILE__ << "(line " << __LINE__ << "): out of memory!" << endl;
                free(new_prob->y);
                free(new_prob->x);
                free(new_prob);
                exit(EXIT_FAILURE);
            }
            for(int j=0; j<no_nodes[i]; ++j) {
                new_prob->x[i][j].index = prob->x[i][j].index;
                new_prob->x[i][j].value = prob->x[i][j].value;
            }
            new_prob->x[i][index[i]].value = values[i];
        }
        else {
            struct svm_node *temp = Malloc(struct svm_node,no_nodes[i]);
            if(temp == NULL) {
                cerr << __FILE__ << "(line " << __LINE__ << "): out of memory!" << endl;
                free(new_prob->y);
                free(new_prob->x);
                free(new_prob);
                exit(EXIT_FAILURE);
            }
            new_prob->x[i] = temp;
            new_prob->x[i][no_nodes[i]-1].index = -1;
            int j = no_nodes[i]-1;
            while(j>0 && prob->x[i][j-1].index>feature) {
                new_prob->x[i][j].index = prob->x[i][j-1].index;
                new_prob->x[i][j].value = prob->x[i][j-1].value;
                --j;
            }
            new_prob->x[i][j].index = feature;
            new_prob->x[i][j].value = values[i];
            --j;
            while(j >= 0) {
                new_prob->x[i][j].index = prob->x[i][j].index;
                new_prob->x[i][j].value = prob->x[i][j].value;
                --j;
            }
        }
    }
    return new_prob;
}

vector<int> svm_features(const struct svm_problem *prob)
{
    set<int> features;
    for(int i=0; i<prob->l; ++i) {
        int n(0);
        while(prob->x[i][n].index != -1)
            features.insert(prob->x[i][n++].index);
    }
    vector<int> vfeatures(features.begin(),features.end());
    return vfeatures;
}

vector<double> predict_svm_problem(const svm_model *model, const svm_problem *prob)
{
    vector<double> pred_result;
    for(int i=0; i<prob->l; ++i)
        pred_result.push_back(svm_predict(model,prob->x[i]));
    return pred_result;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
svm_parameter_construct::svm_parameter_construct()
{
    param = Malloc(struct svm_parameter, 1);
    param->svm_type     = C_SVC;  //-s
    param->kernel_type  = RBF;    //-t
    param->degree       = 3;      //-d
    param->gamma        = 0;      //-g, 1/nr_features
    param->coef0        = 0;      //-r
    param->nu           = 0.5;    //-n
    param->cache_size   = 100;    //-m
    param->C            = 1;      //-c
    param->eps          = 1e-3;   //-e
    param->p            = 0.1;    //-p
    param->shrinking    = 1;      //-h
    param->probability  = 0;      //-b
    param->nr_weight    = 0;      //-w
    param->weight_label = NULL;
    param->weight       = NULL;
}

void svm_parameter_construct::setSVMType(int type)
{
    switch(type){
        case 0: param->svm_type = C_SVC; break;
        case 3: param->svm_type = EPSILON_SVR; break;
        default: cerr << "invalid svm_type, available(0-C_SVC, 3-EPSILON_SVR)" << endl;free_svm_parameter(param);exit(1);
    }
}

void svm_parameter_construct::setKernelType(int type)
{
    switch(type) {
        case 0: param->kernel_type = LINEAR; break;
        case 1: param->kernel_type = POLY; break;
        case 2: param->kernel_type = RBF; break;
        case 3: param->kernel_type = SIGMOID; break;
        default: cerr << "invalid kernel_type, available(0-LINEAR, 1-POLY, 2-RBF, 3-SIGMOID)" << endl; free_svm_parameter(param); exit(1);
    }
}

void svm_parameter_construct::setIfProbability(int prob)
{
    if(prob!=0 && prob!=1) {
        cerr << "setIfProbability: only 0 or 1 allowed!!!" << endl;
        free_svm_parameter(param);
        exit(1);
    }
    param->probability = prob;
}

void svm_parameter_construct::setWeight(const int *label, const double *w, int num)
{
    param->nr_weight = num;
    param->weight_label = Malloc(int, num);
    param->weight = Malloc(double, num);
    for(int i=0; i<num; i++){
        param->weight_label[i] = label[i];
        param->weight[i] = w[i];
    }
}

void svm_parameter_construct::setWeight(const vector<int> &label, const vector<double> &weight)
{
    param->nr_weight = int(label.size());
    param->weight_label = Malloc(int, param->nr_weight);
    param->weight = Malloc(double, param->nr_weight);
    for(int i=0; i<param->nr_weight; i++) {
        param->weight_label[i] = label[i];
        param->weight[i] = weight[i];
    }
}

struct svm_parameter* svm_parameter_construct::copySVMParameter()
{
    struct svm_parameter *parameter = Malloc(struct svm_parameter, 1);
    parameter->svm_type    = param->svm_type;
    parameter->kernel_type = param->kernel_type;
    parameter->degree      = param->degree;
    parameter->gamma       = param->gamma;
    parameter->coef0       = param->coef0;
    parameter->nu          = param->nu;
    parameter->cache_size  = param->cache_size;
    parameter->C           = param->C;
    parameter->eps         = param->eps;
    parameter->p           = param->p;
    parameter->shrinking   = param->shrinking;
    parameter->probability = param->probability;
    parameter->nr_weight   = param->nr_weight;
    if(parameter->nr_weight)
    {
        parameter->weight_label = Malloc(int, parameter->nr_weight);
        parameter->weight = Malloc(double, parameter->nr_weight);
        for(int i=0; i<parameter->nr_weight; i++) {
            parameter->weight_label[i] = param->weight_label[i];
            parameter->weight[i] = param->weight[i];
        }
    }
    return parameter;
}

svm_parameter_construct::~svm_parameter_construct()
{
    free_svm_parameter(param);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


//stratified sampling
struct svm_problem* subset_svm_problem(struct svm_problem *prob, double proportion, 
        bool isClassify, struct svm_problem *remain)
{
    return subset_svm_problem(prob, int(prob->l*proportion), isClassify, remain);
}

struct svm_problem* subset_svm_problem(struct svm_problem *prob, int sample_size, 
        bool isClassify, struct svm_problem *remain)
{
    if(sample_size > prob->l){
        std::cerr << "ERROR: You are trying to sample " << sample_size << " instances, but data set only has " << prob->l << " instances!!!" << std::endl;
        exit(1);
    }

    bitset<SVM_BIT_SIZE> mark;
    //struct svm_problem *subProb = Malloc(struct svm_problem, 1);
    //subProb->l =sample_size;

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

    return subset_svm_problem(prob, mark, remain); 
}

struct svm_problem* subset_svm_problem(struct svm_problem *prob, 
        bitset<SVM_BIT_SIZE> &sample_mark, struct svm_problem *remain)
{
    if(sample_mark.size() < (unsigned)prob->l) {
        cerr << "Warrning: `SVM_BIT_SIZE` is too small to represent the whole dataset!"
            << "You need to redefine `SVM_BIT_SIZE` at the very begining "
            << "of your header file(or source file if no header file)!!!" << endl;
        return NULL;
    }
    struct svm_problem *prob1 = Malloc(struct svm_problem, 1);
    prob1->l = sample_mark.count();
    prob1->y = Malloc(double, prob1->l);
    prob1->x = Malloc(struct svm_node *, prob1->l);

    if(remain != NULL)
    {
        if(remain->l && remain->y!=NULL) {
            free(remain->y);
            for(int i=0; i<remain->l; i++)
                free(remain->x[i]);
        }
        remain->l = prob->l - prob1->l;
        remain->y = Malloc(double, remain->l);
        remain->x = Malloc(struct svm_node *, remain->l);
    }

    /*
    for(int i=0; i<prob->l; i++)
        std::cout << sample_mark[i];
    std::cout << std::endl;
    */

    int m_pick = 0, m_remain=0;
    for(int i=0; i<prob->l; i++)
    {
        if(sample_mark.test(i))
        {
            //to copy prob->y[i]
            prob1->y[m_pick] = prob->y[i];
            //to count number of features prob->x[i] has.
            int n_features = 0, n=0;
            while(prob->x[i][n].index != -1) {
                ++n_features;
                ++n;
            }
            ++n_features;
            //to copy prob->x[i]
            prob1->x[m_pick] = Malloc(struct svm_node, n_features);
            for(int j=0; j<n_features; j++) {
                prob1->x[m_pick][j].index = prob->x[i][j].index;
                prob1->x[m_pick][j].value = prob->x[i][j].value;
            }
            ++m_pick;
        }
        else if(remain != NULL)
        {
            remain->y[m_remain] = prob->y[i];
            int n_features=0, n=0;
            while(prob->x[i][n].index != -1) {
                ++n_features;
                ++n;
            }
            ++n_features;
            remain->x[m_remain] = Malloc(struct svm_node, n_features);
            for(int j=0; j<n_features; j++) {
                remain->x[m_remain][j].index = prob->x[i][j].index;
                remain->x[m_remain][j].value = prob->x[i][j].value;
            }
            ++m_remain;
        }
    }

    return prob1;
}

struct svm_problem* rm_feature(const struct svm_problem *prob, int id)
{
    struct svm_problem* new_prob = Malloc(struct svm_problem,1);
    new_prob->l = prob->l;
    new_prob->y = Malloc(double,new_prob->l);
    for(int i=0; i<new_prob->l; ++i)
        new_prob->y[i] = prob->y[i];
    new_prob->x = Malloc(struct svm_node *, new_prob->l);
    for(int i=0; i<new_prob->l; ++i) {
        int count_nodes(0), n(0), m(0);
        while(prob->x[i][n].index != -1) {
            if(prob->x[i][n++].index != id)
                ++count_nodes;
        }
        new_prob->x[i] = Malloc(struct svm_node, count_nodes+1);
        n = 0;
        do {
            if(prob->x[i][n].index != id) {
                new_prob->x[i][m].index = prob->x[i][n].index;
                new_prob->x[i][m].value = prob->x[i][n].value;
                ++m;
            }
        } while(prob->x[i][n++].index != -1);
    }
    return new_prob;
}

struct svm_problem* pick_features(struct svm_problem *prob, 
        bitset<SVM_BIT_SIZE> &mark, bool pick)
{
    struct svm_problem *new_prob = Malloc(struct svm_problem, 1);
    new_prob->l = prob->l;
    new_prob->y = Malloc(double, new_prob->l);
    for(int i=0; i<new_prob->l; ++i)
        new_prob->y[i] = prob->y[i];
    // pick features
    new_prob->x = Malloc(struct svm_node *, new_prob->l);
    for(int i=0; i<new_prob->l; ++i) {
        int count_nodes = 0, n=0, m=0;
        while(prob->x[i][n].index != -1) {
            if(mark.test(prob->x[i][n].index-1)) {
                if(pick) ++count_nodes;
            }
            else {
                if(!pick) ++count_nodes;
            }
            ++n;
        }
        ++count_nodes;
        new_prob->x[i] = Malloc(struct svm_node, count_nodes);
        n = 0;
        while(prob->x[i][n].index != -1) {
            if(mark.test(prob->x[i][n].index-1)) {
                if(pick) {
                    new_prob->x[i][m].index = prob->x[i][n].index;
                    new_prob->x[i][m++].value = prob->x[i][n].value;
                }
            }
            else {
                if(!pick) {
                    new_prob->x[i][m].index = prob->x[i][n].index;
                    new_prob->x[i][m++].value = prob->x[i][n].value;
                }
            }
            ++n;
        }
        new_prob->x[i][m].index = -1;
    }

    return new_prob;
}



