##############
# Make the project
# By qinjianbin
##############

PROD	:= PROD
OPT     := -O3 # -pg
VERSION := \"0.1_${PROD}\"
TARGETS := preproc search
DEFINES := #-DREAL_PROF
SRCS    := bitset.cpp preproc.cpp verify.cpp search.cpp qgrams.cpp hashmap.cpp rawdata.cpp indexer.cpp prober.cpp querier.cpp basichist.cpp dpselect.cpp sorting.cpp usage.cpp

CCFLAGS = ${OPT} -Wall  -Wno-deprecated -ggdb -D${PROD} ${DEFINES} -I./ -DVERSION=${VERSION} # -m64 -mcmodel=large
LDFLAGS = ${OPT} -ggdb   #-mcmodel=large
LIBS    = #-lcrypto
CC	= g++
OBJS    := ${SRCS:.cpp=.o}

.PHONY: all clean distclean 
all:: ${TARGETS} 

preproc: preproc.o hashmap.o qgrams.o rawdata.o sorting.o
	${CC} ${LDFLAGS} -o $@ $^ ${LIBS}

search: search.o hashmap.o qgrams.o rawdata.o indexer.o prober.o verify.o bitset.o querier.o basichist.o dpselect.o sorting.o usage.o
	${CC} ${LDFLAGS} -o $@ $^ ${LIBS}

${OBJS}: %.o: %.cpp
	${CC} ${CCFLAGS} -o $@ -c $<

install:
	mkdir -p ../bin
	cp ${TARGETS} README.txt ../bin/

clean::
	-rm -f *~ *.o ${TARGETS}

distclean:: clean
