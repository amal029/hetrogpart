// Heterogeneous Architecture GRaph Partitioner --- 1.0.0.1, Aug 2012
// 17.08.2012

/** @file main.cpp
 *
 */
#include <iostream>
#include <vector>

#include "TopoGen.hpp"
#include "ApplicationGraph.hpp"
//#include "ZoltanInterface.hpp"
#include "ApplicationDendogram.hpp"
#include "TopologyDendogram.hpp"

using namespace boost;
using namespace std;

//main()
int main( int argc, char **argv )
{
	ofstream metis_log;
	metis_log.open( "metis.log" );
	metis_log << endl;
	metis_log.close();

	//MPI_Init( &argc, &argv );

	//level/part/node/tp_wgts
	vector< vector< vector< vector< float_t >* >* >* > tp_wgts_vector;
	//level/part/node_id
	vector< vector< vector< uint32_t >* >* > parts_vector;

	TopologyDendogram t;
	t.CreateTopologyGraph( atoi( argv[ 2 ] ), atoi( argv[ 3 ] ), &tp_wgts_vector, &parts_vector, argc, argv );

	/*
	TopoGen g;
	g.Create( atoi( argv[ 2 ] ), atoi( argv[ 3 ] ) );

	MetisInterface mtn( GRAPH_K_WAY );
	vector< uint32_t > *temp_level_parts = new vector< uint32_t >;
	vector< vector< uint32_t > > parts( 4 );
	mtn.PartitionGraph( &g, 4, &parts, temp_level_parts );
	*/

	if( argv[ 1 ] == NULL )
	{
		cout << "ERROR:No application specfied" << endl;
		return -1;
	}

	ApplicationDendogram a;
	a.SetApplication( argv[ 1 ] );

	a.ConstructDendogram( &parts_vector, &tp_wgts_vector, &t, argc, argv );

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

	//MPI_Finalize();
	return 0;
}
//end of main()
