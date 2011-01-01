.PHONY: api-doc internals-doc clean all

all: api-doc internals-doc

api-doc:
	doxygen api.doxyfile

internals-doc:
	doxygen internals.doxyfile

clean:
	rm -fr api
	rm -fr internals
