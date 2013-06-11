// Heterogeneous Architecture GRaph Partitioner --- 1.0.0.1, Aug 2012
// 16.08.2012

/** @file MetisInterface.cpp
 *
 */

#ifndef METIS_INTERFACE_CPP
#define METIS_INTERFACE_CPP

#include "MetisInterface.hpp"

extern "C" {
idx_t* gpmetis( int argc, char *argv[] );
}

/*
 *  @brief Contains actual code of respective functions
 *         present in the header.
 *
 *  No detailed comments will be present, refer to the header
 *  for any documentation regarding the class and its inherent
 *  functions and attributes.
 */

MetisInterface :: MetisInterface( PartionerType type_p )
{
	type = type_p;
}

int32_t MetisInterface :: PartitionGraph( TopoGen *topo_graph, uint32_t nparts,
							vector< vector< uint32_t > > *partitions, vector< uint32_t > *parts )
{
	if( nparts < 1 )
	{
		cout << "ERROR: No of partitions requested is invalid" << endl;
		return -1;
	}

	BGL_FORALL_VERTICES( v, topo_graph->mesh, TopologyGraph )
	{
		partitions->at( 0 ).push_back( topo_graph->mesh[ v ].id );
	}

	if( nparts == 1 )
	{
		parts->push_back( partitions->at( 0 ).size() );
		return 1;
	}

	if( num_edges( topo_graph->mesh ) <= 1 ||
			num_vertices( topo_graph->mesh ) <= 1 )
	{
		cout << "ERROR: Cannot partition a graph with " << num_edges( topo_graph->mesh ) 
                << " edges or " << num_vertices( topo_graph->mesh ) << " vertices" << endl;

        parts->push_back( partitions->at( 0 ).size() );
		return 1;
	}

	/*
	vector< idx_t > xadj;
	vector< idx_t > adjncy;
	vector< idx_t > vwgt;
	vector< idx_t > adjwgt;

	uint32_t index = 0;
	BGL_FORALL_VERTICES( v, topo_graph->mesh, TopologyGraph )
	{
		for( uint32_t i = 0; i < topo_graph->mesh[ v ].constraint.size(); i++ )
			vwgt.push_back( ( idx_t ) topo_graph->mesh[ v ].constraint[ i ] );

		xadj.push_back( index );
		BGL_FORALL_OUTEDGES( v, e, topo_graph->mesh, TopologyGraph )
		{
			adjncy.push_back( ( idx_t ) topo_graph->mesh[ target( e, topo_graph->mesh ) ].id );
			adjwgt.push_back( ( idx_t ) topo_graph->mesh[ e ].bandwidth );
			index++;
		}
	}
	*/

	vector< idx_t > part;

	//if( MetisPart( topo_graph, nparts, &xadj, &adjncy, &vwgt, &adjwgt, NULL, &part ) < 1 )
	if( MetisPart( topo_graph, nparts, " ", &part ) < 1 )
	{
		cout << "ERROR: Metis Partition Failed" << endl;
		return -1;
	}

	for( uint32_t i = 0; i < part.size(); i++ )
	{
		for( uint32_t j = 0; j < partitions->at( 0 ).size(); j++ )
		{
			if( partitions->at( 0 ).at( j ) == i )
			{
				partitions->at( 0 ).erase( partitions->at( 0 ).begin() + j );
				break;
			}
		}
		partitions->at( part[ i ] ).push_back( lexical_cast< uint32_t >( i ) );
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

	return 1;
}

int32_t MetisInterface :: MetisPart( TopoGen *topo_graph, uint32_t nparts,
										string options, vector< idx_t > *partitions )
{
	string f1 = "tmp.grf";
	vector< string > argv;
	int argc = 1;

	topo_graph->GenerateMetisFile( f1 );
	argv.push_back( f1 );
	argc++;

	argv.push_back( lexical_cast< string >( nparts ) );
	argc++;

    //argv.push_back( " -ptype=rb " ); 
    //argc++;

    //argv.push_back( " -ncuts=100 " );
    //argc++;

	char *arg[] = { ( char* ) "abc", ( char* ) argv[ 0 ].c_str(), ( char* ) argv[ 1 ].c_str() };
	cout << "INFO: TG Metis call: " << argv[ 0 ].c_str() << " " << argv[ 1 ].c_str() << /*", No of args " << argc <<*/ ", ret ";

	idx_t *r_part = gpmetis( argc, arg );

	if( r_part == NULL )
	{
		cout << "ERROR: GPmetis failed!!!" << endl;
		return -1;
	}

	cout << "Parts: ";
	for( uint32_t i = 0; i < topo_graph->no_of_nodes; i++ )
	{
		cout << r_part[ i ] << " ";
		partitions->push_back( r_part[ i ] );
	}
	cout << endl;

	free( r_part );

	return 1;
}

int32_t MetisInterface :: PartitionGraph( ApplicationGraph *app_graph_obj, uint32_t nparts,
											vector< vector< float >* > *tp_wgts, vector< vector< uint32_t > > *partitions )
{
	if( nparts < 1 )
	{
		cout << "ERROR: No of partitions requested is invalid" << endl;
		return -1;
	}

	vector< vector< uint32_t > > d_partitions( nparts );

	BGL_FORALL_VERTICES( v, app_graph_obj->app_graph, ApplicationGraphType )
	{
		partitions->at( 0 ).push_back( app_graph_obj->app_graph[ v ].id );
		d_partitions.at( 0 ).push_back( app_graph_obj->app_graph[ v ].d_id );
	}


	if( nparts == 1 )
	{
		cout << "INFO: Requesting a single partition" << endl;
		return 1;
	}


	vector< real_t > tpwgts;

	if( //num_edges( app_graph_obj->app_graph ) < 1 ||
			num_vertices( app_graph_obj->app_graph ) < 1 )
	{
		cout << "ERROR: Cannot partition a graph with zero edges or vertices, found "
			<< num_vertices( app_graph_obj->app_graph ) << "Vertices "
			<< num_edges( app_graph_obj->app_graph ) << " Edges"<< endl;

		return -1;
	}

    uint32_t skip_partitioning = 0;

	if( tp_wgts->size() > 0 )
	//if( 0 )
	{
		if( app_graph_obj->SetTpWgts( tp_wgts, &tpwgts ) )
		{
			cout << "tpwgts : " ;
			for( uint32_t i = 0; i < tpwgts.size(); i++ )
			{
                if( tpwgts[ i ] == 0.00000 || tpwgts[ i ] == 1.00000 )
                    skip_partitioning = 1;

				cout << setprecision(5) << tpwgts[ i ] << " ";
			}
			cout << endl;
		}
		else
		{
			cout << "ERROR: Failed to get tp wgts" << endl;
			return -1;
		}
	}

	vector< idx_t > part;

    if( !skip_partitioning )
    {
        if( MetisPart( app_graph_obj, nparts, &tpwgts, " ", &part ) < 1 )
        {
            cout << "ERROR: Metis Partition Failed" << endl;
            return -1;
        }
    }
    else
    {
        uint32_t find_part = 0;
        uint32_t n_cnstr = app_graph_obj->no_of_constraints;
        for( uint32_t i = 0; i < tpwgts.size(); i++ )
        {
            if( tpwgts.at( i ) == 1.0000 )
            {
                find_part = i / n_cnstr;
                break;
            }
        }

        for( uint32_t i = 0; i < num_vertices( app_graph_obj->app_graph ); i++ )
        {
            part.push_back( find_part );
        }
    }

	for( uint32_t i = 0; i < part.size(); i++ )
	{
		for( uint32_t j = 0; j < d_partitions.at( 0 ).size(); j++ )
		{
			if( d_partitions.at( 0 ).at( j ) == ( i + 1 ) )
			{
				partitions->at( part[ i ] ).push_back( partitions->at( 0 ).at( j ) );
				d_partitions.at( part[ i ] ).push_back( d_partitions.at( 0 ).at( j ) );

				d_partitions.at( 0 ).erase( d_partitions.at( 0 ).begin() + j );
				partitions->at( 0 ).erase( partitions->at( 0 ).begin() + j );
				break;
			}
		}
	}

	return 1;
}

int32_t MetisInterface :: MetisPart( ApplicationGraph *app_graph, uint32_t nparts,
										vector< real_t > *tpwgts, string options, vector< idx_t > *partitions )
{
	string f1 = "tmp.grf";
	string f2 = "tpw.grf";
	vector< string > argv;
	int argc = 1;

	if( tpwgts != NULL && tpwgts->size() > 0 )
	{
		ofstream tpwgt_file;
		tpwgt_file.open( f2 );

		uint32_t n_cnstr = app_graph->no_of_constraints;
		uint32_t index = 0;
		uint32_t c_index = 0;
		for( uint32_t i = 0; i < tpwgts->size(); i++ )
		{
			tpwgt_file << fixed << index << ":" << c_index << " = " << setprecision(10) << tpwgts->at( i ) << endl;

			if( c_index < ( n_cnstr - 1 ) )
			{
				c_index++;
			}
			else
			{
				c_index = 0;
				index++;
			}
		}

		tpwgt_file.close();
	}

	argv.push_back( "-tpwgts=" + f2 );
	argc++;

	if( num_edges( app_graph->app_graph ) < 1 )
		app_graph->GenerateMetisFileAddDummyNode( f1 );
	else
		app_graph->GenerateMetisFile( f1 );
	argv.push_back( f1 );
	argc++;

	argv.push_back( lexical_cast< string >( nparts ) );
	argc++;

	char **arg = new char*[ 4 * sizeof( char * ) ];
	arg[ 0 ] = new char[ 20 * sizeof( char ) ];
	arg[ 1 ] = new char[ ( argv[ 0 ].size() + 1 ) * sizeof( char ) ];
	arg[ 2 ] = new char[ ( argv[ 1 ].size() + 1 ) * sizeof( char ) ];
	arg[ 3 ] = new char[ ( argv[ 2 ].size() + 1 ) * sizeof( char ) ];

	strcpy( arg[ 0 ], "gpmetis" );
	strcpy( arg[ 1 ], argv[ 0 ].c_str() );
	strcpy( arg[ 2 ], argv[ 1 ].c_str() );
	strcpy( arg[ 3 ], argv[ 2 ].c_str() );

	arg[ 1 ][ ( argv[ 0 ].size() + 1 ) ] = '\0';
	arg[ 2 ][ ( argv[ 1 ].size() + 1 ) ] = '\0';
	arg[ 3 ][ ( argv[ 2 ].size() + 1 ) ] = '\0';


	//char *arg[] = {  ( char* ) "abc", ( char* ) argv[ 0 ].c_str(), ( char* ) argv[ 1 ].c_str(), ( char* ) argv[ 2 ].c_str() };
	cout << "INFO: AG Metis call: " << arg[ 0 ] << " " << arg[ 1 ] << " "<< arg[ 2 ] << " " << arg[ 3 ] << /*", No of args " << argc <<*/ endl;

	idx_t *r_part = gpmetis( argc, arg );

	if( r_part == NULL )
	{
		cout << "ERROR: GPmetis failed!!!" << endl;
		return -1;
	}

	//~ cout << "Parts: ";
	for( uint32_t i = 0; i < app_graph->no_of_vertices; i++ )
	{
		//~ cout << r_part[ i ] << " ";
		partitions->push_back( r_part[ i ] );
	}
	//~ cout << endl;

	//cout << "finished metis" << endl;

	free( r_part );

	delete[] arg[ 0 ];
	delete[] arg[ 1 ];
	delete[] arg[ 2 ];
	delete[] arg[ 3 ];
	delete[] arg;

	return 1;
}


/*
int32_t MetisInterface :: MetisPart( TopoGen *topo_graph, uint32_t nparts,
										vector< idx_t > *xadj, vector< idx_t > *adjncy, vector< idx_t > *vwgt,
										vector< idx_t > *adjwgt, vector< real_t > *tpwgts, vector< idx_t > *partitions )
{
	idx_t options[ METIS_NOPTIONS ];
	//METIS_SetDefaultOptions( options );
	options[ METIS_OPTION_DBGLVL ] = METIS_DBG_INFO | METIS_DBG_TIME | METIS_DBG_COARSEN |
										METIS_DBG_REFINE | METIS_DBG_IPART | METIS_DBG_MOVEINFO |
										METIS_DBG_SEPINFO | METIS_DBG_CONNINFO | METIS_DBG_CONTIGINFO;

	idx_t nvtx = ( idx_t ) topo_graph->no_of_nodes;
	idx_t ncon = ( idx_t ) topo_graph->no_of_constraints;
	idx_t npts = ( idx_t ) nparts;

	idx_t *vsize = ( idx_t* ) calloc( nvtx, sizeof( idx_t ) );
	real_t *ubvec = NULL;

	idx_t objval;
	idx_t *part_v = ( idx_t* ) calloc( nvtx, sizeof( idx_t ) );

	idx_t *xadj_v = ( idx_t* ) calloc( xadj->size(), sizeof( idx_t ) );//new idx_t[ xadj.size() ];
	idx_t *adjncy_v = ( idx_t* ) calloc( adjncy->size(), sizeof( idx_t ) );
	idx_t *vwgt_v = ( idx_t* ) calloc( vwgt->size(), sizeof( idx_t ) );
	idx_t *adjwgt_v = ( idx_t* ) calloc( adjwgt->size(), sizeof( idx_t ) );

	memcpy( xadj_v, &xadj->front(), sizeof( idx_t ) * xadj->size() );
	memcpy( adjncy_v, &adjncy->front(), sizeof( idx_t ) * adjncy->size() );
	memcpy( vwgt_v, &vwgt->front(), sizeof( idx_t ) * vwgt->size() );
	memcpy( adjwgt_v, &adjwgt->front(), sizeof( idx_t ) * adjwgt->size() );

	real_t *tpwgts_v = NULL;

	if( tpwgts != NULL )
	{
		real_t *tpwgts_v = ( real_t* ) calloc( tpwgts->size(), sizeof( real_t ) );
		memcpy( tpwgts_v, &tpwgts->front(), sizeof( idx_t ) * tpwgts->size() );
	}

	if( type == GRAPH_K_WAY )
	{
		options[ METIS_OPTION_OBJTYPE ] = METIS_OBJTYPE_CUT;
		options[ METIS_OPTION_CTYPE ] = METIS_CTYPE_SHEM;
		options[ METIS_OPTION_IPTYPE ] = METIS_IPTYPE_METISRB;
		options[ METIS_OPTION_RTYPE ] = METIS_RTYPE_GREEDY;
		options[ METIS_OPTION_NCUTS ] = 1;
		options[ METIS_OPTION_NITER ] = 10;
		options[ METIS_OPTION_UFACTOR ] = 30;
		options[ METIS_OPTION_MINCONN ] = 0;
		options[ METIS_OPTION_CONTIG ] = 0;
		options[ METIS_OPTION_SEED ] = -1;
		options[ METIS_OPTION_NUMBERING ] = 0;


		cout << "ERROR: Metis k-Way Partitioning Failed" << endl;
		cout << "nvtx: " << nvtx << "  ncon: " << ncon << "  npts: " << npts << endl;
		cout << "adjncy: ";
		for( uint32_t i = 0; i < adjncy->size(); i++ )
		{
			cout << adjncy_v[ i ] << " ";
		}
		cout << endl;

		cout << "xadj: ";
		for( uint32_t i = 0; i < xadj->size(); i++ )
		{
			cout << xadj_v[ i ] << " ";
		}
		cout << endl;

		cout << "vwgt: ";
		for( uint32_t i = 0; i < vwgt->size(); i++ )
		{
			cout << vwgt_v[ i ] << " ";
		}
		cout << endl;

		cout << "adjwgt: ";
		for( uint32_t i = 0; i < adjwgt->size(); i++ )
		{
			cout << adjwgt_v[ i ] << " ";
		}
		cout << endl;

		cout << "part: ";
		for( int32_t i = 0; i < nvtx; i++ )
		{
			cout << part_v[ i ] << " ";
		}
		cout << endl;
		fflush( stdout );


		if( METIS_OK != METIS_PartGraphKway( &nvtx, &ncon,  xadj_v, adjncy_v, vwgt_v, vsize,
												adjwgt_v, &npts, tpwgts_v, ubvec,
												options, &objval, part_v ) )
		{
			cout << "ERROR: Metis k-Way Partitioning Failed" << endl;
			cout << "nvtx: " << nvtx << "  ncon: " << ncon << "  npts: " << npts << endl;
			cout << "adjncy: ";
			for( uint32_t i = 0; i < adjncy->size(); i++ )
			{
				cout << adjncy_v[ i ] << " ";
			}
			cout << endl;

			cout << "xadj: ";
			for( uint32_t i = 0; i < xadj->size(); i++ )
			{
				cout << xadj_v[ i ] << " ";
			}
			cout << endl;

			cout << "vwgt: ";
			for( uint32_t i = 0; i < vwgt->size(); i++ )
			{
				cout << vwgt_v[ i ] << " ";
			}
			cout << endl;

			cout << "adjwgt: ";
			for( uint32_t i = 0; i < adjwgt->size(); i++ )
			{
				cout << adjwgt_v[ i ] << " ";
			}
			cout << endl;

			cout << "part: ";
			for( int32_t i = 0; i < nvtx; i++ )
			{
				cout << part_v[ i ] << " ";
			}
			cout << endl;
			fflush( stdout );

			return -1;
		}

		cout << "INFO: Metis k-Way Partitioning Successful with edge-cuts " << objval << endl;
	}
	else if( type == GRAPH_RECURSIVE )
	{
		//options[ METIS_OPTION_CTYPE ] =
		//options[ METIS_OPTION_IPTYPE ] =
		//options[ METIS_OPTION_RTYPE ] =
		//options[ METIS_OPTION_NCUTS ] =
		//options[ METIS_OPTION_NITER ] =
		//options[ METIS_OPTION_SEED ] =
		//options[ METIS_OPTION_UFACTOR ] =
		//options[ METIS_OPTION_NUMBERING ] =

		if( METIS_OK != METIS_PartGraphRecursive( &nvtx, &ncon, xadj_v, adjncy_v, vwgt_v, vsize,
													adjwgt_v, &npts, tpwgts_v, ubvec,
													options, &objval, part_v ) )
		{
			cout << "ERROR: Metis Recursive Partitioning Failed" << endl;
			cout << "nvtx: " << nvtx << "  ncon: " << ncon << "  npts: " << npts << endl;
			cout << "adjncy: ";
			for( uint32_t i = 0; i < adjncy->size(); i++ )
			{
				cout << adjncy_v[ i ] << " ";
			}
			cout << endl;

			cout << "xadj: ";
			for( uint32_t i = 0; i < xadj->size(); i++ )
			{
				cout << xadj_v[ i ] << " ";
			}
			cout << endl;

			cout << "vwgt: ";
			for( uint32_t i = 0; i < vwgt->size(); i++ )
			{
				cout << vwgt_v[ i ] << " ";
			}
			cout << endl;

			cout << "adjwgt: ";
			for( uint32_t i = 0; i < adjwgt->size(); i++ )
			{
				cout << adjwgt_v[ i ] << " ";
			}
			cout << endl;

			cout << "part: ";
			for( int32_t i = 0; i < nvtx; i++ )
			{
				cout << part_v[ i ] << " ";
			}
			cout << endl;
			fflush( stdout );

			return -1;
		}

		cout << "INFO: Metis Recursive Partitioning Successful with comm vol " << objval << endl;
	}
	else
	{
		cout << "ERROR: Unknown Partitioning Scheme" << endl;
		return -1;
	}

	cout << "Parts: ";
	for( int32_t i = 0; i < nvtx; i++ )
	{
		cout << part_v[ i ] << " ";
		partitions->push_back( part_v[ i ] );
	}
	cout << endl;

	free( adjwgt_v );
	free( xadj_v );
	free( vwgt_v );
	free( adjncy_v );
	free( part_v );

	if( tpwgts_v != NULL )
		free( tpwgts_v );

	return 1;
}
*/


#endif
