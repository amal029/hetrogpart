# Heterogeneous Architecture GRaph Partitioner --- 1.0.0.1, Aug 2012
# 16.08.2012
#
# makefile
#

CPP = mpic++ -Wall -std=gnu++0x -I/home/servesh/App/boost_1_51_0/include -I/home/servesh/App/parmetis-4.0.2/include -I/home/servesh/App/metis-5.0.2/include/ -I/home/servesh/App/zoltan-3.6/include/ -g -lm

OBJ = main.o TopoGen.o ApplicationGraph.o ZoltanInterface.o ApplicationDendogram.o TopologyDendogram.o

LINK = -L/home/servesh/App/boost_1_51_0/lib -L/home/servesh/App/metis-5.0.2/lib/ -L/home/servesh/App/parmetis-4.0.2/lib -L/home/servesh/App/openmpi-1.4.5/lib -L/home/servesh/App/zoltan-3.6/lib/ -lboost_graph -lzoltan -lboost_mpi -lmpi -lparmetis -lmetis  -L/home/servesh/App/scotch_5.1.2/lib/ -lptscotch  -lptscotcherr -lptscotcherrexit

RM = rm -f -v

.PHONY: all all-before all-after clean clean-custom

all: all-before Run all-after

clean: clean-custom
	${RM} $(OBJ)
	${RM} Run.x
	${RM} ops/*.*

Run: $(OBJ)
	$(CPP) $(OBJ) ${LINK} -o Run.x

main.o: main.cpp
	$(CPP) -c main.cpp -o main.o

TopoGen.o: TopoGen.cpp TopoGen.hpp
	$(CPP) -c TopoGen.cpp -o TopoGen.o

ApplicationGraph.o: ApplicationGraph.cpp ApplicationGraph.hpp
	$(CPP) -c ApplicationGraph.cpp -o ApplicationGraph.o

ApplicationDendogram.o: ApplicationDendogram.cpp ApplicationDendogram.hpp ApplicationGraph.o
	$(CPP) -c ApplicationDendogram.cpp -o ApplicationDendogram.o

ZoltanInterface.o: ZoltanInterface.cpp ZoltanInterface.hpp TopoGen.o ApplicationGraph.o
	$(CPP) -c ZoltanInterface.cpp -o ZoltanInterface.o

TopologyDendogram.o: TopologyDendogram.cpp TopologyDendogram.hpp TopoGen.o
	$(CPP) -c TopologyDendogram.cpp -o TopologyDendogram.o
