# Heterogeneous Architecture GRaph Partitioner --- 1.0.0.1, Aug 2012
# 16.08.2012
#
# makefile
#

CPP = g++ -Wall -I /home/servesh/App/boost_1_49_0
#MPI Includes /home/servesh/App/boost_1_49_0/

OBJ = main.o

LINK = -L /home/servesh/App/boost_1_49_0/stage/lib -lboost_graph
#MPI libs /home/servesh/App/openmpi-1.4.5/lib

RM = rm -f -v

.PHONY: all all-before all-after clean clean-custom

all: all-before Run all-after

clean: clean-custom
	${RM} $(OBJ)
	${RM} Run.x
	${RM} ops/*.*

Run: $(OBJ)
	$(CPP) $(OBJ) ${LINK} -o Run.x

main.o: main.cpp $(OBJ)
	$(CPP) -c main.cpp -o main.o

