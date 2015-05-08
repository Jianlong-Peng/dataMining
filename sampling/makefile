OBJS = ../SVM/svmTool.o ../stringUtils/stringUtils.o kennard-stone.o
CFLAGS = -Wall -IF:\script\cpplibrary\dataMining

kstone.exe: kstone.cpp ../data/dataTableSP.h $(OBJS)
	g++ $(CFLAGS) -s $(OBJS) kstone.cpp -o kstone.exe

kennard-stone.o: kennard-stone.cpp kennard-stone.hpp ../data/dataTableSP.h
	g++ $(CFLAGS) -O2 kennard-stone.cpp -c
