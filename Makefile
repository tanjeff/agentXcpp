.PHONY: all documentation library clean

all: library documentation

documentation:
	doxygen doxygen.conf

library:
	make -C src/


#main : main.cpp $(OBJS)
#	g++ $< *.o -o $@ $(CXXFLAGS)


clean:
	rm -fr doc/
	make -C src/ clean
