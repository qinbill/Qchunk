##############
# Make the project
# By qinjianbin
##############

PROD	:= PROD
OPT     := -O3 # -pg
VERSION := \"1.0_${PROD}\"
TARGETS := preproc search
TEST_TARGETS := test_qgrams test_rawdata test_basichist test_dpselect test_sorting test_results test_bruteforce
DEFINES := #-DREAL_PROF
TEST_SRCS := test_basichist.cpp test_rawdata.cpp test_qgrams.cpp test_dpselect.cpp test_results.cpp test_bruteforce.cpp
SRCS    := bitset.cpp preproc.cpp verify.cpp search.cpp qgrams.cpp hashmap.cpp rawdata.cpp indexer.cpp prober.cpp querier.cpp basichist.cpp dpselect.cpp sorting.cpp usage.cpp test_bruteforce.cpp

CCFLAGS = ${OPT} -Wall  -Wno-deprecated -ggdb -D${PROD} ${DEFINES} -I./ -DVERSION=${VERSION} # -m64 -mcmodel=large
LDFLAGS = ${OPT} -ggdb   #-mcmodel=large
LIBS    = #-lcrypto
CC	= g++
OBJS    := ${SRCS:.cpp=.o}
TESTOBJS:= ${TEST_SRCS:.cpp=.o}


.PHONY: all clean distclean 
all:: ${TARGETS} 

test:: ${TEST_TARGETS}

preproc: preproc.o hashmap.o qgrams.o rawdata.o sorting.o
	${CC} ${LDFLAGS} -o $@ $^ ${LIBS}

search: search.o hashmap.o qgrams.o rawdata.o indexer.o verify.o bitset.o querier.o basichist.o dpselect.o sorting.o usage.o
	${CC} ${LDFLAGS} -o $@ $^ ${LIBS}

test_bruteforce: test_bruteforce.o hashmap.o qgrams.o rawdata.o indexer.o prober.o verify.o bitset.o querier.o basichist.o dpselect.o sorting.o usage.o
	${CC} ${LDFLAGS} -o $@ $^ ${LIBS}

test_sorting: test_sorting.o sorting.o
	${CC} ${CCFLAGS} -DUTEST -o $@ $^ ${LIBS}

test_results: test_results.o usage.o verify.o
	${CC} ${CCFLAGS} -DUTEST -o $@ $^ ${LIBS}

test_qgrams: test_qgrams.o qgrams.o
	${CC} ${CCFLAGS} -DUTEST -o $@ $^ ${LIBS}

test_rawdata: test_rawdata.o rawdata.o hashmap.o qgrams.o sorting.o
	${CC} ${CCFLAGS} -DUTEST -o $@ $^ ${LIBS}

test_basichist: test_basichist.o basichist.o
	${CC} ${CCFLAGS} -DUTEST -o $@ $^ ${LIBS}


test_dpselect: test_dpselect.o dpselect.o
	${CC} ${CCFLAGS} -DUTEST -o $@ $^ ${LIBS}


# rawdata.o: rawdata.cpp
# 	${CC} ${CCFLAGS} -DUTEST -o $@ -c $< 


${OBJS}: %.o: %.cpp
	${CC} ${CCFLAGS} -o $@ -c $< 

install:
	mkdir -p ../bin
	cp ${TARGETS} README.txt ../bin/

clean:: 
	-rm -f *~ *.o ${TARGETS} ${TEST_TARGETS}

distclean:: clean
