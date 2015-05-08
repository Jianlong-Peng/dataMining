#include <iostream>
#include <cstdlib>
#include <vector>
#include <set>
#include <string>
#include <data/dataTable.h>

using namespace std;

int main(int argc, char *argv[])
{
	if(argc!=2 && argc!=3) {
		cerr << "usage: test_dataTable.exe infile [sep]" << endl;
		cerr << "[sep]: to specify the separator <default: space>" << endl;
		return -1;
	}
	
	Data<double, int> data(true);
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
	
	set<int> ys = data.getUniqueY();
	cout << "class labels:" << endl;
	for(set<int>::const_iterator iter=ys.begin(); iter!=ys.end(); iter++)
		cout << *iter << " ";
	cout << endl;
	
	Record<double, int> record1 = data[0];
	cout << "1st record's label is " << record1.yValue << ", it has independent values:" << endl;
	for(vector<double>::const_iterator iter=record1.xValue.begin(); iter!=record1.xValue.end(); iter++)
		cout << *iter << " ";
	cout << endl;
	
	return 0;
}