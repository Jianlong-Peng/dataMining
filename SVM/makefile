CXX = g++
CFLAGS = -Wall -O2

all: svm.o svmTool.o eval.o grid_eSVR.exe grid_cSVC.exe featureImportance.exe

svm.o: svm.cpp
	$(CXX) $(CFLAGS) -c svm.cpp

svmTool.o: svmTool.cpp
	$(CXX) $(CFLAGS) -c svmTool.cpp

grid_eSVR.exe: svm.o svmTool.o grid_eSVR.cpp
	$(CXX) $(CFLAGS) svm.o svmTool.o grid_eSVR.cpp -o grid_eSVR.exe

eval.o: eval.cpp
	$(CXX) $(CFLAGS) -c eval.cpp

featureImportance.o: featureImportance.cpp
	$(CXX) $(CFLAGS) -c featureImportance.cpp

featureImportance.exe: featureImportance.cpp svm.o eval.o svmTool.o
	$(CXX) $(CFLAGS) featureImportance.cpp svm.o eval.o svmTool.o -o featureImportance.exe

grid_cSVC.exe: grid_cSVC.cpp eval.o svm.o svmTool.o
	$(CXX) $(CFLAGS) grid_cSVC.cpp eval.o svm.o svmTool.o -o grid_cSVC.exe

