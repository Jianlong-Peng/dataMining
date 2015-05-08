/*=============================================================================
#     FileName: dataTable.cpp
#         Desc: definition of template class `Data`
#       Author: jlpeng
#        Email: jlpeng1201@gmail.com
#     HomePage: 
#      Version: 0.0.1
#      Created: 2012-08-04 12:04:50
#   LastChange: 2012-08-07 10:36:09
#      History:
=============================================================================*/

#include "dataTable.h"

//copy constructor
template <typename xtype, typename ytype>
Data<xtype,ytype>::Data(Data<xtype, ytype> &data2)
{
    isClassify = data2.isClassify;
    featureNames = data2.featureNames;
    dataset = data2.dataset;
}

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
    /*
    getline(inf, line);
    istringstream names(line);
    string name;
    names >> name;  //ignore 1st item
    while(names >> name)
        featureNames.push_back(name);
    */
    int line_no = 1;
    //get remaining data values
    while(getline(inf, line))
    {
        vector<string> line_split = split(line, sep);
        if(line_split.size()-1 != featureNames.size()) {
            std::cerr << "Error: It has " << featureNames.size() << " features, but line " << line_no
                << " has " << line_split.size()-1 << " records!!!!" << std::endl;
            inf.close();
            exit(EXIT_FAILURE);
        }
        Record<xtype, ytype> each_record;
        istringstream istream(line_split[0]);
        istream >> each_record.yValue;
        for(vector<string>::size_type i=1; i<line_split.size(); i++) {
            istringstream istream(line_split[i]);
            xtype _temp;
            istream >> _temp;
            each_record.xValue.push_back(_temp);
        }
        dataset.push_back(each_record);
        ++line_no;
        /*
        Record<xtype, ytype> each_record;
        istringstream istream(line);
        istream >> yv;
        each_record.yValue = yv;
        while(istream >> xv)
            each_record.xValue.push_back(xv);
        assert(each_record.xValue.size() == featureNames.size());
        dataset.push_back(each_record);
        */
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

    int max_features=0, nr_features=0;
    char *line, *p, *idx;

    //to get number of features
    while((line=readline(fp))!=NULL)
    {
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

    //to read and store dataset
    while((line=readline(fp)) != NULL)
    {
        Record<xtype,ytype> each_record;
        p = strtok(line, " \t");  //label
        istringstream itemp(p);
        itemp >> each_record.yValue;
        int i = 1;
        while(1)
        {
            idx = strtok(NULL, ":");
            p = strtok(NULL, " \t");
            if(p == NULL || *p == '\n')
                break;

            int index = atoi(idx);
            //to replace missing features with 0
            while(i < index) {
                each_record.xValue.push_back(0);
                ++i;
            }
            //to add ith feature's value
            istringstream itemp(p);
            xtype x_temp;
            itemp >> x_temp;
            each_record.xValue.push_back(x_temp);
            ++i;
        }
        //to replace missing features with 0
        while(i < max_features) {
            each_record.xValue.push_back(0);
            ++i;
        }
        //assert(each_record.xValue.size() == featureNames.size());
        dataset.push_back(each_record);
    }
    fclose(fp);

}

template <typename xtype, typename ytype>
set<ytype> Data<xtype,ytype>::getUniqueY()
{
    set<ytype> uniqueY;
    if(isClassify) {
        for(typename vector<Record<xtype, ytype> >::const_iterator iter=dataset.begin(); iter!=dataset.end(); iter++)
            uniqueY.insert(iter->yValue);
    }
    return uniqueY;
}


