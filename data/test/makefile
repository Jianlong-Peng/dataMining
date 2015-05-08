ROOT = F:\script\cpplibrary\dataMining
CFLAGS = -I$(ROOT)
OBJS = $(ROOT)\stringUtils\stringUtils.o $(ROOT)\SVM\svmTool.o

all: test_dataTable.exe test_dataTableSP.exe
	

test_dataTable.exe: test_dataTable.cpp ..\dataTable.h ..\dataTable.cpp $(OBJS)
	g++ -Wall $(CFLAGS) $(OBJS) test_dataTable.cpp -o test_dataTable.exe

test_dataTableSP.exe: test_dataTableSP.cpp ..\dataTableSP.h ..\dataTableSP.cpp $(OBJS)
	g++ -Wall $(CFLAGS) $(OBJS) test_dataTableSP.cpp -o test_dataTableSP.exe