TARGETS=main
TESTFILES = tests/CountingTests.o tests/HighLevelFunctionsTests.o tests/IOTests.o tests/tagTests.o

ifdef D
	DEBUG=-g
	OPT=
else
	DEBUG=
	OPT=-Ofast
endif

ifdef NH
	ARCH=
else
	ARCH=-msse4.2 -D__SSE4_2_
endif

ifdef P
	PROFILE=-pg -no-pie # for bug in gprof.
endif

CXX = g++ -std=c++11
CC = g++ -std=c++11
LD= g++ -std=c++11

CXXFLAGS = -Wall $(DEBUG) $(PROFILE) $(OPT) $(ARCH) -m64 -I. -Wno-unused-result -Wno-strict-aliasing -Wno-unused-function

LDFLAGS = $(DEBUG) $(PROFILE) $(OPT)

#
# declaration of dependencies
#

all: $(TARGETS)



# dependencies between programs and .o files

main:                  main.o 								 gqf.o
	$(LD) $^ $(LDFLAGS) -o $@
# dependencies between .o files and .h files

test:  $(TESTFILES) gqf.c test.o
	$(LD) $(LDFLAGS) -DTEST -o mqf_test test.o $(TESTFILES) gqf.c

main.o: 								 									gqf.h

# dependencies between .o files and .cc (or .c) files


gqf.o: gqf.c gqf.h

#
# generic build rules
#







%.o: %.cc
	$(CXX) $(CXXFLAGS) $(INCLUDE) $< -c -o $@

%.o: %.c
	$(CC) $(CXXFLAGS) $(INCLUDE) $< -c -o $@





clean:
	rm -f *.o $(TARGETS) $(TESTS) $(TESTFILES)
