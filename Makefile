TARGETS=main
TESTFILES = tests/CountingTests.o tests/HighLevelFunctionsTests.o tests/IOTests.o tests/tagTests.o tests/LayeredCountingTests.o tests/bufferedCountingTests.o tests/onDiskCountingTests.o

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

INCLUDE= -I ThirdParty/stxxl/include/ -I ThirdParty/stxxl/build/include/

CXXFLAGS =  -fPIC -Wall $(DEBUG) $(PROFILE) $(OPT) $(ARCH) $(INCLUDE) -fopenmp -m64 -I. -Wno-unused-result -Wno-strict-aliasing -Wno-unused-function

#STXXL= -L ThirdParty/stxxl/build/lib/ -llibstxxl
STXXL= ThirdParty/stxxl/build/lib/libstxxl.a

LDFLAGS = -fopenmp $(DEBUG) $(PROFILE) $(OPT)

#
# declaration of dependencies
#

all: $(TARGETS)

OBJS= gqf.o	utils.o LayeredMQF.o bufferedMQF.o  onDiskMQF.o


# dependencies between programs and .o files

main:	main.o $(OBJS)
	$(LD) $^ $(LDFLAGS) -o $@ $(STXXL)
# dependencies between .o files and .h files

libgqf.so: $(OBJS)
	$(LD) $^ $(LDFLAGS) --shared -o $@

test:  $(TESTFILES) gqf.c test.o utils.o
	$(LD) $(LDFLAGS) -DTEST -o mqf_test test.o LayeredMQF.o bufferedMQF.o onDiskMQF.o utils.o $(TESTFILES) gqf.c $(STXXL)

main.o: gqf.h

# dependencies between .o files and .cc (or .c) files


gqf.o: gqf.c gqf.h








%.o: %.cc
	$(CXX) $(CXXFLAGS) $(INCLUDE) $< -c -o $@

%.o: %.c %.h
	$(CC) $(CXXFLAGS) $(INCLUDE) $< -c -o $@

%.o: %.cpp  %.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) $< -c -o $@




clean:
	rm -f $(OBJS) $(TARGETS) $(TESTS) $(TESTFILES)
