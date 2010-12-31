.PHONY: api-doc internal-doc clean all

all: api-doc internal-doc

api-doc:
	doxygen api.doxyfile

internal-doc:
	doxygen internal.doxyfile

clean:
	rm -fr api
	rm -fr internal
