TARGETS=main 
TARGETS=main insertionPerSecond

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
	OPT=insertionPerSecond ikii
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

main:	main.o	gqf.o hashutil.o
	$(LD) $^ $(LDFLAGS) -o $@

insertionPerSecond:	insertionPerSecond.o	gqf.o hashutil.o
		$(LD) $^ $(LDFLAGS) -o $@
# dependencies between .o files and .h files


main.o:	gqf.h hashutil.o

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
