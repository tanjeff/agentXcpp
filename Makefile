OBJS=oid.o octet_string.o
CXXFLAGS=-g -O0

%.o : %.cpp %.h
	g++ $< -o $@ -c $(CXXFLAGS)

main : main.cpp $(OBJS)
	g++ $< *.o -o $@ $(CXXFLAGS)
