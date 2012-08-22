#ifndef TOPOLOGYDENDOGRAMGEN_HPP
#define TOPOLOGYDENDOGRAMGEN_HPP

#include "TopoGen.hpp"

#define TRUE 1
#define FALSE 0
#define _INF_ 2147483648	

class TopologyDendogram
{
	public:
	// Data Members
		TopoGen originalGraph, previousLevel;
		std::vector <int> originalComputePower;
		std::vector <int> previousLevelComputePower;
		int totalComputePowerOfTopology;
		int dendogramHeight;

	// Member Functions
		int createTopologyGraph( int numberOfProcs_dim1, int numberOfProcs_dim2 ); // This has to set originalGraph and currentLevel;
		int calculateDendogramHeight(int numberOfNodes);
		int permuteConstraints( TopoGen* tempGraph, int *constraintIDs );
		std::map<int, std::vector<int> > groupNodesInPartitions(TopoGen temp, std::vector <int> part);
		std::vector<int> evaluatePartitioningScheme( std::map<int, std::vector<int> > nodesInPartition);
		int chooseBestPartitioningScheme( ); // Dont know what the input is. The big for loop calls this

		int buildDendogram(int dim1, int dim2); // This function is the big for loop.
		// more functions need to be added

};

#endif
