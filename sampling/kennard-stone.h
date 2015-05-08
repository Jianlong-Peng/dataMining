/*
 * name: kennard-stone.hpp
 * OBJ: implemention of kennard-stone algorithm for sampling
 * algorithm:
 *   1. two choices:
 *      a. find the point(p1) furthest to the center, and point(p2) furthest to p1
 *      b. find the two points(p1,p2) most furthest from each other in the training set. 
 *   2. For each candidate point, find the smallest distance to any object already selected. 
 *      (let this distance be d_si)
 *   3. pick out the point which has the largest `d_si` as new training member!
 * author: jlpeng
 * create: 2012-07-11
 * update: 2012-08-11
 * dependence:
 *   <data/dataTableSP.h>
 *     <stringUtils/stringUtils.hpp>
 *     <SVM/svmTool.hpp>
 *   <statistics/similarity.hpp>
 * Usage:
 *   KennardStone ks(infile, sep);
 *   ks.pickSample(num, out_pick_file, out_remain_out_file);
 */
#ifndef KENNARD_STONE_HPP
#define KENNARD_STONE_HPP

#include <data/dataTableSP.hpp>

class KennardStone
{
public:
    //if data is from libsvm, then set `sep` to "libsvm"
    KennardStone(const char *infile, const char *sep=" "): dataset(infile, sep),distance(NULL),sep(sep) {}

    //calculate euclidean distance. Each pair of points is calculated only once.
    void calcEuclideanDistance();
    //to pick `num` points according to kennard-stone algorithm
    //picked instances will be saved to `pickOutFile`
    //if `remainOutFile` is given, the remaining instances will be saved
    //information about which points are selected will be displayed in the screen.
    void pickSample(int num, const char *pickOutFile, const char *remainOutFile=NULL, int method=1);

    void saveDistance(const char *outfile);
    inline int numOfInstances() {return dataset.numOfInstances();}
    ~KennardStone() {freeDistance();}
private:
    //NO default and copy constructor allowed!!!!
    KennardStone(){}
    KennardStone(KennardStone &ks){}
    //
    Data<double,string> dataset;
    //It's an upper triangular matrix
    //distance[i][j-i-1] --> distance between i and j
    //distance[m][n]   ---> distance between m and m+1+n (m<n)
    double **distance;
    //initial two points
    int initPoints[2];
    //separator
    string sep;
    //private method
    void freeDistance();
    void saveResult(std::set<int> &ids, const char *outfile);
    void findInitPoints(); //only used when `method == 2`
};

#endif
