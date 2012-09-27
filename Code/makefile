# Heterogeneous Architecture GRaph Partitioner --- 1.0.0.1, Aug 2012
# 16.08.2012
#
# makefile
#

CPP = g++ -Wall -std=gnu++0x -I/home/servesh/App/boost_1_51_0/include -I/home/servesh/App/metis-old/include/ -g -lm -lrt
# -I/home/servesh/App/parmetis-4.0.2/include-I/home/servesh/App/zoltan-3.6/include/

CPPFLAGS =

CC = gcc
CC_FLAGS = -DSVNINFO="0"

LINK_C =

OBJ = main.o TopoGen.o ApplicationGraph.o ApplicationDendogram.o TopologyDendogram.o MetisInterface.o $(METIS_O)
#ZoltanInterface.o

LINK = -L/home/servesh/App/boost_1_51_0/lib -L/home/servesh/App/metis-old/lib/ -lboost_graph -lmetis
#  -L/home/servesh/App/parmetis-4.0.2/lib -L/home/servesh/App/openmpi-1.4.5/lib -L/home/servesh/App/zoltan-3.6/lib/  -lzoltan -lboost_mpi -lmpi -lparmetis -lmetis -L/home/servesh/App/scotch_5.1.2/lib/ -lptscotch  -lptscotcherr -lptscotcherrexit

RM = rm -f -v

.PHONY: all all-before all-after clean clean-custom

METIS_C = $(wildcard metis/*.c)
METIS_O = $(foreach f, $(METIS_C), $(notdir $(basename $(f))).o)

#debug-shortpath: makefile TopoGen.cpp TopoGen.hpp
#	$(CPP) ${CPPFLAGS} -c TopoGen.cpp -DSHORT_PATH  -o TopoGen.o
#	$(CPP) ${CPPFLAGS} $(OBJ) ${LINK} -o Run.x

all: all-before Run all-after

clean: clean-custom
	${RM} $(OBJ)
	${RM} Run.x
	${RM} ops/*.*
	${RM} rm -rf *.part.*
	${RM} rm -rf Level_*


clean-ops:
	${RM} Level*

Run: $(OBJ)
	$(CPP) ${CPPFLAGS} $(OBJ) ${LINK} -o Run.x

%.o: metis/%.c makefile
	$(CC) $(CC_FLAGS) -Imetis/GKlib -Imetis -Imetis/libmetis -c $< -o $@

main.o: makefile main.cpp
	$(CPP) ${CPPFLAGS} -c main.cpp -o main.o

TopoGen.o: makefile TopoGen.cpp TopoGen.hpp
	$(CPP) ${CPPFLAGS} -c TopoGen.cpp -o TopoGen.o

ApplicationGraph.o: makefile ApplicationGraph.cpp ApplicationGraph.hpp
	$(CPP) ${CPPFLAGS} -c ApplicationGraph.cpp -o ApplicationGraph.o

ApplicationDendogram.o: makefile ApplicationDendogram.cpp ApplicationDendogram.hpp ApplicationGraph.o TopologyDendogram.o
	$(CPP) ${CPPFLAGS} -c ApplicationDendogram.cpp -o ApplicationDendogram.o

#ZoltanInterface.o: makefile ZoltanInterface.cpp ZoltanInterface.hpp TopoGen.o ApplicationGraph.o
#	$(CPP) -c ZoltanInterface.cpp -o ZoltanInterface.o

MetisInterface.o: makefile MetisInterface.cpp MetisInterface.hpp TopoGen.o ApplicationGraph.o $(METIS_O) $(METIS_C)
	$(CPP) ${CPPFLAGS} -c MetisInterface.cpp -o MetisInterface.o

TopologyDendogram.o: makefile TopologyDendogram.cpp TopologyDendogram.hpp TopoGen.o ApplicationGraph.o
	$(CPP) ${CPPFLAGS} -c TopologyDendogram.cpp -o TopologyDendogram.o
