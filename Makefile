TARGETS= insertionPerSecond speedPerformance compareLoadingFactor sizeTest
TESTFILES = tests/CountingTests.o tests/HighLevelFunctionsTests.o tests/IOTests.o tests/tagTests.o tests/LayeredCountingTests.o tests/bufferedCountingTests.o tests/onDiskCountingTests.o
OBJS= gqf.o LayeredMQF.o bufferedMQF.o hashutil.o utils.o cqf/gqf.o  countmin/countmin.o countmin/massdal.o countmin/prng.o
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

INCLUDE= -I ThirdParty/stxxl/include/ -I ThirdParty/stxxl/build/include/

CXXFLAGS =  -std=c++14  -fPIC -Wall -Wno-sign-compare -Wno-unused-variable $(DEBUG) $(PROFILE) $(OPT) $(ARCH) $(INCLUDE) -fopenmp -m64 -I. -Wno-unused-result -Wno-strict-aliasing -Wno-unused-function -fpermissive

#STXXL= -L ThirdParty/stxxl/build/lib/ -llibstxxl
STXXL= ThirdParty/stxxl/build/lib/libstxxl.a

LDFLAGS = -fopenmp $(DEBUG) $(PROFILE) $(OPT) -lz -lbz2 -lpthread


#
# declaration of dependencies
#

all: $(TARGETS)

OBJS= gqf.o	utils.o LayeredMQF.o bufferedMQF.o  onDiskMQF.o


# dependencies between programs and .o files

# main:	main.o $(OBJS)
# 	$(LD) $^ $(LDFLAGS) -o $@ $(STXXL)
# dependencies between .o files and .h files

load_test_mqf:	load_test_mqf.o gqf.o hashutil.o utils.o
	$(LD) $^ $(LDFLAGS) -o $@
insertionPerSecond:	insertionPerSecond.o  LayeredMQF.o onDiskMQF.o bufferedMQF.o  gqf.o hashutil.o utils.o cqf/gqf.o  countmin/countmin.o countmin/massdal.o countmin/prng.o
	$(LD) $^   $(LDFLAGS) -o $@ $(STXXL)
libgqf.so: gqf.o utils.o
	$(LD) $^ $(LDFLAGS) --shared -o $@

speedPerformance: speedPerformance.o LayeredMQF.o onDiskMQF.o bufferedMQF.o  gqf.o hashutil.o utils.o cqf/gqf.o  countmin/countmin.o countmin/massdal.o countmin/prng.o
	$(LD) $^   $(LDFLAGS) -o $@ $(STXXL)

compareLoadingFactor: compareLoadingFactor.o LayeredMQF.o onDiskMQF.o bufferedMQF.o  gqf.o hashutil.o utils.o cqf/gqf.o  countmin/countmin.o countmin/massdal.o countmin/prng.o
		$(LD) $^   $(LDFLAGS) -o $@ $(STXXL)


sizeTest: sizeTest.o LayeredMQF.o onDiskMQF.o bufferedMQF.o  gqf.o hashutil.o utils.o cqf/gqf.o  countmin/countmin.o countmin/massdal.o countmin/prng.o
				$(LD) $^  $(LDFLAGS) -o $@ $(STXXL)


test:  $(TESTFILES) gqf.cpp test.o utils.o
	$(LD) $(LDFLAGS) -DTEST -o mqf_test test.o LayeredMQF.o bufferedMQF.o onDiskMQF.o utils.o $(TESTFILES) gqf.cpp $(STXXL)

# main.o: hashutil.o gqf.h

# dependencies between .o files and .cc (or .c) files


gqf.o: gqf.cpp gqf.h










%.o: %.cc
	$(CXX) $(CXXFLAGS) $(INCLUDE) $< -c -o $@

%.o: %.c %.h
	$(CC) $(CXXFLAGS) $(INCLUDE) $< -c -o $@

%.o: %.cpp  %.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) $< -c -o $@




clean:
	rm -f *.o $(TARGETS) $(TESTS) $(TESTFILES) $(OBJS)
