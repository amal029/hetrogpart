// Heterogeneous Architecture GRaph Partitioner --- 1.0.0.1, Aug 2012
// 16.08.2012

/** @file TopologyDendogram.cpp
 *
 */

#ifndef TOPOLOGY_DENDOGRAM_CPP
#define TOPOLOGY_DENDOGRAM_CPP

#include "TopologyDendogram.hpp"

/*
 *  @brief Contains actual code of respective functions
 *         present in the header.
 *
 *  No detailed comments will be present, refer to the header
 *  for any documentation regarding the class and its inherent
 *  functions and attributes.
 */

TopologyDendogram::TopologyDendogram()
{
	dendogram_height = 0;
}

void TopologyDendogram::CalculateDendogramHeight( uint32_t number_of_nodes )
{
	dendogram_height = floor( log2( number_of_nodes ) );
}

void TopologyDendogram::CreateTopologyGraph( int dim1, int dim2,
												vector< vector< vector< vector< float_t >* >* >* > *tp_wgts_vector,
												vector< vector< vector< uint32_t >* >* > *parts_vector,
												//MPI_Comm mpi_comm,
												int argc, char **argv )
{
	topo_graph.Create( dim1, dim2 );
	CalculateDendogramHeight( topo_graph.no_of_nodes );

	vector< uint32_t > parts_req;
	parts_req.push_back( topo_graph.no_of_nodes );

	for( uint32_t i = 1; i < dendogram_height; i++ )
	{
		parts_req.push_back( topo_graph.no_of_nodes / ( i * 2 ) );
	}
	parts_req.push_back( 1 );
	dendogram_height++;

	BuildDendogram( parts_req, tp_wgts_vector, parts_vector, argc, argv );
}

//BuildDendogram()
int32_t TopologyDendogram :: BuildDendogram( vector< uint32_t > parts_req,
												vector< vector< vector< vector< float_t >* >* >* > *tp_wgts_vector,
												vector< vector< vector< uint32_t >* >* > *parts_vector,
												//MPI_Comm mpi_comm,
												int argc, char **argv )
{
	cout << endl << "INFO: Generating Topology Dendogram of Parts : ";
	for( uint32_t i = 0; i < parts_req.size(); i++ )
	{
		cout << parts_req[ i ] << " ";
	}
	cout << endl;

	MetisInterface mtn( GRAPH_K_WAY );

	prev_level_graph = &topo_graph;
	for( uint32_t level = 0; level < dendogram_height; level++ )
	{
		float_t min_dev_compute_power = -1;
		//cout << "Computing topology for level " << level << endl;

		uint32_t permute = 0;
		uint32_t c_permute = 0;

		do
		{
			//ZoltanInterface ztn;
			TopoGen *topo_graph = NULL;
			vector< uint32_t > *temp_level_parts = NULL;
			vector< vector< uint32_t > > parts( parts_req[ level ] );
			float_t temp_power = -1;

			//ztn.Create( mpi_comm, argc, argv );
			//ztn.SetParmetis();

			temp_level_parts = new vector< uint32_t >;

			//ztn.PartitionGraph( prev_level_graph, parts_req[ level ], &parts, temp_level_parts );
			mtn.PartitionGraph( prev_level_graph, parts_req[ level ], &parts, temp_level_parts );
			topo_graph = new TopoGen();

			if( parts_req[ level ] != temp_level_parts->size() )
			{
				cout << "INFO: Req=" << parts_req[ level ] << " Got=" << temp_level_parts->size() << " of each " ;
				for( uint32_t i = 0; i < temp_level_parts->size(); i++ )
				{
					cout << temp_level_parts->at( i ) << " ";
				}
				cout << endl;
			}

			if( prev_level_graph->GenerateTopoSubGraph( &parts, topo_graph ) )
			{
				temp_power = topo_graph->DeviationInComputePower();

				if( ( min_dev_compute_power == -1 && temp_power != -1 ) ||
					( temp_power != -1 && temp_power < min_dev_compute_power ) )
				{
					if( min_dev_compute_power != -1 )
						cout << "INFO: Better permutation found " << permute << " of pow " << temp_power << endl;

					min_dev_compute_power = temp_power;
					curr_level_graph = topo_graph;
					c_permute = permute;
				}
				else
				{
					delete topo_graph;
					delete temp_level_parts;
				}
			}

			//cout << "Completed permutation " << permute << " of pow " << temp_power << endl;
			permute++;

			//ztn.DestroyZoltan();
		}
		while( prev_level_graph->PermuteConstraints( NULL ) );
		prev_level_graph->ResetConstraints();

		cout << "INFO: Chose permutation " << c_permute << " at level " << level << " of pow " << min_dev_compute_power << endl;

		curr_level_graph->level = level;
		topo_graph_vector.push_back( curr_level_graph );
		prev_level_graph = curr_level_graph;
	}

	for( uint32_t i = 0; i < topo_graph_vector.size(); i++ )
	{
		topo_graph_vector[ i ]->PrintGraphViz( "Level_" + lexical_cast< string >( i ) + "_TopoGraph.dot" );
	}

	//vector< vector< vector< vector< float_t >* >* >* > tp_wgts_vector;
	//vector< vector< vector< uint32_t >* >* > parts_vector;

	GenTpWgts( tp_wgts_vector, parts_vector );

	return 1;
}
//BuildDendogram()

void TopologyDendogram :: GenTpWgts( vector< vector< vector< vector< float_t >* >* >* > *tp_wgts_vector, vector< vector< vector< uint32_t >* >* > *parts_vector )
{
	cout << endl << "INFO: Generating TP Weights" << endl;
	for( uint32_t i = ( topo_graph_vector.size() - 1 ); i != 0; i-- )
	{
		/*
		 * tp_wgts dims;
		 * 1 - diff tp wgts
		 * 2 - diff nodes
		 * 3 - diff parts
		 * 4 - diff levels
		 */
		vector< vector< vector< float_t >* >* > *tp_wgts;

		/*
		 * parts dims
		 * 1 - diff parts
		 * 2 - diff levels
		 * 3 - diff nodes
		 */
		vector< vector< uint32_t >* > *parts;

		tp_wgts = new vector< vector< vector< float_t >* >* >;
		parts = new vector< vector< uint32_t >* >;

		topo_graph_vector.at( i - 1 )->GenTpWgts( topo_graph_vector.at( i ), tp_wgts, parts );

		tp_wgts_vector->push_back( tp_wgts );
		parts_vector->push_back( parts );
	}

	cout << endl << "Topology Partitions :" << endl;

	for( uint32_t k = 0; k < parts_vector->size(); k++ )
	{
		cout << "L" << k << " ";
		for( uint32_t i = 0; i < parts_vector->at( k )->size(); i++ )
		{
			cout << "P" << i << " [";
			for( uint32_t j = 0; j < parts_vector->at( k )->at( i )->size(); j++ )
			{
				cout << " N" << parts_vector->at( k )->at( i )->at( j );// << " ";
			}
			cout << " ]";

			if( i < ( parts_vector->at( k )->size() - 1 ) )
				cout << " || ";
		}
		cout << endl;
	}

	tp_wgts_p = tp_wgts_vector;
	parts_p = parts_vector;
}

void TopologyDendogram :: GenMapping( vector< vector< ApplicationGraph* > > *app_dendogram_obj )
{
	vector < uint32_t > n_id;
	for( uint32_t j = 0; j < parts_p->at( parts_p->size() - 1 )->size(); j++ )
	{
		for( uint32_t k = 0; k < parts_p->at( parts_p->size() - 1 )->at( j )->size(); k++ )
		{
			n_id.push_back( parts_p->at( parts_p->size() - 1 )->at( j )->at( k ) );
		}
	}

	if( n_id.size() !=  app_dendogram_obj->at( app_dendogram_obj->size() - 1 ).size() )
	{
		cout << "Error: There is something obviously wrong!!! NO_NODES=" << n_id.size();
		cout << " NO_APP_PARTS=" << app_dendogram_obj->at( app_dendogram_obj->size() - 1 ).size() << endl;
	}

	for( uint32_t i = 0; i < app_dendogram_obj->at( app_dendogram_obj->size() - 1 ).size(); i++ )
	{
		vector < uint32_t > o_ids;
		BGL_FORALL_VERTICES( v, topo_graph_vector[ 0 ]->mesh, TopologyGraph )
		{
			if( topo_graph_vector[ 0 ]->mesh[ v ].id == n_id[ i ] )
			{
				o_ids = topo_graph_vector[ 0 ]->mesh[ v ].ids;
				break;
			}
		}

		if( o_ids.size() > 1 )
		{
			cout << "INFO: Selecting node id " << o_ids[ 0 ];
			cout << " out of " << o_ids.size() << " nodes" << endl;
		}

		if( o_ids.size() < 1 )
		{
			cout << "ERROR: No original ids found" << endl;
			continue;
		}

		BGL_FORALL_VERTICES( v, topo_graph.mesh, TopologyGraph )
		{
			if( topo_graph.mesh[ v ].id == o_ids[ 0 ] )
			{
				topo_graph.mesh[ v ].compute_req = app_dendogram_obj->at( app_dendogram_obj->size() - 1 ).at( i )->const_wgt_tot;
				BGL_FORALL_VERTICES( a, app_dendogram_obj->at( app_dendogram_obj->size() - 1 ).at( i )->app_graph, ApplicationGraphType )
				{
					topo_graph.mesh[ v ].app_ids.push_back( app_dendogram_obj->at( app_dendogram_obj->size() - 1 ).at( i )->app_graph[ a ].id );
				}
				break;
			}
		}
	}

	ApplicationGraph *app = app_dendogram_obj->at( 0 ).at( 0 );

	BGL_FORALL_VERTICES( v, topo_graph.mesh, TopologyGraph )
	{
		for( uint32_t i = 0; i < topo_graph.mesh[ v ].app_ids.size(); i++ )
		{
			BGL_FORALL_VERTICES( a_v, app->app_graph, ApplicationGraphType )
			{
				if( app->app_graph[ a_v ].id == topo_graph.mesh[ v ].app_ids[ i ] )
				{
					BGL_FORALL_OUTEDGES( v, e, topo_graph.mesh, TopologyGraph )
					{
						BGL_FORALL_OUTEDGES( a_v, a_e, app->app_graph, ApplicationGraphType )
						{
							for( uint32_t j = 0; j < topo_graph.mesh[ target( e, topo_graph.mesh ) ].app_ids.size(); j++ )
							{
								if( app->app_graph[ target( a_e, app->app_graph ) ].id == topo_graph.mesh[ target( e, topo_graph.mesh ) ].app_ids[ j ] )
								{
									topo_graph.mesh[ v ].comm_time += app->app_graph[ a_e ].weight / topo_graph.mesh[ e ].bandwidth;
									break;
								}
							}
						}
					}
				}
			}
		}
	}

	double_t tot_time = 0;
	BGL_FORALL_VERTICES( v, topo_graph.mesh, TopologyGraph )
	{
		uint32_t compute_req = 1;
		for( uint32_t i = 0; i < topo_graph.mesh[ v ].compute_req.size(); i++ )
		{
			compute_req *= topo_graph.mesh[ v ].compute_req[ i ];
		}

		topo_graph.mesh[ v ].comp_time = compute_req / ( topo_graph.mesh[ v ].constraint[ MIPS ] * topo_graph.mesh[ v ].constraint[ VEC ] );

		if( ( topo_graph.mesh[ v ].comp_time + topo_graph.mesh[ v ].comm_time ) > tot_time )
		{
			tot_time = ( topo_graph.mesh[ v ].comp_time + topo_graph.mesh[ v ].comm_time );
		}

		topo_graph.mesh[ v ].label += " A_IDs:";
		for( uint32_t i = 0; i < topo_graph.mesh[ v ].app_ids.size(); i++ )
		{
			topo_graph.mesh[ v ].label +=  " " + lexical_cast< string >( topo_graph.mesh[ v ].app_ids[ i ] );
		}

		topo_graph.mesh[ v ].label += " COMP_T=" + lexical_cast< string >( topo_graph.mesh[ v ].comp_time );
		topo_graph.mesh[ v ].label += " COMM_T=" + lexical_cast< string >( topo_graph.mesh[ v ].comm_time );
	}

	topo_graph.PrintGraphViz( "Level_Topo_App_Map.dot" );

	cout << "Hurray!!! Everything Done!!! " << endl;
	cout << "The max app latency is " << lexical_cast< uint32_t >( tot_time )<< endl;
}

#endif
