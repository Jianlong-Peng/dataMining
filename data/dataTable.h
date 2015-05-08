/*=============================================================================
#     FileName: dataTable.h
#         Desc: declaration of template class Data
#               It'll not be maintained any more !!!!!!
#       Author: jlpeng
#        Email: jlpeng1201@gmail.com
#     HomePage: 
#      Version: 0.0.1
#      Created: 2012-08-04 12:02:18
#   LastChange: 2012-11-01 15:00:17
#      History: 1. both declaration and definition are in the same file
=============================================================================*/


#ifndef  DATATABLE_H
#define  DATATABLE_H

#include <iostream>
//#include <assert.h>
#include <vector>
#include <string>
#include <stdexcept>
#include <set>
#include <fstream>
#include <sstream>
#include <cstddef>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <stringUtils/stringUtils.hpp>  //method `split`
#include <SVM/svmTool.hpp>  //method `readline`
using std::ifstream;
using std::istringstream;
using std::vector;
using std::string;
using std::set;

/*
 * here, I suppose that all independent variables have same data type!
 */
template <typename xtype, typename ytype>
struct Record
{
    vector<xtype> xValue;
    ytype yValue;
};

template <typename xtype, typename ytype>
class Data
{
public:
    explicit Data(bool isClassify=true):isClassify(isClassify){}
    Data(const char *infile, const char *sep=" ", bool isClassify=true):isClassify(isClassify){
        if(strcmp(sep,"libsvm") == 0)
            readLIBSVM(infile);
        else
            readFile(infile, sep);
    }
    //copy constructor
    Data(Data<xtype, ytype> &data2);

    //to count number of instances
    std::size_t numOfInstances() {return dataset.size();}
    //to count number of features
    std::size_t numOfFeatures() {return featureNames.size();}
    //to get all features' name
    vector<string> &getFeatureNames() {return featureNames;}
    //to get ith feature's name
    string getFeatureNameOf(unsigned int i) {
        if(numOfFeatures() == 0) {
            std::cerr << "There is no feature any more!!!!" << std::endl;
            return "";
        }
        if(i<numOfFeatures()) return featureNames[i];
        else throw std::out_of_range("(featureName)index out of range!!");
    }
    //If isClassify is true, unique class labels will be returned, else empty set returned.
    set<ytype> getUniqueY();

    //to get ith record
    Record<xtype, ytype> &operator[](unsigned int i) {
        if(i<dataset.size()) return dataset[i];
        else throw std::out_of_range("(dataset)index out of range!!");
    }

    //assignment operator
    Data<xtype,ytype> &operator=(const Data<xtype,ytype> &data2);

    /*
     * by default, I suppose that file has the following structure
     *   1st line: feature names
     *   1st column: label/y
     *   values are separated by seperator specified by `sep`
     */
    void readFile(const char *infile, const char *sep);
    /*
     * an alternative way to store libsvm dataset
     * called when `sep == "libsvm"`
     */
    void readLIBSVM(const char *infile);

private:
    vector<Record<xtype,ytype> > dataset;
    vector<string> featureNames;
    bool isClassify;
};

#include "dataTable.cpp"
#endif   /* ----- #ifndef DATATABLE_H  ----- */
