/*
 * name: kennard-stone.cpp
 * OBJ: definition of `class KennardStone` declared in `kennard-stone.hpp`
 * author: jlpeng
 * create: 2012-07-11
 * update: 2012-8-7
 * dependence:
 *   <statistics/similarity.hpp>
 *   <data/dataTableSP.h>
 *     <SVM/svmTool.hpp>
 *     <stringUtils/stringUtils.hpp>
 */

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <set>
#include <list>
#include <sampling/kennard-stone.h>
#include <statistics/similarity.hpp>

using std::cout;
using std::cerr;
using std::endl;
using std::ofstream;
using std::vector;
using std::set;
using std::list;

void KennardStone::calcEuclideanDistance()
{
    distance = (double **)malloc(sizeof(double *)*(dataset.numOfInstances()-1));
    double max_distance = -1.0;
    int max_i=dataset.numOfInstances(), max_j=dataset.numOfInstances();
    for(int i=0; i<dataset.numOfInstances()-1; i++)
    {
        int num = dataset.numOfInstances()-1-i;
        distance[i] = (double *)malloc(sizeof(double)*num);
        for(int j=0; j<num; j++) {
            distance[i][j] = euclidean(dataset.getRecordOf(i), dataset.getRecordOf(i+1+j), dataset.numOfFeatures());
            if(distance[i][j] > max_distance) {
                max_distance = distance[i][j];
                max_i = i;
                max_j = i+1+j;
            }
        }
    }
    initPoints[0] = max_i;
    initPoints[1] = max_j;
}

void KennardStone::findInitPoints()
{
    //find the center
    double *center = new double[dataset.numOfFeatures()];
    //vector<double> center;
    cout << "center:";
    for(int j=0; j<dataset.numOfFeatures(); j++) {
        double value = 0.;
        for(int i=0; i<dataset.numOfInstances(); i++)
            value += dataset(i,j);
            //value += dataset[i].xValue[j];
        value = value/dataset.numOfInstances();
        center[j] = value;
        //center.push_back(value);
        cout << " " << value;
    }
    //find the point furthest to center
    double maxDist = -1.0, temp_dist;
    int p1 = dataset.numOfInstances();
    for(int i=0; i<dataset.numOfInstances(); i++) {
        temp_dist = euclidean(dataset.getRecordOf(i), center, dataset.numOfFeatures());
        if(temp_dist > maxDist) {
            maxDist = temp_dist;
            p1 = i;
        }
    }
    cout << endl << "initial item furthest to the center: " << p1+1 << endl;
    //find the point furthest to point `p1`
    //distance[i][p1-i-1] (i belongs to [0,p1))
    //distance[p1][j] (j belongs to [0,dataset.numOfInstances()-1-p1))
    maxDist = -1.;
    int p2 = dataset.numOfInstances();
    for(int i=0; i<p1; i++) {
        if(distance[i][p1-i-1] > maxDist) {
            maxDist = distance[i][p1-i-1];
            p2 = i;
        }
    }
    for(int j=0; j<dataset.numOfInstances()-1-p1; j++) {
        if(distance[p1][j] > maxDist) {
            maxDist = distance[p1][j];
            p2 = j;
        }
    }
    cout << "furthest item from the initial point: " << p2+1 << endl;

    initPoints[0] = p1;
    initPoints[1] = p2;

    delete [] center;
}

void KennardStone::pickSample(int num, const char *pickOutFile, const char *remainOutFile, int method)
{
    if((num <= 0) || (num > dataset.numOfInstances())) {
        cerr << "You are trying to pick " << num << " instances from dataset with size " << dataset.numOfInstances() << endl;
        freeDistance();
        exit(EXIT_FAILURE);
    }

    if(distance == NULL)
        calcEuclideanDistance();

    if(method == 1)
        findInitPoints();
    else if(method == 2) {
        cout << "pair of instances with greatest distance: " << initPoints[0]+1 << " " << initPoints[1]+1 << endl;
        cout << "ID(begin with 1) of instances being picked" << endl;
    }
    else {
        cerr << "method should be either 1 or 2, but " << method << " given!" << endl;
        exit(EXIT_FAILURE);
    }

    list<int> picked;
    list<int> remain;
    for(int i=0; i<dataset.numOfInstances(); i++) {
        if(i==initPoints[0] || i==initPoints[1])
            picked.push_back(i);
        else
            remain.push_back(i);
    }
    
    while((int)picked.size() < num)
    {
        double greatest = -1.0;
        list<int>::iterator pickId;
        for(list<int>::iterator iter_remain=remain.begin(); iter_remain!=remain.end(); iter_remain++)
        {
            //to find the smallest distance between *iter_remain and points already picked
            double smallest = 1e8;
            for(list<int>::iterator iter_picked=picked.begin(); iter_picked!=picked.end(); iter_picked++)
            {
                if(*iter_remain < *iter_picked) {
                    int j = *iter_picked - *iter_remain - 1;
                    if(distance[*iter_remain][j] < smallest)
                        smallest = distance[*iter_remain][j];
                }
                else {
                    int j = *iter_remain - *iter_picked - 1;
                    if(distance[*iter_picked][j] < smallest)
                        smallest = distance[*iter_picked][j];
                }
            }
            //to get the pair of points having greatest `smallest` distance.
            if(smallest > greatest) {
                greatest = smallest;
                pickId = iter_remain;
            }
        }
        //update `picked` and `remain`
        cout << *pickId+1 << " (" << greatest << ")" << endl;
        picked.push_back(*pickId);
        remain.erase(pickId);
    }

    //to sort ID numbers
    set<int> picked_set(picked.begin(),picked.end());
    set<int> remain_set(remain.begin(),remain.end());
    cout << endl << "The following lines are picked and saved in " << pickOutFile << endl;
    for(set<int>::iterator i=picked_set.begin(); i!=picked_set.end(); ++i)
        cout << *i+1 << " ";
    cout << endl << endl;
    //output picked and remaining(if `remainOutFile` is given) instances
    saveResult(picked_set, pickOutFile);
    if(remainOutFile != NULL)
        saveResult(remain_set, remainOutFile);
 
}

void KennardStone::saveDistance(const char *outfile)
{
    if(distance == NULL) {
        cerr << "Sorry, but `calcEuclideanDistance` should be called first!!" << endl;
        exit(EXIT_FAILURE);
    }
    ofstream outf(outfile);
    for(int i=0; i<dataset.numOfInstances()-1; i++)
    {
        int num = dataset.numOfInstances()-1-i;
        for(int j=0; j<num; j++)
            outf << distance[i][j] << " ";
        outf << endl;
    }
    outf << "============================" << endl;
    outf << "It's an upper triangular matrix" << endl;
    outf << "distance[i][j]:   distance between i and i+j  (where i,j begin with 1, and i<j)" << endl;
    outf.close();

}
void KennardStone::saveResult(set<int> &ids, const char *outfile)
{    
    ofstream outf(outfile);
    if(sep == "libsvm")
    {
        for(set<int>::const_iterator iter=ids.begin(); iter!=ids.end(); iter++)
        {
            outf << dataset[*iter];
            for(int j=0; j<dataset.numOfFeatures(); ++j) {
                //if(dataset(*iter,j) <= 1e-99)
                //    continue;
                outf << " " << dataset.getFeatureNameOf(j) << ":" << std::setprecision(6) 
                    << dataset(*iter,j);
            }
            outf << endl;
        }
    }
    else
    {
        outf << "labels";
        for(int i=0; i<dataset.numOfFeatures(); i++)
            outf << sep << dataset.getFeatureNameOf(i);
        outf << endl;
        for(set<int>::const_iterator iter=ids.begin(); iter!=ids.end(); iter++)
        {
            outf << dataset[*iter];
            for(int j=0; j<dataset.numOfFeatures(); ++j)
                outf << sep << std::setprecision(6) << std::setiosflags(std::ios::scientific) 
                    << dataset(*iter,j);
            outf << endl;
        }
    }
    outf.close();
}

void KennardStone::freeDistance()
{
    if(distance != NULL)
    {
        for(int i=0; i<dataset.numOfInstances()-1;i++)
            free(distance[i]);
        free(distance);
    }
}


