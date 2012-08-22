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
int ZoltanInterface :: Create( MPI_Comm mpi_comm, int argc, char* argv[] )
{
	world = communicator( mpi_comm, comm_attach ); // create a boost comm and attach it to mpi_comm
	rank = world.rank();
	world_size = world.rank();

	cout << "Created Process " << rank << " of " << world_size << endl;

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

void ZoltanInterface :: SetParmetis(  ApplicationGraph *app_graph_obj_ref )
{
	app_graph_obj = app_graph_obj_ref;

	zz->Set_Param(  "DEBUG_LEVEL", "0" );
	zz->Set_Param(  "LB_METHOD", "GRAPH" );
	zz->Set_Param(  "LB_APPROACH", "PARTITION" );
	zz->Set_Param(  "NUM_GID_ENTRIES", "1" );
	zz->Set_Param(  "NUM_LID_ENTRIES", "1" );

	zz->Set_Param( "NUM_GLOBAL_PARTS", "3");

	zz->Set_Param(  "OBJ_WEIGHT_DIM", "2" ); //number of weights per object
	zz->Set_Param(  "EDGE_WEIGHT_DIM", "1" ); //number of weights per edge

	//zz->Set_Param(  "RETURN_LISTS", "ALL" ); //return the new process and partition assignment of every local object
	zz->Set_Param(  "PHG_EDGE_SIZE_THRESHOLD", "1.0" );

	zz->Set_Param( "GRAPH_PACKAGE", "PARMETIS");
	zz->Set_Param(  "PARMETIS_METHOD", "PARTKWAY"); //multilevel Kernighan-Lin partitioning
	//zz->Set_Param(  "PARMETIS_COARSE_ALG", "1" ); //serial partitioning algorithm
	zz->Set_Param(  "CHECK_GRAPH", "2"); //full checking
	//zz->Set_Param(  "IMBALANCE_TOL", "1.5"); //node weight imbalance tolerance

	// set call backs for zoltan
	zz->Set_Num_Obj_Fn( ZoltanInterface :: GetNoOfAppNodes, app_graph_obj );
	zz->Set_Obj_List_Fn( ZoltanInterface :: GetAppNodeList, app_graph_obj );
	zz->Set_Num_Edges_Multi_Fn( ZoltanInterface :: GetNumAppEdges, app_graph_obj );
	zz->Set_Edge_List_Multi_Fn( ZoltanInterface :: GetAppEdgeList, app_graph_obj );
}

int ZoltanInterface :: PartitionGraph( ApplicationGraph *app_graph_obj_ref )
{
	int changed, numGidEntries, numLidEntries, numImport, numExport;
	ZOLTAN_ID_PTR importGlobalGids, importLocalGids, exportGlobalGids, exportLocalGids;
	int *importProcs, *importToPart, *exportProcs, *exportToPart;

	app_graph_obj = app_graph_obj_ref;

	int rc = zz->LB_Partition(
							changed, //1 if partitioning was changed, 0 otherwise
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

	cout << "changed=" << changed << " numImport=" << numImport << " numExport=" << numExport;
	cout << " numGidEntries=" << numGidEntries << " numLidEntries=" << numLidEntries << endl;

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

	/*
	cout << "importProcs=";
	for( int i = 0; i < numImport; i++ )
	{
		cout << setw(2) << setfill(' ') << importProcs[ i ] << " ";
	}
	cout << endl;
	*/

	cout << "exportToPart    = ";
	for( int i = 0; i < numExport; i++ )
	{
		cout << setw(2) << setfill(' ') << exportToPart[ i ] << " ";
	}
	cout << endl;

	cout << "exportGlobalGids= ";
	for( int i = 0; i < numExport; i++ )
	{
		cout << setw(2) << setfill(' ') << exportGlobalGids[ i ] << " ";
	}
	cout << endl;

	cout << "exportLocalGids = ";
	for( int i = 0; i < numExport; i++ )
	{
		cout << setw(2) << setfill(' ') << exportLocalGids[ i ] << " ";
	}
	cout << endl;

	/*
	cout << "exportProcs=";
	for( int i = 0; i < numExport; i++ )
	{
		cout << setw(2) << setfill(' ') << exportProcs[ i ] << " ";
	}
	cout << endl;
	*/


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
	cout << "Destroyed zoltan on Process" << rank << endl;
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

void ZoltanInterface :: GetAppNodeList( void *data, int sizeGID, int sizeLID,
							ZOLTAN_ID_PTR globalID, ZOLTAN_ID_PTR localID,
							int wgt_dim, float *obj_wgts, int *ierr )
{
	ApplicationGraph *app_graph_obj = ( ApplicationGraph * ) data;

	uint32_t id_index = 0;
	uint32_t wgt_index = 0;
	BGL_FORALL_VERTICES( v, app_graph_obj->app_graph, ApplicationGraphType )
	{
		globalID[ id_index ] = app_graph_obj->app_graph[ v ].id;
		localID[ id_index ] = app_graph_obj->app_graph[ v ].id;

		id_index++;

		for( uint32_t i = 0; i < app_graph_obj->app_graph[ v ].constraint.size(); i++  )
			obj_wgts[ wgt_index++ ] = app_graph_obj->app_graph[ v ].constraint[ i ];

		//wgt_dim = app_graph_obj->app_graph[ v ].constraint.size();
	}

	*ierr = ZOLTAN_OK;
}

void ZoltanInterface :: GetNumAppEdges( void *data, int sizeGID, int sizeLID,
							int num_obj,
							ZOLTAN_ID_PTR globalID, ZOLTAN_ID_PTR localID,
							int *num_edges, int *ierr )
{
	ApplicationGraph *app_graph_obj = ( ApplicationGraph * ) data;

	if( ( sizeGID != 1 ) || ( sizeLID != 1 ) )
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

void ZoltanInterface :: GetAppEdgeList( void *data, int sizeGID, int sizeLID, int num_obj,
							ZOLTAN_ID_PTR globalID, ZOLTAN_ID_PTR localID,
							int *num_edges, ZOLTAN_ID_PTR nborGID, int *nborProc,
							int wgt_dim, float *ewgts, int *ierr )
{
	ApplicationGraph *app_graph_obj = ( ApplicationGraph * ) data;

	ZOLTAN_ID_PTR nextID = nborGID;
	int *nextProc = nborProc;
	float *nextWght = ewgts;

	if( ( sizeGID != 1 ) || ( sizeLID != 1 ) || ( wgt_dim != 1 ) )
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
			*nextProc++ = 0; // All nodes are on the first process
			e = *out_i; //assign edge desciptor to iterator

			*nextID++ = app_graph_obj->app_graph[ target( e, app_graph_obj->app_graph ) ].id; // array containing ids of the nieghbours
			*nextWght++ = app_graph_obj->app_graph[ e ].weight; // array containting weight of each edge
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
