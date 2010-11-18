.PHONY: all documentation library clean
CXXFLAGS += -I src/

all: library main

documentation:
	doxygen doxygen.conf

library:
	make -C src/


main : main.cpp library
	$(CXX) $< -o $@ $(CXXFLAGS)


clean:
	rm -fr doc/
	make -C src/ clean
