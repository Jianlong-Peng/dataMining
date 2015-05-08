/*=============================================================================
#     FileName: dataTableSP.h
#         Desc: template class Data. Similar to class declared in `dataTable.h`
#               except that here we use smart pointer instead.
#       Author: jlpeng
#        Email: jlpeng1201@gmail.com
#     HomePage: 
#      Version: 0.0.1
#      Created: 2012-08-07 09:24:18
#   LastChange: 2012-11-01 14:59:53
#      History: 
=============================================================================*/


#ifndef  DATATABLESP_H
#define  DATATABLESP_H

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
#include <cassert>
#include <stringUtils/stringUtils.hpp>  //method `split`
#include <SVM/svmTool.h>  //method `readline`
using std::ifstream;
using std::istringstream;
using std::vector;
using std::string;
using std::set;
using std::out_of_range;

template <typename xtype, typename ytype> class RecordSP;

/*
 * here, I suppose that all independent variables have same data type!
 */
template <typename xtype, typename ytype>
class Record
{
    friend class RecordSP<xtype,ytype>;
    Record():xValue(0),yValue(0),nrows(0),ncols(0),use(1) {}
    Record(int rows, int cols):nrows(rows),ncols(cols),use(1) {
        assert(rows>0 && cols>0);
        xValue = new xtype*[nrows];
        for(int i=0; i<nrows; ++i)
            xValue[i] = new xtype[ncols];
        yValue = new ytype[nrows];
    }
    ~Record() {
        if(nrows) {
            delete [] yValue;
            if(ncols) {
                for(int i=0; i<nrows; ++i)
                    delete [] xValue[i];
            }
            delete [] xValue;
        }
    }
    xtype **xValue;
    ytype *yValue;
    int nrows,ncols;  //nrows=len(yValue), ncols=len(xValue[i])
    std::size_t use;
};

template <typename xtype, typename ytype>
class RecordSP
{
public:
    RecordSP(): ptr(new Record<xtype,ytype>()) {}
    RecordSP(int rows, int cols): ptr(new Record<xtype,ytype>(rows,cols)) {}

    RecordSP(const RecordSP<xtype,ytype> &orig): ptr(orig.ptr) {++ptr->use;}
    RecordSP<xtype,ytype>& operator=(const RecordSP<xtype,ytype> &rhs);

    //allocate memory
    void allocMem(int m, int n);       //only used when instance is created using default constructor
    //xValue[i][j]
    xtype& operator()(int i, int j);
    const xtype& operator()(int i, int j) const;
    const xtype* getRecordOf(int i) const;
    //yValue[i]
    ytype& operator[](int i);
    const ytype& operator[](int i) const;
    //number of rows and cols
    inline int nrows() const {return ptr->nrows;}
    inline int ncols() const {return ptr->ncols;}

    ~RecordSP() {if(--ptr->use == 0) delete ptr;}
    
private:
    Record<xtype,ytype> *ptr;
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
    Data(Data<xtype, ytype> &data2): isClassify(data2.isClassify),featureNames(data2.featureNames),dataset(data2.dataset) {}

    //to count number of instances
    int numOfInstances() {return dataset.nrows();}
    //to count number of features
    int numOfFeatures() {return featureNames.size();}
    //to get all features' name
    vector<string> &getFeatureNames() {return featureNames;}
    //to get ith feature's name
    string getFeatureNameOf(int i) {
        if(i<0 || i>(int)featureNames.size())
            throw out_of_range("(getFeatureNameOf)out of range!!!!");
        else
            return featureNames[i];
    }
    //If isClassify is true, unique class labels will be returned, else empty set returned.
    set<ytype> getUniqueY();

    //to get values!!!
    // - xValue
    xtype& operator()(int i, int j) {return dataset(i,j);}
    const xtype& operator()(int i, int j) const {return dataset(i,j);}
    const xtype* getRecordOf(int i) const {return dataset.getRecordOf(i);}
    // - yValue
    ytype& operator[](int i) {return dataset[i];}
    const ytype& operator[](int i) const {return dataset[i];}

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
    //vector<Record<xtype,ytype> > dataset;
    RecordSP<xtype,ytype> dataset;
    vector<string> featureNames;
    bool isClassify;
};

#include "dataTableSP.cpp"

#endif   /* ----- #ifndef DATATABLESP_H  ----- */
