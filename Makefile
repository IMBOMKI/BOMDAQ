#
#  Makefile for Agilent - IVI-C Linux GetStarted
#
#  Copyright Agilent Technologies, Inc. 2001, 2002-2012
#

# Makefile behaviour can be changed by defining the following standard GNU make
# environment variables:
#   CC: the C compiler to use
#   CXX: the C++ compiler to use
#   CPPFLAGS: for C and C++ preprocessor options
#   CFLAGS: for C compiler options
#   CXXFLAGS: for C++ compiler options
#   LDFLAGS: for link options
#   RM: the rm command to use


TARGETS= \
  DataTaking


CPPFLAGS += $(shell root-config --cflags)
CPPFLAGS += -lrt
LDFLAGS  += $(shell root-config --libs)
LIBCPPFLAGS= -D_LINUX
LIBLDFLAGS= -lAgMD1



all: $(TARGETS)

clean:
	$(RM) $(TARGETS)


%: %.c
	$(CC) $(LIBCPPFLAGS) $(CPPFLAGS) $(CFLAGS) $< $(LIBLDFLAGS) $(LDFLAGS) -o $@

%: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LIBCPPFLAGS) $< $(LDFLAGS) $(LIBLDFLAGS) -o $@


