CFLAGS=-Wall -O2 -IF:\script\cpplibrary\dataMining
OBJ=..\SVM\svm.o ..\SVM\svmTool.o ..\SVM\eval.o ..\SVM\grid.o SVM_SBS.o 

all: SVM_SBS.o sbs-svm.exe
	

SVM_SBS.o: SVM_SBS.h SVM_SBS.cpp
	g++ $(CFLAGS) -c SVM_SBS.cpp

sbs-svm.exe: main_svmsbs.cpp SVM_SBS.o
	g++ $(CFLAGS) -s $(OBJ) main_svmsbs.cpp -o sbs-svm.exe