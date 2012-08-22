// Heterogeneous Architecture GRaph Partitioner --- 1.0.0.1, Aug 2012
// 17.08.2012

/** @file main.cpp
 *
 */

#include "TopoGen.hpp"
#include "ApplicationGraph.hpp"
#include "ZoltanInterface.hpp"

//main()
int main( int argc, char **argv )
{
	MPI_Init( &argc, &argv );

	TopoGen twodmesh;

	twodmesh.Create( 4, 4 );

	ApplicationGraph a;

	a.ReadGraph( argv[ 1 ] );

	ZoltanInterface ztn;

	ztn.Create( MPI_COMM_WORLD, argc, argv );
	ztn.SetParmetis(  &a );
	ztn.PartitionGraph( &a );

	MPI_Finalize();

	/*
	ztn.SetParmetis( &a );


	int ierr = 0;

	int sizeGID = 1;
	int sizeLID = 1;
	int wgt_dim = 2;

	int num_obj = ztn.GetNoOfAppNodes( NULL, &ierr );

	cout << "No of nodes=" << num_obj << endl;

	int *globalID = ( int * )calloc( sizeof( int ), num_obj + 1 );
	int *localID = ( int * )calloc( sizeof( int ), num_obj + 1 );
	float *obj_wgts = ( float * )calloc( sizeof( float ), ( num_obj + 1 ) * 3 );;

	ztn.GetAppNodeList( NULL, sizeGID, sizeLID, globalID, localID, wgt_dim, obj_wgts, &ierr );

	int *num_edges = ( int * )calloc( sizeof( int ), num_obj + 1 );

	ztn.GetNumAppEdges( NULL, sizeGID, sizeLID, num_obj, globalID, localID, num_edges, &ierr );

	int tot_num_edges = 0;
	for( int i = 0; i < num_obj; i++ )
	{
		cout << globalID[ i ] << "\t" << localID[ i ] << "\t" << num_edges[ i ] << "\t";

		for( int j = 0; j < wgt_dim; j++ )
			cout << obj_wgts[ ( i * wgt_dim ) + j ] << "\t";

		tot_num_edges += num_edges[ i ];
		cout << endl;
	}

	int *nborGID = ( int * )calloc( sizeof( int ), tot_num_edges );
	int *nborProc = ( int * )calloc( sizeof( int ), tot_num_edges );
	int e_wgt_dim = 1;
	float *ewgts = ( float * )calloc( sizeof( float ), tot_num_edges );

	ztn.GetAppEdgeList( NULL, sizeGID, sizeLID, num_obj, globalID, localID, num_edges, nborGID, nborProc, e_wgt_dim, ewgts, &ierr );

	for( int i = 0; i < tot_num_edges; i++ )
	{
		cout << nborGID[ i ] <<  "," << ewgts[ i ] << "\t";
	}
	cout << endl;
	*/

	return 0;
}
//end of main()
