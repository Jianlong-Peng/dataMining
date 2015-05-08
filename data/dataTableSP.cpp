/*=============================================================================
#     FileName: dataTableSP.cpp
#         Desc: definition of template classes declared in `dataTableSP.h`
#       Author: jlpeng
#        Email: jlpeng1201@gmail.com
#     HomePage: 
#      Version: 0.0.1
#      Created: 2012-08-07 10:00:34
#   LastChange: 2012-08-07 13:59:28
#      History:
=============================================================================*/

#include "dataTableSP.hpp"

template <typename xtype, typename ytype>
RecordSP<xtype,ytype>& RecordSP<xtype,ytype>::operator=(const RecordSP<xtype,ytype> &rhs)
{
    ++rhs.ptr->use;
    if(--ptr->use == 0)
        delete ptr;

    ptr = rhs.ptr;
    return *this;
}

template <typename xtype, typename ytype>
void RecordSP<xtype, ytype>::allocMem(int m, int n)
{
    if(ptr->nrows==0 && ptr->ncols==0) {
        ptr->nrows = m;
        ptr->ncols = n;
        ptr->xValue = new xtype*[m];
        ptr->yValue = new ytype[m];
        assert(ptr->xValue && ptr->yValue);
        for(int i=0; i<m; ++i)
            ptr->xValue[i] = new xtype[n];
    }
}

//ptr->xValue[i][j]
template <typename xtype, typename ytype>
xtype& RecordSP<xtype,ytype>::operator()(int i, int j)
{
    if(i<0 || j<0 || i>ptr->nrows || j>ptr->ncols)
        throw out_of_range("(xValue(i,j))index out of range!!!");
    return ptr->xValue[i][j];
}
template <typename xtype, typename ytype>
const xtype& RecordSP<xtype,ytype>::operator()(int i, int j) const
{
    if(i<0 || j<0 || i>ptr->nrows || j>ptr->ncols)
        throw out_of_range("(xValue(i,j))index out of range!!!");
    return ptr->xValue[i][j];
}
template <typename xtype, typename ytype>
const xtype* RecordSP<xtype,ytype>::getRecordOf(int i) const
{
    if(i<0 || i>ptr->nrows)
        throw out_of_range("(getRecordOf)index out of range");
    return ptr->xValue[i];
}
//ptr->yValue[i]
template <typename xtype, typename ytype>
ytype& RecordSP<xtype,ytype>::operator[](int i)
{
    if(i<0 || i>ptr->nrows)
        throw out_of_range("(yValue[i])index out of range!!");
    return ptr->yValue[i];
}
template <typename xtype, typename ytype>
const ytype& RecordSP<xtype,ytype>::operator[](int i) const
{
    if(i<0 || i>ptr->nrows)
        throw out_of_range("(yValue[i])index out of range!!");
    return ptr->yValue[i];
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
//assignment
template <typename xtype, typename ytype>
Data<xtype,ytype>& Data<xtype,ytype>::operator=(const Data<xtype, ytype> &data2)
{
    if(this == &data2)
        return *this;
    else
    {
        isClassify = data2.isClassify;
        featureNames = data2.featureNames;
        dataset = data2.dataset;
        return *this;
    }
}

//to read dataset
template <typename xtype, typename ytype>
void Data<xtype,ytype>::readFile(const char *infile, const char *sep)
{
    ifstream inf(infile);
    if(!inf) {
        std::cerr << "IOError: unable to open file " << infile << std::endl;
        exit(EXIT_FAILURE);
    }
    string line;
    //get first line - features' name
    getline(inf, line);
    featureNames = split(line, sep);
    if(line.find(sep) != 0)  //only if first line not begin with `sep`
        featureNames.erase(featureNames.begin());
    assert(featureNames.size());
    //to get number of rows, that is number of records
    int numRows(0);
    while(getline(inf,line))
        ++numRows;
    dataset.allocMem(numRows, featureNames.size());
    //go back to the begining of the stream buffer
    inf.close();
    inf.open(infile);
    getline(inf,line);
    //get remaining data values
    int i(0);
    while(getline(inf, line))
    {
        vector<string> line_split = split(line, sep);
        if(line_split.size()-1 != featureNames.size()) {
            std::cerr << "Error: It has " << featureNames.size() << " features, but line " << i+1
                << " has " << line_split.size()-1 << " records!!!!" << std::endl;
            inf.close();
            exit(EXIT_FAILURE);
        }
        istringstream istream(line_split[0]);
        istream >> dataset[i];  //yValue[i]
        for(vector<string>::size_type j=1; j<line_split.size(); j++) {
            istringstream istream(line_split[j]);
            istream >> dataset(i,j-1);
        }
        ++i;
    }
    inf.close();
}

//to read LIBSVM file
template <typename xtype, typename ytype>
void Data<xtype,ytype>::readLIBSVM(const char *infile)
{
    FILE *fp = fopen(infile,"r");
    if(fp == NULL)
    {
        std::cerr << "can't open input file " << infile << std::endl;
        exit(EXIT_FAILURE);
    }

    int max_features=0, nr_features=0, num_lines=0;
    char *line, *p, *idx;

    //to get number of features
    while((line=readline(fp))!=NULL)
    {
        ++num_lines;
        p = strtok(line," \t"); // label

        // features
        while(1)
        {
            idx = strtok(NULL, ":");
            p = strtok(NULL," \t");
            if(p == NULL || *p == '\n') // check '\n' as ' ' may be after the last feature
                break;
            nr_features = atoi(idx);
        }
        if(nr_features > max_features)
            max_features = nr_features;
    }
    rewind(fp);

    //feature names
    char labels[10];
    for(int i=1; i<=max_features; i++) {
        sprintf(labels,"%d",i);
        featureNames.push_back(labels);
    }

    dataset.allocMem(num_lines,max_features);
    //to read and store dataset
    int i(0);
    while((line=readline(fp)) != NULL)
    {
        p = strtok(line, " \t");  //label
        istringstream itemp(p);
        itemp >> dataset[i];  //get yValue
        int j = 1;
        while(1)
        {
            idx = strtok(NULL, ":");
            p = strtok(NULL, " \t");
            if(p == NULL || *p == '\n')
                break;

            int index = atoi(idx);
            //to replace missing features with 0
            while(j < index) {
                dataset(i, j-1) = 0;
                ++j;
            }
            //to add ith feature's value
            istringstream itemp(p);
            itemp >> dataset(i, j-1);
            ++j;
        }
        //to replace missing features with 0
        while(j <= max_features) {
            dataset(i,j-1) = 0;
            //each_record.xValue.push_back(0);
            ++j;
        }
        ++i;
    }
    fclose(fp);
}

template <typename xtype, typename ytype>
set<ytype> Data<xtype,ytype>::getUniqueY()
{
    set<ytype> uniqueY;
    if(isClassify) {
        for(int i=0; i<dataset.nrows(); ++i)
            uniqueY.insert(dataset[i]);
    }
    return uniqueY;
}

