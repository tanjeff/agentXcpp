OBJS=oid.o \
     octet_string.o \
     Integer.o \
     varbind.o \
     Counter32.o \
     Counter64.o \
     Gauge32.o \
     TimeTicks.o \
     Opaque.o
CXXFLAGS=-g -O0

agentxcpp.a: $(OBJS)
	$(AR) -r $@ $(OBJS)

%.o : %.cpp %.h
	$(CXX) $< -o $@ -c $(CXXFLAGS)



. PHONY: clean

clean:
	rm -f $(OBJS)
	rm -f agentxcpp.a
