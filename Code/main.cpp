// Heterogeneous Architecture GRaph Partitioner --- 1.0.0.1, Aug 2012
// 17.08.2012

/** @file main.cpp
 *
 */
#include <iostream>
#include <vector>

#include "TopoGen.hpp"
#include "ApplicationGraph.hpp"
#include "ZoltanInterface.hpp"
#include "ApplicationDendogram.hpp"
#include "TopologyDendogram.hpp"

using namespace boost;
using namespace std;

//main()
int main( int argc, char **argv )
{
	MPI_Init( &argc, &argv );

	cout << endl;

	//TopoGen twodmesh;
	//twodmesh.Create( 4, 4 );

	vector< vector< uint32_t >* > *tp_wgts;
	vector< vector< uint32_t >* > *parts;

	tp_wgts = new vector< vector< uint32_t >* >;
	parts = new vector< vector< uint32_t >* >;

	TopologyDendogram t;

	t.CreateTopologyGraph( 4, 4, tp_wgts, parts, MPI_COMM_WORLD, argc, argv );

	/*
	ApplicationDendogram a;
	a.SetApplication( argv[ 1 ] );

	vector< vector< uint32_t > > parts_req;

	for( uint32_t i = 1; i < 3; i++ )
	{
		vector< uint32_t > part;
		part.resize( i, 2 );

		parts_req.push_back( part );
	}

	a.ConstructDendogram( parts_req, NULL, argc, argv );
	*/

	/*
	vector< vector< uint32_t > > partitions;

	vector< uint32_t > a;
	partitions.push_back( a );
	partitions.push_back( a );

	for( uint32_t i = 0; i < 16; i++ )
	{
		partitions[ i%2 ].push_back( i );
	}

	TopoGen *subgraph;
	twodmesh.GenerateTopoSubGraph( &partitions, subgraph );
	*/

	MPI_Finalize();
	return 0;
}
//end of main()
