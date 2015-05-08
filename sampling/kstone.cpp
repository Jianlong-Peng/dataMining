/*
 * name: kstone.cpp
 * OBJ: smapling using kennard-stone algorithm
 * author: jlpeng
 * create: 2012-07-12
 * update: 2012-8-7
 * dependence:
 *   <sampling/kennard-stone.hpp>
 *     <statistics/similarity.hpp>
 *     <data/dataTableSP.h>
 *       <SVM/svmTool.hpp>
 *       <stringUtils/stringUtils.hpp>
 */

#include "kennard-stone.h"
#include <iostream>
#include <cstdlib>

using namespace std;

void exit_with_help()
{
    cerr << endl << "  Usage: kstone.exe [options] infile" << endl
        << "  [options]" << endl
        << "  -p pick_out_file:" << endl
        << "     where to save picked records" << endl
        << "  -r remain_out_file: (default: do not save remaining records)" << endl
        << "     where to save remaining records" << endl
        << "  -d delim: (default: <space>)" << endl
        << "     to specify the separator." << endl
        << "     `-d libsvm` ===> infile is a libsvm file" << endl
        << "  -n number: to specify how many records needed to be sampled" << endl
        << "  -f fraction: to specify the fraction of records to be sampled" << endl
        << "  (only one of `-n`, `-f` is allowed)" << endl
        << "  -m method: method to choose initial point(s) (default: 1)" << endl
        << "    1 - choose the initial as only 1 item that is furthest to the center" << endl
        << "    2 - choose the initial as 2 items that are furthest to each other" << endl
        << "    (these two methods give the same result! why?)" << endl << endl;
    exit(EXIT_FAILURE);
}
int main(int argc, char *argv[])
{
    if(argc <= 2)
        exit_with_help();

    char *out_pick=NULL, *out_remain=NULL;
    char *delim = (char*)malloc(sizeof(char)*10);
    int nr=0, i=1, method=1, num;
    double frac=0.;
    //default separator
    delim[0] = ' ';
    delim[1] = '\0';
    //parse arguments
    while(true)
    {
        if(argv[i][0] != '-')
            break;
        switch(argv[i][1])
        {
            case 'p': out_pick=argv[i+1]; break;
            case 'r': out_remain=argv[i+1]; break;
            case 'd': strcpy(delim,argv[i+1]); break;
            case 'n': nr=atoi(argv[i+1]); break;
            case 'f': frac=atof(argv[i+1]); break;
            case 'm': method=atoi(argv[i+1]);break;
            default: cerr << "invalid option: " << argv[i] << endl; exit_with_help();
        }
        i += 2;
    }
    if(i != argc-1)
        exit_with_help();
    
    if(out_pick == NULL) {
        cerr << "Error: `-p pick_out_file` should be passed" << endl;
        exit_with_help();
    }
    if(!((nr==0 && frac!=0.) || (nr!=0 && frac==0.))) {
        cerr << "Error: inappropriate `-n -f` given !!!!!" << endl;
        exit_with_help();
    }
    if(frac<0. || frac>=1.) {
        cerr << "Error: `-f " << frac << "` is passed, but fraction should be among (0.,1.)" << endl;
        exit_with_help();
    }
    if(method!=1 && method!=2) {
        cerr << "Error: `-m 1` or `-m 2`, but `-m " << method << "` passed!!!" << endl;
        exit_with_help();
    }

    KennardStone ks(argv[i], delim);
    if(frac != 0.)
        num = (int)(ks.numOfInstances() * frac);
    else
        num = nr;
    ks.pickSample(num, out_pick, out_remain, method);

    return 0;
}



