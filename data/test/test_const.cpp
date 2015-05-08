#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
using namespace std;
class Test
{
public:
	Test(int num): value(new int[num]),n(num) {}
	
	int& operator[](int i) {return value[i];}
	const int &operator[](int i) const {return value[i];}
	
	friend ostream &operator<<(ostream &os, Test &rhs) {
		int i;
		for(i=0; i<rhs.n; i++) {
			os << rhs.value[i] << " ";
			if((i+1)%10 == 0)
				os << endl;
		}
		if(i%10 != 0)
			os << endl;
		return os;
	}
	
	~Test() {delete value;}
private:
	int *value;
	int n;
};

int main()
{
	Test data(20);
	for(int i=0; i<20; i++)
		data[i] = i+1;
	cout << data << endl;
	
	string *aa = (string*)malloc(sizeof(string)*10);
	istringstream in("gejkgoeg");
	in >> aa[0];
	cout << aa[0] << endl;
	
	return 0;
}