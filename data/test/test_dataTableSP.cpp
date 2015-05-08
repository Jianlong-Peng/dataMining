#include <iostream>
#include <cstdlib>
#include <vector>
#include <set>
#include <string>
#include <data/dataTableSP.h>

using namespace std;

typedef int YTYPE;

int main(int argc, char *argv[])
{
	if(argc!=2 && argc!=3) {
		cerr << "usage: test_dataTable.exe infile [sep]" << endl;
		cerr << "[sep]: to specify the separator <default: space>" << endl;
		return -1;
	}
	
	Data<double, YTYPE> data(true);
	if(argc == 2)
		data.readFile(argv[1]," ");
	else
		data.readLIBSVM(argv[1]);
	cout << "There are " << data.numOfInstances() << " instances with " << data.numOfFeatures() << " variables" << endl;
	cout << "feature names:" << endl;
	vector<string> names = data.getFeatureNames();
	vector<string>::size_type i;
	for(i=0; i!=names.size(); i++)
	{
		cout << names[i] << " ";
		if((i+1)%5 == 0)
			cout << endl;
	}
	if((i+1)%5)
		cout << endl;
	
	cout << "2nd feature is " << data.getFeatureNameOf(1) << endl;
	//cout << "10th feature is " << data.getFeatureNameOf(9) << endl;
	
	cout << endl << "dataset: " << endl;
	for(int i=0; i<data.numOfInstances(); ++i) {
		cout << data[i] << " ";
		for(int j=0; j<data.numOfFeatures(); ++j)
			cout << data(i,j) << " ";
		cout << endl;
	}
	
	set<YTYPE> ys = data.getUniqueY();
	cout << endl << "class labels:" << endl;
	for(set<YTYPE>::const_iterator iter=ys.begin(); iter!=ys.end(); iter++)
		cout << *iter << " ";
	cout << endl;
	
	cout << "1st record's label is " << data[0] << ", it has independent values:" << endl;
	for(int j=0; j<data.numOfFeatures(); ++j)
		cout << data(0,j) << " ";
	cout << endl;
	
	return 0;
}