#ifndef TOPOLOGYDENDOGRAMGEN_CPP
#define TOPOLOGYDENDOGRAMGEN_CPP

#include "TopologyDendogramgen.hpp"

int 
factorial(int n)
{
  return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

int
TopologyDendogram::
createTopologyGraph(int dim1, int dim2)
{
	originalGraph.Create(dim1, dim2);
	currentLevel = originalGraph; // Operator overloading must be written.

	// computePower vectors must also be intialized here.
	for(int i=0; i<originalGraph.no_of_nodes; ++i)
	{
		originalComputePower.push_back(0);
		previousLevelComputePower.push_back(0);		
	}
	
	totalComputePowerOfTopology = 0;
	int index=0;
	BGL_FORALL_VERTICES( v, originalGraph.mesh, TopologyGraph)
	{
		int temp;
		temp = originalGraph.mesh[v].constraint[0] * originalGraph.mesh[v].constraint[1];
		previousLevelComputePower.at(originalGraph.mesh[v].id) = originalComputePower.at(originalGraph.mesh[v].id) = temp;
		totalComputePowerOfTopology += temp;
	}

}

int 
TopologyDendogram::
calculateDendogramHeight(int numberOfNodes)
{
	dendogramHeight = floor(log2(numberOfNodes));
}

int
TopologyDendogram::
permuteConstraints( TopoGen* tempGraph, int* constraintIDs)
{
	int numberOfNodes=tempGraph->no_of_nodes, numberOfConstraints=tempGraph->mesh[0].constraint.size();
	BGL_FORALL_VERTICES( v, tempGraph->mesh, TopologyGraph)
	{
		std::vector <int> temp;
		for(int kk=0; kk<numberOfConstraints; ++kk)
			temp.push_back( tempGraph->mesh[v].constraint[constraintIDs[kk]] );
		for(int kk=0; kk<numberOfConstraints; ++kk)
			tempGraph->mesh[v].constraint[kk] = temp[kk];
	}
}

std::map<int, std::vector<int> >
TopologyDendogram::
groupNodesInPartitions(TopoGen temp, std::vector <int> part)
{
	int min = part[0], doIt = FALSE, nodeCounter = 0, counter = -1;
	std::map<int, std::vector<int> > nodesInPartition;
	label1:
	for(int kk=0; kk<(int) part.size(); ++kk) // identify minimum above counter
		if(part[kk] > counter)
			if(part[kk] < min)
				min = part[kk];

	for(int kk=0; kk<(int) part.size(); ++kk) // change all min above counter to counter + 1
		if(part[kk] == min)
		{
			part[kk] = counter+1;
			nodeCounter++;
			doIt = TRUE;
		}

	if(doIt == TRUE)
	{
		++counter;
		doIt = FALSE;
	}
	++min;
	if(nodeCounter != part.size())
		goto label1;

	for(int kk=0; kk<(int) part.size(); kk++)
		nodesInPartition[part[kk]].push_back(kk);
	
	return nodesInPartition;
}

std::vector <int>
TopologyDendogram::
evaluatePartitioningScheme( std::map<int, std::vector<int> > nodesInPartition )
{
	std::vector<int> tempPreviousLevelComputePower;
	int diffArray = 0;
	for(int kk=0; kk<nodesInPartition.size(); ++kk)
	{
		int tempAdder;
		tempAdder = 0;
		for(int jj=0; jj<nodesInPartition[kk].size(); ++jj)
			tempAdder += previousLevelComputePower[nodesInPartition[kk][jj]];
		tempPreviousLevelComputePower.push_back(tempAdder);
	}
	return tempPreviousLevelComputePower;
}

std::vector <int>
findNeighboursPartitions(std::map <int, std::vector<int> > nodesInPartition, std::vector <int> neighbours )
{
	std::vector <int> neighboursPartitions( neighbours.size() );
	for(int kk=0; kk<nodesInPartition.size(); ++kk)
		for(int jj=0; jj<nodesInPartition[kk].size(); ++jj)
			for(int ii=0; ii<neighbours.size(); ++ii)
				if( neighbours[ii] == nodesInPartition[kk][jj]  )
					neighboursPartitions[ii] = kk;
	
	return neighboursPartitions;
}

int
findEdgeInVector( std::vector <E> tempVector, E edgeToBeFound)
{
	for(int ii=0; ii<tempVector.size(); ++ii)
	{
		if( (tempVector.first == edgeToBeFound.first && tempVector.second) || (tempVector.second == edgeToBeFound.first && tempVector.first == edgeToBeFound.second) )
		{
			
		}
	}
}

int
TopologyDendogram::
buildDendogram(int dim1, int dim2) // any other inputs needed? Maybe some constraints like number of GPUs or ratio between CPU and GPU cud be included
{
	createTopologyGraph( dim1, dim2 );

	int i=0, j=0, numberOfConstraints = originalGraph.mesh[0].constraint.size(), numberOfNodes = originalGraph.no_of_nodes;
	int numberOfPermutations = factorial(numberOfConstraints), constraintIDs[numberOfConstraints];
	std::vector <int> tempPreviousLevelComputePower[numberOfPermutations];

	calculateDendogramHeight( numberOfNodes );
	
	for(;i<numberOfConstraints;++i)
		constraintIDs[i] = i;

	for(i=0; i<dendogramHeight; i++) // maybe changed
	{
		TopoGen tempPermutedGraphs[numberOfPermutations];
		int k = 0, permIndex =  0;
		std::map <int, std::vector<int> > nodesInPartition[numberOfPermutations];
		std::vector <int> diffArray (numberOfPermutations);
		int desiredComputePower, minPermIndex = 0, sentinnel=_INF_;
		do
		{
			tempPermutedGraphs[permIndex] = currentLevel;
			permuteConstraints( &tempPermutedGraphs[permIndex], constraintIDs );

			std::vector <int> zoltanOutputParts; // The immediate call has to fill this vector. Equivalent to part vector from Metis.
			// CALL TO ZOLTAN

			nodesInPartition[permIndex] = groupNodesInPartitions( tempPermutedGraphs[permIndex], zoltanOutputParts );
			tempPreviousLevelComputePower[permIndex] = evaluatePartitioningScheme( nodesInPartition[permIndex] );
			desiredComputePower = totalComputePowerOfTopology / nodesInPartition[permIndex].size();
			for(int kk=0; kk<tempPreviousLevelComputePower[permIndex].size(); ++kk)
				diffArray[permIndex] += fabs( tempPreviousLevelComputePower[permIndex][kk] - desiredComputePower );
			if( diffArray[permIndex] <= diffArray[minPermIndex] )
				minPermIndex = permIndex;

			permIndex++;
			zoltanOutputParts.clear();
		}while( next_permutation( constraintIDs, constraintIDs + numberOfConstraints ) );

		TopoGen chosenPermutation;
		chosenPermutation.no_of_nodes = nodesInPartition[minPermIndex].size();
		
		std::vector <E> edge_vector, visitedNodePairs;
		// Code to reform the graph goes here.
		for(int kk=0; kk<nodesInPartition[minPermIndex].size(); ++kk)
		{
			for(int jj=0; jj<nodesInPartition[minPermIndex][kk].size(); ++jj)
			{
				std::vector <int> neighbours;
				neighbours = previousLevel.getNeighbour( nodesInPartition[minPermIndex][kk][jj] ); // has to be implemented
				std::vector <int> neighboursPartitions( neighbours.size() );
				neighboursPartitions = findNeighboursPartitions( nodesInPartition[minPermIndex], neighbours );
				for(int ii=0; ii<neighboursPartitions; ++ii)
				{
					// have to continue here.. 
					// create a temp edge between the partition containing the node that is currently scrutinized and each partition in neighboursPartitions
					E tempEdge;
					tempEdge.first = kk;
					tempEdge.second = neighboursPartitions[ii];
					// and then check if this node pair exists in the edge_vector and the corresponding node pair from the previousLevel also exists. 4 if-else blocks.
					
					// if not add it!

				}
			}
		}
		// iterate thru nodesInPartition[minPermIndex][kk], get neighbours for this node from the BGL object(previousLevel), check if all these nodes that this node is connected to are already in edge_vector, if not add this to edge_vector, and accumulate the bandwidth for now.
			
		// cleanup
		previousLevelComputePower.clear();
		previousLevelComputePower = tempPreviousLevelComputePower[minPermIndex];
	}
}

#endif 
