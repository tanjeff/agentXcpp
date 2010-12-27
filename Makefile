.PHONY: all documentation library clean
CXXFLAGS += -I src/

all: library main

api-doc:
	doxygen api.doxyfile

library:
	make -C src/


main : main.cpp library
	$(CXX) $< -o $@ src/agentxcpp.a $(CXXFLAGS)


clean:
	rm -fr doc/
	make -C src/ clean
