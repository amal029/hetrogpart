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

static double ts_to_float( struct timespec s )
{
//  return round((( (double)s.tv_sec * 1e9 + (double)s.tv_nsec ))/1e3)/1e6;
    return (double)s.tv_sec + (double)s.tv_nsec / 1e9;
}

static double us_diff( struct timespec start, struct timespec end )
{
    return ts_to_float( end ) - ts_to_float( start );
}

//main()
int main( int argc, char **argv )
{
	if( argc < 4 )
	{
		cout << "No of options specified is not valid" << endl;
		cout << "Usage: application_graph x_dim_topo y_dim_topo" << endl;
		return -1;
	}
	srand( time( 0 ) );

	struct timespec start, end;

	ofstream metis_log;
	metis_log.open( "metis.log" );
	metis_log << endl;
	metis_log.close();

	ofstream out;
	out.open( "Map.out" );
	out.close();

	//MPI_Init( &argc, &argv );

	//level/part/node/tp_wgts
	vector< vector< vector< vector< float_t >* >* >* > tp_wgts_vector;
	//level/part/node_id
	vector< vector< vector< uint32_t >* >* > parts_vector;


	clock_gettime(CLOCK_MONOTONIC, &start);

	TopologyDendogram t;
	t.CreateTopologyGraph( atoi( argv[ 2 ] ), atoi( argv[ 3 ] ), &tp_wgts_vector, &parts_vector, argc, argv );

	clock_gettime(CLOCK_MONOTONIC, &end);

	float_t topo_time = us_diff( start, end );

	if( argv[ 1 ] == NULL )
	{
		cout << "ERROR:No application specfied" << endl;
		return -1;
	}

	clock_gettime(CLOCK_MONOTONIC, &start);

	float_t app_lat_time = 0;
	for( uint32_t kk = 0; kk < 10; kk++ )
	{
		ApplicationDendogram a;

		a.SetApplication( argv[ 1 ] );
		a.ConstructDendogram( &parts_vector, &tp_wgts_vector, &t, argc, argv );

		app_lat_time += t.time;
	}

	clock_gettime(CLOCK_MONOTONIC, &end);
	float_t app_time = us_diff( start, end );
	app_time = app_time / 10;

	fstream out1;
	out1.open( "Map.out", fstream::out | fstream::app );
	out1 << "Runtime = " << app_time + topo_time << "s" << endl;
	out1 << "Avg app lat time = " << app_lat_time << "s" << endl;
	out1.close();


	/*
	TopoGen g;
	g.Create( atoi( argv[ 2 ] ), atoi( argv[ 3 ] ) );

	MetisInterface mtn( GRAPH_K_WAY );
	vector< uint32_t > *temp_level_parts = new vector< uint32_t >;
	vector< vector< uint32_t > > parts( 4 );
	mtn.PartitionGraph( &g, 4, &parts, temp_level_parts );
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

	//MPI_Finalize();
	return 0;
}
//end of main()
