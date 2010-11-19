OBJS=oid.o \
     octet_string.o \
     Integer.o \
     varbind.o \
     counter32.o \
     Gauge32.o \
     TimeTicks.o
CXXFLAGS=-g -O0

agentxcpp.a: $(OBJS)
	$(AR) -r $@ $(OBJS)

%.o : %.cpp %.h
	$(CXX) $< -o $@ -c $(CXXFLAGS)



. PHONY: clean

clean:
	rm -f $(OBJS)
	rm -f agentxcpp.a
