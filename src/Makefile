OBJS=oid.o \
     Octet_String.o \
     Integer.o \
     varbind.o \
     Counter32.o \
     Counter64.o \
     Gauge32.o \
     TimeTicks.o \
     Opaque.o \
     IpAddress.o \
     PDU.o \
     OpenPDU.o \
     ClosePDU.o \
     RegisterPDU.o

CXXFLAGS=-g -O0

agentxcpp.a: $(OBJS)
	$(AR) -r $@ $(OBJS)

%.o : %.cpp %.h
	$(CXX) $< -o $@ -c $(CXXFLAGS)



. PHONY: clean

clean:
	rm -f $(OBJS)
	rm -f agentxcpp.a
