OBJS=oid.o octet_string.o varbind.o integer.o
CXXFLAGS=-g -O0

all: $(OBJS)

%.o : %.cpp %.h
	g++ $< -o $@ -c $(CXXFLAGS)



. PHONY: clean

clean:
	rm -f *.o
