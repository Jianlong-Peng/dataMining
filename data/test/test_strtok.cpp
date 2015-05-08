#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>

using namespace std;

int main()
{
	char *line = (char *)malloc(sizeof(char)*50);
	char *a = "3.21 world  jgeige  jgeg geg  ";
	char *p;
	
	memcpy(line,a,strlen(a));
	line[strlen(a)] = '\0';
	
	char labels[10];
	sprintf(labels, "%d",4);
	cout << labels << endl;
	sprintf(labels, "%d",3902);
	cout << labels << endl;
	
	p = strtok(line, " ");
	cout << "after strtok(line, \" \"): " << p << endl;
	istringstream is(p);
	double temp;
	is >> temp;
	cout << "after is >> temp: " << temp << endl;
	do
	{
		p = strtok(NULL, " ");
		if(p == NULL)
			cout << "after strtok(NULL, \" \"): no more " << endl;
		else
			cout << "after strtok(NULL, \" \"): " << p << endl;
	}while(p!=NULL);
	
	cout << endl << "line: " << line << endl;
	
	free(line);
	
	return 0;
}
/*
RESULT:
after strtok(line, " "): Hello,
after strtok(NULL, " "): world
after strtok(NULL, " "): jgeige
after strtok(NULL, " "): jgeg
after strtok(NULL, " "): geg
after strtok(NULL, " "): no more

line: Hello,
*/