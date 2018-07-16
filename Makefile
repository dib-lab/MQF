TARGETS=main load_test_mqf insertionPerSecond
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
	ARCH=
endif

ifdef P
	OPT=
	PROFILE=-pg -no-pie # for bug in gprof.
endif

CXX = g++ -std=c++11
CC = g++ -std=c++11
LD= g++ -std=c++11

CXXFLAGS =  -fPIC -Wall $(DEBUG) $(PROFILE) $(OPT) $(ARCH) -m64 -I. -Wno-unused-result -Wno-strict-aliasing -Wno-unused-function

LDFLAGS = $(DEBUG) $(PROFILE) $(OPT)

#
# declaration of dependencies
#

all: $(TARGETS)



# dependencies between programs and .o files

main:	main.o	gqf.o	utils.o hashutil.o
	$(LD) $^ $(LDFLAGS) -o $@
# dependencies between .o files and .h files

load_test_mqf:	load_test_mqf.o gqf.o hashutil.o utils.o
	$(LD) $^ $(LDFLAGS) -o $@
insertionPerSecond:	insertionPerSecond.o gqf.o hashutil.o utils.o
	$(LD) $^ $(LDFLAGS) -o $@
libgqf.so: gqf.o utils.o
	$(LD) $^ $(LDFLAGS) --shared -o $@

test:  $(TESTFILES) gqf.c test.o utils.o
	$(LD) $(LDFLAGS) -DTEST -o mqf_test test.o utils.o $(TESTFILES) gqf.c

main.o: hashutil.o gqf.h

# dependencies between .o files and .cc (or .c) files


gqf.o: gqf.c gqf.h

#
# generic build rules
#








%.o: %.cc
	$(CXX) $(CXXFLAGS) $(INCLUDE) $< -c -o $@

%.o: %.c
	$(CC) $(CXXFLAGS) $(INCLUDE) $< -c -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) $< -c -o $@




clean:
	rm -f *.o $(TARGETS) $(TESTS) $(TESTFILES)
