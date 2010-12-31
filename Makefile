.PHONY: all documentation library clean
CXXFLAGS += -I src/

all: library main

library:
	make -C src/


main : main.cpp library
	$(CXX) $< -o $@ src/agentxcpp.a $(CXXFLAGS)


documentation:
	make -C doc/

clean:
	make -C src/ clean
	make -C doc/ clean
