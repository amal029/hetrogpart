// Heterogeneous Architecture GRaph Partitioner --- 1.0.0.1, Aug 2012
// 20.08.2012

/** @file ZoltanInterface.cpp
 *
 */

#ifndef ZOLTAN_INTERFACE_CPP
#define ZOLTAN_INTERFACE_CPP

#include "ZoltanInterface.hpp"

/*
 *  @brief Contains actual code of respective functions
 *         present in the header.
 *
 *  No detailed comments will be present, refer to the header
 *  for any documentation regarding the class and its inherent
 *  functions and attributes.
 */

//ZoltanInterface()
ZoltanInterface :: ZoltanInterface()
{
	rank = 0;
	world_size = 0;

}
//ZoltanInterface()

//Create( MPI_Comm mpi_comm )
int32_t ZoltanInterface :: Create( MPI_Comm mpi_comm, int argc, char* argv[] )
{
	world = communicator( mpi_comm, comm_attach ); // create a boost comm and attach it to mpi_comm
	rank = world.rank();
	world_size = world.rank();

	//cout << "Created Process " << rank << " of " << world_size << endl;

	float ver;
	int rc = Zoltan_Initialize( argc, argv, &ver );
	if( rc != ZOLTAN_OK )
	{
		cout << "Failled to create on Process" << rank << endl;
		return -1;
	}

	zz = new Zoltan( mpi_comm );

	return 0;
}
//end of Create( MPI_Comm mpi_comm )

void ZoltanInterface :: SetParmetis()
{
	zz->Set_Param(  "DEBUG_LEVEL", "0" );
	zz->Set_Param(  "LB_METHOD", "GRAPH" );
	zz->Set_Param(  "LB_APPROACH", "PARTITION" );
	zz->Set_Param(  "NUM_GID_ENTRIES", "1" );
	zz->Set_Param(  "NUM_LID_ENTRIES", "1" );

	//zz->Set_Param(  "RETURN_LISTS", "ALL" ); //return the new process and partition assignment of every local object
	//zz->Set_Param(  "PHG_EDGE_SIZE_THRESHOLD", "1.0" );

	zz->Set_Param( "GRAPH_PACKAGE", "PARMETIS");
	zz->Set_Param(  "PARMETIS_METHOD", "PARTKWAY"); //multilevel Kernighan-Lin partitioning

	//zz->Set_Param( "DEBUG_LEVEL", "10" );
	zz->Set_Param( "PARMETIS_OUTPUT_LEVEL", "7" );
	zz->Set_Param(  "PARMETIS_COARSE_ALG", "1" ); //serial partitioning algorithm
	zz->Set_Param(  "PARMETIS_SEED", "3840" );
	zz->Set_Param(  "CHECK_GRAPH", "2"); //full checking
	//zz->Set_Param(  "IMBALANCE_TOL", "1.9"); //node weight imbalance tolerance


	/*
	zz->Set_Param(  "DEBUG_LEVEL", "0" );
	zz->Set_Param(  "LB_METHOD", "GRAPH" );
	zz->Set_Param(  "LB_APPROACH", "PARTITION" );
	zz->Set_Param(  "NUM_GID_ENTRIES", "1" );
	zz->Set_Param(  "NUM_LID_ENTRIES", "1" );

	//zz->Set_Param(  "RETURN_LISTS", "ALL" ); //return the new process and partition assignment of every local object
	//zz->Set_Param(  "PHG_EDGE_SIZE_THRESHOLD", "1.0" );

	zz->Set_Param( "SCOTCH_TYPE", "LOCAL" );
	zz->Set_Param( "GRAPH_PACKAGE", "SCOTCH");
	zz->Set_Param(  "SCOTCH_METHOD", "RBISECT"); //multilevel Kernighan-Lin partitioning
	//zz->Set_Param(  "PARMETIS_COARSE_ALG", "1" ); //serial partitioning algorithm
	zz->Set_Param(  "CHECK_GRAPH", "2"); //full checking
	//zz->Set_Param(  "IMBALANCE_TOL", "1.5"); //node weight imbalance tolerance
	*/
}

int32_t ZoltanInterface :: PartitionGraph( ApplicationGraph *app_graph_obj_ref, uint32_t nparts,
											vector< vector< float >* > *tp_wgts, vector< vector< uint32_t > > *partitions )
{
	if( nparts < 1 )
	{
		cout << "Error: No of partitions requested is invalid" << endl;
		return -1;
	}

	int changed, numGidEntries, numLidEntries, numImport, numExport;
	ZOLTAN_ID_PTR importGlobalGids, importLocalGids, exportGlobalGids, exportLocalGids;
	int *importProcs, *importToPart, *exportProcs, *exportToPart;

	app_graph_obj = app_graph_obj_ref;

	if( num_edges( app_graph_obj_ref->app_graph ) == 0 ||
			num_vertices( app_graph_obj_ref->app_graph ) == 0 )
	{
		cout << "Error: Cannot partition a graph with zero edges or vertices" << endl;
		return -1;
	}

	BGL_FORALL_VERTICES( v, app_graph_obj->app_graph, ApplicationGraphType )
	{
		partitions->at( 0 ).push_back( app_graph_obj->app_graph[ v ].id );
	}

	zz->Set_Param( "NUM_GLOBAL_PARTS", lexical_cast< string >( nparts ) );

	zz->Set_Param(  "OBJ_WEIGHT_DIM", lexical_cast< string >( app_graph_obj->no_of_constraints ) ); //number of weights per object
	zz->Set_Param(  "EDGE_WEIGHT_DIM", "1" ); //number of weights per edge

	// set call backs for zoltan
	zz->Set_Num_Obj_Fn( ZoltanInterface :: GetNoOfAppNodes, app_graph_obj );
	zz->Set_Obj_List_Fn( ZoltanInterface :: GetAppNodeList, app_graph_obj );
	zz->Set_Num_Edges_Multi_Fn( ZoltanInterface :: GetNumAppEdges, app_graph_obj );
	zz->Set_Edge_List_Multi_Fn( ZoltanInterface :: GetAppEdgeList, app_graph_obj );

	if( tp_wgts->size() > 0 )
	//if( 0 )
	{
		int global_num = 1;
		int len;
		vector< int32_t > part_ids;// = new vector< int32_t >;
		vector< int32_t > wgt_idx;// = new vector< int32_t >;
		vector< float_t > part_sizes;// = new vector< float_t >;

		if( app_graph_obj->SetTpWgts( tp_wgts, &part_ids, &wgt_idx, &part_sizes ) )
		{
			len = part_sizes.size();
			cout << "tpwgts : " ;
			for( uint32_t i = 0; i < part_sizes.size(); i++ )
			{
				cout << setprecision( 2 ) << part_sizes[ i ] << " ";
			}
			cout << endl;

			int rc = zz->LB_Set_Part_Sizes( global_num, len, ( int* ) &part_ids[ 0 ], ( int* ) &wgt_idx[ 0 ], ( float* ) &part_sizes[ 0 ] );

			if( rc != ZOLTAN_OK )
			{
				cout << "Failled to set tp wgts on process" << rank << endl;
				return -1;
			}
		}
		else
		{
			cout << "Failled to get tp wgts on process" << rank << endl;
			return -1;
		}
	}

	int rc = zz->LB_Partition(	changed, //1 if partitioning was changed, 0 otherwise
								numGidEntries, //Number of integers used for a global ID
								numLidEntries, //Number of integers used for a local ID
								numImport, // Number of vertices to be sent to me
								importGlobalGids, //Global IDs of vertices to be sent to me
								importLocalGids, //Local IDs of vertices to be sent to me
								importProcs, //Process rank for source of each incoming vertex
								importToPart, //New partition for each incoming vertex
								numExport, //Number of vertices I must send to other processes
								exportGlobalGids, //Global IDs of the vertices I must send
								exportLocalGids, //Local IDs of the vertices I must send
								exportProcs, //Process to which I send each of the vertices
								exportToPart ); //Partition to which each vertex will belong

	if( rc != ZOLTAN_OK )
	{
		cout << "Failled to partition on Process" << rank << endl;
		return -1;
	}

	//zz->LB_Eval( 1, NULL, NULL, NULL );

	//cout << rc << " changed=" << changed << " numImport=" << numImport << " numExport=" << numExport;
	//cout << " numGidEntries=" << numGidEntries << " numLidEntries=" << numLidEntries << endl;

	/*
	cout << "importToPart    = ";
	for( int i = 0; i < numImport; i++ )
	{
		cout << setw(2) << setfill(' ') << importToPart[ i ] << " ";
	}
	cout << endl;

	cout << "importGlobalGids= ";
	for( int i = 0; i < numImport; i++ )
	{
		cout << setw(2) << setfill(' ') << importGlobalGids[ i ] << " ";
	}
	cout << endl;

	cout << "importLocalGids = ";
	for( int i = 0; i < numImport; i++ )
	{
		cout << setw(2) << setfill(' ') << importLocalGids[ i ] << " ";
	}
	cout << endl;
	*/

	/*
	cout << "importProcs=";
	for( int i = 0; i < numImport; i++ )
	{
		cout << setw(2) << setfill(' ') << importProcs[ i ] << " ";
	}
	cout << endl;
	*/

	/*
	cout << "exportToPart    = ";
	for( int i = 0; i < numExport; i++ )
	{
		cout << setw( 2 ) << setfill(' ') << exportToPart[ i ] << " ";
	}
	cout << endl;

	cout << "exportGlobalGids= ";
	for( int i = 0; i < numExport; i++ )
	{
		cout << setw( 2 ) << setfill(' ') << exportGlobalGids[ i ] << " ";
	}
	cout << endl;
	*/

	/*
	cout << "exportLocalGids = ";
	for( int i = 0; i < numExport; i++ )
	{
		cout << setw( 2 ) << setfill(' ') << exportLocalGids[ i ] << " ";
	}
	cout << endl;
	*/

	/*
	cout << "exportProcs=";
	for( int i = 0; i < numExport; i++ )
	{
		cout << setw(2) << setfill(' ') << exportProcs[ i ] << " ";
	}
	cout << endl;
	*/

	if( changed == 1 )
	{
		for( int32_t i = 0; i < numExport; i++ )
		{
			partitions->at( exportToPart[ i ] ).push_back( lexical_cast< uint32_t >( exportGlobalGids[ i ]  ) );

			for( uint32_t j = 0; j < partitions->at( 0 ).size(); j++ )
			{
				if( partitions->at( 0 ).at( j ) == exportGlobalGids[ i ] )
				{
					partitions->at( 0 ).erase( partitions->at( 0 ).begin() + j );
					break;
				}
			}
		}
	}


	/*
	std::map<int, int> assignmentMap;
	if( world.rank() == 0 )
	{
		for( int i = 0; i < numExport; i++ )
		{
			("obj %d to rank %d\n", exportGlobalGids[i], exportProcs[i]);
			Vertex *v = theG->vlist[exportGlobalGids[i]];
			// note: graph nodes assigned to rank 0 are not put in the
			// assignment map. (default = 0)
			assignmentMap[ v->id() ] = exportProcs[ i ];
		}
	}

	// Distrubute subgraphs to each node
	broadcast(world, assignmentMap, 0);

	for( VertexList_t::iterator iter = theG->vlist.begin(); iter != theG->vlist.end(); ++iter ) {
	Vertex *v = (*iter).second;
	v->prop_list.set(GRAPH_RANK, boost::str(boost::format("%1%") % assignmentMap[ v->id() ] ) );
	v->rank = assignmentMap[ v->id() ];
	}
	*/

	// cleanup
	Zoltan_LB_Free_Part( &importGlobalGids, &importLocalGids, &importProcs, &importToPart );
	Zoltan_LB_Free_Part( &exportGlobalGids, &exportLocalGids, &exportProcs, &exportToPart );

	return 0;
}

void ZoltanInterface :: DestroyZoltan()
{
	//cout << "Destroyed zoltan on Process " << rank << endl;
	delete zz;
}

/*
 * Application Graph specific query functions
 */
int ZoltanInterface :: GetNoOfAppNodes( void *data, int *ierr )
{
	ApplicationGraph *app_graph_obj = ( ApplicationGraph * ) data;

	*ierr = ZOLTAN_OK;
	return num_vertices( app_graph_obj->app_graph );
}

void ZoltanInterface :: GetAppNodeList( void *data, int size_gid, int size_lid,
										ZOLTAN_ID_PTR global_id, ZOLTAN_ID_PTR local_id,
										int wgt_dim, float *obj_wgts, int *ierr )
{
	ApplicationGraph *app_graph_obj = ( ApplicationGraph * ) data;

	uint32_t id_index = 0;
	uint32_t wgt_index = 0;
	BGL_FORALL_VERTICES( v, app_graph_obj->app_graph, ApplicationGraphType )
	{
		global_id[ id_index ] = app_graph_obj->app_graph[ v ].id;
		local_id[ id_index ] = app_graph_obj->app_graph[ v ].id;

		id_index++;

		for( uint32_t i = 0; i < app_graph_obj->app_graph[ v ].constraint.size(); i++  )
			obj_wgts[ wgt_index++ ] = app_graph_obj->app_graph[ v ].constraint[ i ];

		//wgt_dim = app_graph_obj->app_graph[ v ].constraint.size();
	}

	*ierr = ZOLTAN_OK;
}

void ZoltanInterface :: GetNumAppEdges( void *data, int size_gid, int size_lid,
										int num_obj, ZOLTAN_ID_PTR global_id, ZOLTAN_ID_PTR local_id,
										int *num_edges, int *ierr )
{
	ApplicationGraph *app_graph_obj = ( ApplicationGraph * ) data;

	if( ( size_gid != 1 ) || ( size_lid != 1 ) )
	{
		*ierr = ZOLTAN_FATAL;
		return;
	}

	uint32_t id_index = 0;
	BGL_FORALL_VERTICES( v, app_graph_obj->app_graph, ApplicationGraphType )
	{
		graph_traits< ApplicationGraphType >::out_edge_iterator out_i, out_end;
		int no_of_edges = 0;
		for( tie( out_i, out_end ) = out_edges( v, app_graph_obj->app_graph ); out_i != out_end; ++out_i)
		{
			no_of_edges++;
		}

		num_edges[ id_index++ ] = no_of_edges;
	}
}

void ZoltanInterface :: GetAppEdgeList( void *data, int size_gid, int size_lid, int num_obj,
										ZOLTAN_ID_PTR global_id, ZOLTAN_ID_PTR local_id,
										int *num_edges, ZOLTAN_ID_PTR nbor_gid, int *nbor_proc,
										int wgt_dim, float *ewgts, int *ierr )
{
	ApplicationGraph *app_graph_obj = ( ApplicationGraph * ) data;

	ZOLTAN_ID_PTR next_id = nbor_gid;
	int *next_proc = nbor_proc;
	float *next_wght = ewgts;

	if( ( size_gid != 1 ) || ( size_lid != 1 ) || ( wgt_dim != 1 ) )
	{
		*ierr = ZOLTAN_FATAL;
		return;
	}

	uint32_t id_index = 0;
	BGL_FORALL_VERTICES( v, app_graph_obj->app_graph, ApplicationGraphType )
	{
		graph_traits< ApplicationGraphType >::out_edge_iterator out_i, out_end;
		graph_traits< ApplicationGraphType >::edge_descriptor e;
		int no_of_edges = 0;
		for( tie( out_i, out_end ) = out_edges( v, app_graph_obj->app_graph ); out_i != out_end; ++out_i)
		{
			no_of_edges++;
			*next_proc++ = 0; // All nodes are on the first process
			e = *out_i; //assign edge desciptor to iterator

			*next_id++ = app_graph_obj->app_graph[ target( e, app_graph_obj->app_graph ) ].id; // array containing ids of the nieghbours
			*next_wght++ = app_graph_obj->app_graph[ e ].weight; // array containting weight of each edge
		}

		if( num_edges[ id_index ] != no_of_edges )
		{
			*ierr = ZOLTAN_FATAL;
			return;
		}

		id_index++;
	}

	*ierr = ZOLTAN_OK;

	return;
}

int32_t ZoltanInterface :: PartitionGraph( TopoGen *topo_graph_obj_ref, uint32_t nparts, vector< vector< uint32_t > > *partitions, vector< uint32_t > *parts )
{
	if( nparts < 1 )
	{
		cout << "Error: No of partitions requested is invalid" << endl;
		return -1;
	}

	int changed, numGidEntries, numLidEntries, numImport, numExport;
	ZOLTAN_ID_PTR importGlobalGids, importLocalGids, exportGlobalGids, exportLocalGids;
	int *importProcs, *importToPart, *exportProcs, *exportToPart;

	topo_graph_obj = topo_graph_obj_ref;

	if( num_edges( topo_graph_obj_ref->mesh ) == 0 ||
			num_vertices( topo_graph_obj_ref->mesh ) == 0 )
	{
		cout << "Error: Cannot partition a graph with zero edges or vertices" << endl;
		return -1;
	}

	BGL_FORALL_VERTICES( v, topo_graph_obj->mesh, TopologyGraph )
	{
		partitions->at( 0 ).push_back( topo_graph_obj->mesh[ v ].id );
	}

	zz->Set_Param( "NUM_GLOBAL_PARTS", lexical_cast< string >( nparts ) );

	zz->Set_Param(  "OBJ_WEIGHT_DIM", lexical_cast< string >( topo_graph_obj->no_of_constraints ) ); //number of weights per object
	zz->Set_Param(  "EDGE_WEIGHT_DIM", "1" ); //number of weights per edge

	// set call backs for zoltan
	zz->Set_Num_Obj_Fn( ZoltanInterface :: GetNoOfTopoNodes, topo_graph_obj );
	zz->Set_Obj_List_Fn( ZoltanInterface :: GetTopoNodeList, topo_graph_obj );
	zz->Set_Num_Edges_Multi_Fn( ZoltanInterface :: GetNumTopoEdges, topo_graph_obj );
	zz->Set_Edge_List_Multi_Fn( ZoltanInterface :: GetTopoEdgeList, topo_graph_obj );

	int rc = zz->LB_Partition(	changed, //1 if partitioning was changed, 0 otherwise
								numGidEntries, //Number of integers used for a global ID
								numLidEntries, //Number of integers used for a local ID
								numImport, // Number of vertices to be sent to me
								importGlobalGids, //Global IDs of vertices to be sent to me
								importLocalGids, //Local IDs of vertices to be sent to me
								importProcs, //Process rank for source of each incoming vertex
								importToPart, //New partition for each incoming vertex
								numExport, //Number of vertices I must send to other processes
								exportGlobalGids, //Global IDs of the vertices I must send
								exportLocalGids, //Local IDs of the vertices I must send
								exportProcs, //Process to which I send each of the vertices
								exportToPart ); //Partition to which each vertex will belong

	if( rc != ZOLTAN_OK )
	{
		cout << "Failled to partition on Process" << rank << endl;
		return -1;
	}

	//cout << rc << " changed=" << changed << " numImport=" << numImport << " numExport=" << numExport;
	//cout << " numGidEntries=" << numGidEntries << " numLidEntries=" << numLidEntries << endl;

	/*
	cout << "importToPart    = ";
	for( int i = 0; i < numImport; i++ )
	{
		cout << setw(2) << setfill(' ') << importToPart[ i ] << " ";
	}
	cout << endl;

	cout << "importGlobalGids= ";
	for( int i = 0; i < numImport; i++ )
	{
		cout << setw(2) << setfill(' ') << importGlobalGids[ i ] << " ";
	}
	cout << endl;

	cout << "importLocalGids = ";
	for( int i = 0; i < numImport; i++ )
	{
		cout << setw(2) << setfill(' ') << importLocalGids[ i ] << " ";
	}
	cout << endl;
	*/

	/*
	cout << "importProcs=";
	for( int i = 0; i < numImport; i++ )
	{
		cout << setw(2) << setfill(' ') << importProcs[ i ] << " ";
	}
	cout << endl;
	*/

	/*
	cout << "exportToPart    = ";
	for( int i = 0; i < numExport; i++ )
	{
		cout << setw( 2 ) << setfill(' ') << exportToPart[ i ] << " ";
	}
	cout << endl;

	cout << "exportGlobalGids= ";
	for( int i = 0; i < numExport; i++ )
	{
		cout << setw( 2 ) << setfill(' ') << exportGlobalGids[ i ] << " ";
	}
	cout << endl;
	*/

	/*
	cout << "exportLocalGids = ";
	for( int i = 0; i < numExport; i++ )
	{
		cout << setw( 2 ) << setfill(' ') << exportLocalGids[ i ] << " ";
	}
	cout << endl;
	*/

	/*
	cout << "exportProcs=";
	for( int i = 0; i < numExport; i++ )
	{
		cout << setw(2) << setfill(' ') << exportProcs[ i ] << " ";
	}
	cout << endl;
	*/

	if( changed == 1 )
	{
		for( int32_t i = 0; i < numExport; i++ )
		{
			partitions->at( exportToPart[ i ] ).push_back( lexical_cast< uint32_t >( exportGlobalGids[ i ] ) );

			for( uint32_t j = 0; j < partitions->at( 0 ).size(); j++ )
			{
				if( partitions->at( 0 ).at( j ) == exportGlobalGids[ i ] )
				{
					partitions->at( 0 ).erase( partitions->at( 0 ).begin() + j );
					break;
				}
			}
		}
	}

	/*
	 * Erase all partitions with size 0
	 */
	for( uint32_t j = 0; ; )
	{
		if( partitions->at( j ).size() == 0 )
		{
			partitions->erase( partitions->begin() + j );
			j = 0;
		}
		else
		{
			j++;
		}

		if( j == partitions->size() )
			break;
	}

	for( uint32_t j = 0; j < partitions->size(); j++ )
	{
		parts->push_back( partitions->at( j ).size() );
	}

	// cleanup
	Zoltan_LB_Free_Part( &importGlobalGids, &importLocalGids, &importProcs, &importToPart );
	Zoltan_LB_Free_Part( &exportGlobalGids, &exportLocalGids, &exportProcs, &exportToPart );

	return 0;
}

/*
 * Topology Graph specific query functions
 */
int ZoltanInterface :: GetNoOfTopoNodes( void *data, int *ierr )
{
	TopoGen *topo_graph_obj = ( TopoGen * ) data;

	*ierr = ZOLTAN_OK;
	return num_vertices( topo_graph_obj->mesh );
}

void ZoltanInterface :: GetTopoNodeList( void *data, int size_gid, int size_lid,
										ZOLTAN_ID_PTR global_id, ZOLTAN_ID_PTR local_id,
										int wgt_dim, float *obj_wgts, int *ierr )
{
	TopoGen *topo_graph_obj = ( TopoGen * ) data;

	uint32_t id_index = 0;
	uint32_t wgt_index = 0;
	BGL_FORALL_VERTICES( v, topo_graph_obj->mesh, TopologyGraph )
	{
		global_id[ id_index ] = topo_graph_obj->mesh[ v ].id;
		local_id[ id_index ] = topo_graph_obj->mesh[ v ].id;

		id_index++;

		for( uint32_t i = 0; i < topo_graph_obj->mesh[ v ].constraint.size(); i++  )
			obj_wgts[ wgt_index++ ] = topo_graph_obj->mesh[ v ].constraint[ i ];

		//wgt_dim = topo_graph_obj->mesh[ v ].constraint.size();
	}

	*ierr = ZOLTAN_OK;
}

void ZoltanInterface :: GetNumTopoEdges( void *data, int size_gid, int size_lid,
										int num_obj, ZOLTAN_ID_PTR global_id, ZOLTAN_ID_PTR local_id,
										int *num_edges, int *ierr )
{
	TopoGen *topo_graph_obj = ( TopoGen * ) data;

	if( ( size_gid != 1 ) || ( size_lid != 1 ) )
	{
		*ierr = ZOLTAN_FATAL;
		return;
	}

	uint32_t id_index = 0;
	BGL_FORALL_VERTICES( v, topo_graph_obj->mesh, TopologyGraph )
	{
		graph_traits< TopologyGraph >::out_edge_iterator out_i, out_end;
		int no_of_edges = 0;
		for( tie( out_i, out_end ) = out_edges( v, topo_graph_obj->mesh ); out_i != out_end; ++out_i)
		{
			no_of_edges++;
		}

		num_edges[ id_index++ ] = no_of_edges;
	}
}

void ZoltanInterface :: GetTopoEdgeList( void *data, int size_gid, int size_lid, int num_obj,
										ZOLTAN_ID_PTR global_id, ZOLTAN_ID_PTR local_id,
										int *num_edges, ZOLTAN_ID_PTR nbor_gid, int *nbor_proc,
										int wgt_dim, float *ewgts, int *ierr )
{
	TopoGen *topo_graph_obj = ( TopoGen * ) data;

	ZOLTAN_ID_PTR next_id = nbor_gid;
	int *next_proc = nbor_proc;
	float *next_wght = ewgts;

	if( ( size_gid != 1 ) || ( size_lid != 1 ) || ( wgt_dim != 1 ) )
	{
		*ierr = ZOLTAN_FATAL;
		return;
	}

	uint32_t id_index = 0;
	BGL_FORALL_VERTICES( v, topo_graph_obj->mesh, TopologyGraph )
	{
		graph_traits< TopologyGraph >::out_edge_iterator out_i, out_end;
		graph_traits< TopologyGraph >::edge_descriptor e;
		int no_of_edges = 0;
		for( tie( out_i, out_end ) = out_edges( v, topo_graph_obj->mesh ); out_i != out_end; ++out_i)
		{
			no_of_edges++;
			*next_proc++ = 0; // All nodes are on the first process
			e = *out_i; //assign edge desciptor to iterator

			*next_id++ = topo_graph_obj->mesh[ target( e, topo_graph_obj->mesh ) ].id; // array containing ids of the nieghbours
			*next_wght++ = topo_graph_obj->mesh[ e ].bandwidth; // array containting weight of each edge
		}

		if( num_edges[ id_index ] != no_of_edges )
		{
			*ierr = ZOLTAN_FATAL;
			return;
		}

		id_index++;
	}

	*ierr = ZOLTAN_OK;

	return;
}


#endif
