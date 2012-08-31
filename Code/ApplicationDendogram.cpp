// Heterogeneous Architecture GRaph Partitioner --- 1.0.0.1, Aug 2012
// 16.08.2012

/** @file ApplicationDendogram.cpp
 *
 */

#ifndef APPLICATION_DENDOGRAM_CPP
#define APPLICATION_DENDOGRAM_CPP

#include "ApplicationDendogram.hpp"

/*
 *  @brief Contains actual code of respective functions
 *         present in the header.
 *
 *  No detailed comments will be present, refer to the header
 *  for any documentation regarding the class and its inherent
 *  functions and attributes.
 */

//ApplicationDendogram()
ApplicationDendogram :: ApplicationDendogram()
{

}
//end of ApplicationDendogram()

void ApplicationDendogram :: SetApplication( string filename )
{
	ApplicationGraph *orig_app = new ApplicationGraph();

	if( filename.find( ".dot", filename.length() - 4 ) != string::npos )
	{
		bool status = orig_app->ReadGraph( filename.c_str() );
		if( status )
		{
			cout << "Successfully read application in dot format" << endl;
		}
	}
	else
	{
		int32_t status = orig_app->ReadMetisFormat( filename.c_str() );
		if( status )
		{
			cout << "Successfully read application in metis format" << endl;
		}
		else
		{
			cout << "Error " << status << " in reading application from metis format" << endl;
		}
	}

	vector< ApplicationGraph* > app_graph_vector;
	app_graph_vector.push_back( orig_app );
	app_dendogram_obj.push_back( app_graph_vector );

}

void ApplicationDendogram :: ConstructDendogram( vector< vector< uint32_t > > parts_req, MPI_Comm mpi_comm,
														int argc, char **argv )
{
	cout << endl << "Creating Application Dendogram for structure :" << endl;
	for( uint32_t i = 0; i < parts_req.size(); i++ )
	{
		for( uint32_t j = 0; j < parts_req[ i ].size(); j++ )
		{
			cout << parts_req[ i ][ j ] << " ";
		}
		cout << endl;
	}

	for( uint32_t level = 0; level < parts_req.size(); level++ )
	{
		vector< ApplicationGraph* > app_graph_vector;
		for( uint32_t j = 0; j < parts_req[ level ].size(); j++ )
		{
			ZoltanInterface ztn;
			vector< vector< uint32_t > > parts;

			ztn.Create( MPI_COMM_WORLD, argc, argv );
			ztn.SetParmetis();

			for( uint32_t i = 0; i < parts_req[ level ][ j ]; i++ )
			{
				vector< uint32_t > t;
				parts.push_back( t );
			}

			cout << endl << "Level " << level << " " << j << " Partitions " <<  parts_req[ level ][ j ] << endl;

			ztn.PartitionGraph( app_dendogram_obj[ level ][ j ], parts_req[ level ][ j ], &parts );

			for( uint32_t i = 0; i < parts.size(); i++ )
			{
				cout << i << ": ";
				for( uint32_t k = 0; k < parts[ i ].size(); k++ )
				{
					cout << parts[ i ][ k ] << " ";
				}
				cout << endl;
			}
			cout << endl;

			app_dendogram_obj[ level ][ j ]->GenerateAppSubGraphs( &parts, &app_graph_vector );

			parts.clear();
			ztn.DestroyZoltan();
		}
		//cout << "Total No of subgraphs " << app_graph_vector.size() << endl;
		for( uint32_t i = 0; i < app_graph_vector.size(); i++ )
		{
			app_graph_vector[ i ]->PrintGraphViz( "Level_" + lexical_cast< string >( level ) +
													"_Subgraph_" + lexical_cast< string >( i ) );
		}
		app_dendogram_obj.push_back( app_graph_vector );
	}

	/*
	int ierr = 0;

	int sizeGID = 1;
	int sizeLID = 1;
	int wgt_dim = 2;

	int num_obj = ztn.GetNoOfAppNodes( &a, &ierr );

	cout << "No of nodes=" << num_obj << endl;

	ZOLTAN_ID_PTR globalID = ( ZOLTAN_ID_PTR )calloc( sizeof( int ), num_obj + 1 );
	ZOLTAN_ID_PTR localID = ( ZOLTAN_ID_PTR )calloc( sizeof( int ), num_obj + 1 );
	float *obj_wgts = ( float * )calloc( sizeof( float ), ( num_obj + 1 ) * 3 );;

	ztn.GetAppNodeList( &a, sizeGID, sizeLID, globalID, localID, wgt_dim, obj_wgts, &ierr );

	int *num_edges = ( int * )calloc( sizeof( int ), num_obj + 1 );

	ztn.GetNumAppEdges( &a, sizeGID, sizeLID, num_obj, globalID, localID, num_edges, &ierr );

	cout << "GID \t LID \t es \t cs" << endl;
	int tot_num_edges = 0;
	for( int i = 0; i < num_obj; i++ )
	{
		cout << globalID[ i ] << "\t" << localID[ i ] << "\t" << num_edges[ i ] << "\t";

		for( int j = 0; j < wgt_dim; j++ )
			cout << obj_wgts[ ( i * wgt_dim ) + j ] << "\t";

		tot_num_edges += num_edges[ i ];
		cout << endl;
	}

	ZOLTAN_ID_PTR nborGID = ( ZOLTAN_ID_PTR )calloc( sizeof( int ), tot_num_edges );
	int *nborProc = ( int * )calloc( sizeof( int ), tot_num_edges );
	int e_wgt_dim = 1;
	float *ewgts = ( float * )calloc( sizeof( float ), tot_num_edges );

	ztn.GetAppEdgeList( &a, sizeGID, sizeLID, num_obj, globalID, localID, num_edges, nborGID, nborProc, e_wgt_dim, ewgts, &ierr );


	for( int i = 0; i < tot_num_edges; i++ )
	{
		cout << nborGID[ i ] <<  "," << ewgts[ i ] << "\t";
	}
	cout << endl;
	*/

	/*
	for( int i = 0; i < num_obj; i++ )
	{
		cout << globalID[ i ] << " -- ";
		for( int j = 0; j < num_edges[ i ]; j++ )
		{
			cout << nborGID[ ( i * num_edges[ abs( i - 1 ) ] ) + j ] <<  "," << ewgts[ ( i * num_edges[ abs( i - 1 ) ] ) + j ] << "\t";
		}
		cout << endl;
	}
	*/

}

#endif
