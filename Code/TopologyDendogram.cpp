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
	time = 0;
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
	orig_topo_graph.Create( dim1, dim2 );
	CalculateDendogramHeight( orig_topo_graph.no_of_nodes );

	vector< uint32_t > parts_req;
	parts_req.push_back( orig_topo_graph.no_of_nodes );
	topo_graph_vector.push_back( &orig_topo_graph );

	for( uint32_t i = 1; i < dendogram_height; i++ )
	{
		parts_req.push_back( orig_topo_graph.no_of_nodes / ( i * 2 ) );
	}

	//parts_req.push_back( orig_topo_graph.no_of_nodes /  2 );
	//dendogram_height = 1;
	//parts_req.push_back( orig_topo_graph.no_of_nodes /  4 );

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

	prev_level_graph = &orig_topo_graph;
	for( uint32_t level = 1; level < dendogram_height; level++ )
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
				orig_topo_graph.SetAccuBw( topo_graph );
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

	/*
	for( uint32_t i = 0; i < topo_graph_vector.size(); i++ )
	{
		topo_graph_vector[ i ]->PrintGraphViz( "Level_" + lexical_cast< string >( i ) + "_TopoGraph.dot" );
	}
	*/

	//vector< vector< vector< vector< float_t >* >* >* > tp_wgts_vector;
	//vector< vector< vector< uint32_t >* >* > parts_vector;

	GenTpWgts( tp_wgts_vector, parts_vector );

	return 1;
}
//BuildDendogram()

void TopologyDendogram :: GenTpWgts( vector< vector< vector< vector< float_t >* >* >* > *tp_wgts_vector, vector< vector< vector< uint32_t >* >* > *parts_vector )
{
	cout << endl << "INFO: Generating TP Weights" << endl;
	vector< uint32_t > prev_order;
	vector< uint32_t > curr_order;

	prev_order.push_back( 0 );
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

		topo_graph_vector.at( i - 1 )->GenTpWgts( topo_graph_vector.at( i ), tp_wgts, parts, &prev_order, &curr_order );

		tp_wgts_vector->push_back( tp_wgts );
		parts_vector->push_back( parts );

		prev_order = curr_order;
		curr_order.clear();
	}

	cout << endl << "Topology Partitions :" << endl;
	/*
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
	*/

	for( uint32_t k = 0; k < parts_vector->size(); k++ )
	{
		cout << "L" << k << " "<< topo_graph_vector.at( topo_graph_vector.size() - 2 - k )->no_of_nodes << " ";
		for( uint32_t i = 0; i < parts_vector->at( k )->size(); i++ )
		{
			cout << "P" << i << " [";
			for( uint32_t j = 0; j < parts_vector->at( k )->at( i )->size(); j++ )
			{
				cout << " N" << parts_vector->at( k )->at( i )->at( j ) << "(";

				BGL_FORALL_VERTICES( v, topo_graph_vector.at( topo_graph_vector.size() - 2 - k )->mesh, TopologyGraph )
				{
					if( topo_graph_vector.at( topo_graph_vector.size() - 2 - k )->mesh[ v ].id == parts_vector->at( k )->at( i )->at( j ) )
					{
						for( uint32_t l = 0; l < topo_graph_vector.at( topo_graph_vector.size() - 2 - k )->mesh[ v ].ids.size(); l++ )
						{
							cout << topo_graph_vector.at( topo_graph_vector.size() - 2 - k )->mesh[ v ].ids[ l ];

							if( l < topo_graph_vector.at( topo_graph_vector.size() - 2 - k )->mesh[ v ].ids.size() - 1 )
								cout << ",";
						}
						break;
					}
				}

				cout << ") ";
			}
			cout << " ]";

			if( i < ( parts_vector->at( k )->size() - 1 ) )
				cout << " || ";
		}
		cout << endl;
	}


	for( int32_t i = topo_graph_vector.size() - 2; i >= 0; i-- )
	{
		//cout << topo_graph_vector.at( i )->no_of_nodes << endl;
		BGL_FORALL_VERTICES( v, topo_graph_vector.at( i )->mesh, TopologyGraph )
		{
			//if( topo_graph_vector.at( i )->mesh)
		}
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

	for( uint32_t i = 0; i < n_id.size(); i++ )
	{
		cout << n_id[ i ] << " ";
	}
	cout << endl;

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
			cout << "ERROR: Selecting node id " << o_ids[ 0 ];
			cout << " out of " << o_ids.size() << " nodes" << endl;

			exit( 1 );
		}

		if( o_ids.size() < 1 )
		{
			cout << "ERROR: No original ids found" << endl;
			continue;
		}

		BGL_FORALL_VERTICES( v, orig_topo_graph.mesh, TopologyGraph )
		{
			if( orig_topo_graph.mesh[ v ].id == o_ids[ 0 ] )
			{
				//orig_topo_graph.mesh[ v ].compute_req = app_dendogram_obj->at( app_dendogram_obj->size() - 1 ).at( i )->const_wgt_tot;
				BGL_FORALL_VERTICES( a, app_dendogram_obj->at( app_dendogram_obj->size() - 1 ).at( i )->app_graph, ApplicationGraphType )
				{
					orig_topo_graph.mesh[ v ].app_ids.push_back( app_dendogram_obj->at( app_dendogram_obj->size() - 1 ).at( i )->app_graph[ a ].id );

					if( app_dendogram_obj->at( app_dendogram_obj->size() - 1 ).at( i )->app_graph[ a ].constraint.size() == 2 )
					{
						orig_topo_graph.mesh[ v ].comp_time += ( app_dendogram_obj->at( app_dendogram_obj->size() - 1 ).at( i )->app_graph[ a ].constraint[ MIPS ] +
										( ceil( app_dendogram_obj->at( app_dendogram_obj->size() - 1 ).at( i )->app_graph[ a ].constraint[ VEC ] /
												( orig_topo_graph.mesh[ v ].constraint[ VEC ] ) ) ) ) /
									( orig_topo_graph.mesh[ v ].constraint[ MIPS ] * CMP_SCALE_FACTOR );

						cout << "A_ID: " << app_dendogram_obj->at( app_dendogram_obj->size() - 1 ).at( i )->app_graph[ a ].id;
						cout << " -> T_ID: " << orig_topo_graph.mesh[ v ].id;
						cout << " C_MIPS: " << ( app_dendogram_obj->at( app_dendogram_obj->size() - 1 ).at( i )->app_graph[ a ].constraint[ MIPS ] );
						cout << " C_VEC: " << app_dendogram_obj->at( app_dendogram_obj->size() - 1 ).at( i )->app_graph[ a ].constraint[ VEC ];
						cout << " T_VEC: " << orig_topo_graph.mesh[ v ].constraint[ VEC ];
						cout << " T_MIPS: " << orig_topo_graph.mesh[ v ].constraint[ MIPS ];
						cout << " C_T: " << ( app_dendogram_obj->at( app_dendogram_obj->size() - 1 ).at( i )->app_graph[ a ].constraint[ MIPS ] +
										( ceil( app_dendogram_obj->at( app_dendogram_obj->size() - 1 ).at( i )->app_graph[ a ].constraint[ VEC ] /
												( orig_topo_graph.mesh[ v ].constraint[ VEC ] ) ) ) ) /
									( orig_topo_graph.mesh[ v ].constraint[ MIPS ] * CMP_SCALE_FACTOR ) << endl;

					}
					else
					{
						orig_topo_graph.mesh[ v ].comp_time += app_dendogram_obj->at( app_dendogram_obj->size() - 1 ).at( i )->app_graph[ a ].constraint[ MIPS ] /
									( orig_topo_graph.mesh[ v ].constraint[ MIPS ] * CMP_SCALE_FACTOR );

						cout << "A_ID: " << app_dendogram_obj->at( app_dendogram_obj->size() - 1 ).at( i )->app_graph[ a ].id;
						cout << " -> T_ID: " << orig_topo_graph.mesh[ v ].id;
						cout << " C_MIPS: " <<
						app_dendogram_obj->at( app_dendogram_obj->size() - 1 ).at( i )->app_graph[ a ].constraint[ MIPS ] << " T_MIPS: " <<
									( orig_topo_graph.mesh[ v ].constraint[ MIPS ] * CMP_SCALE_FACTOR );
						cout << " C_T: " << app_dendogram_obj->at( app_dendogram_obj->size() - 1 ).at( i )->app_graph[ a ].constraint[ MIPS ] /
									( orig_topo_graph.mesh[ v ].constraint[ MIPS ] * CMP_SCALE_FACTOR ) << endl;

					}


					/*
					float_t c_pow = 1;
					for( uint32_t k = 0; k < app_dendogram_obj->at( app_dendogram_obj->size() - 1 ).at( i )->app_graph[ a ].constraint.size(); k++ )
					{
						c_pow *= app_dendogram_obj->at( app_dendogram_obj->size() - 1 ).at( i )->app_graph[ a ].constraint[ k ] == 0
									? 1 : app_dendogram_obj->at( app_dendogram_obj->size() - 1 ).at( i )->app_graph[ a ].constraint[ k ];
					}

					orig_topo_graph.mesh[ v ].compute_req += c_pow;
					*/
				}
				break;
			}
		}
	}

	ApplicationGraph *app = app_dendogram_obj->at( 0 ).at( 0 );

	BGL_FORALL_VERTICES( v, orig_topo_graph.mesh, TopologyGraph )
	{
		for( uint32_t i = 0; i < orig_topo_graph.mesh[ v ].app_ids.size(); i++ )
		{
			vector< pair< uint32_t, float_t > >d_app_ids;
			BGL_FORALL_VERTICES( a_v, app->app_graph, ApplicationGraphType )
			{
				if( app->app_graph[ a_v ].id == orig_topo_graph.mesh[ v ].app_ids[ i ] )
				{
					BGL_FORALL_OUTEDGES( a_v, a_e, app->app_graph, ApplicationGraphType )
					{
						d_app_ids.push_back( make_pair( app->app_graph[ target( a_e, app->app_graph ) ].id,
														app->app_graph[ a_e ].weight ) );
					}
					break;
				}
			}

			for( uint32_t k = 0; k < d_app_ids.size(); k++ )
			{
				bool cond = false;
				BGL_FORALL_VERTICES( v1, orig_topo_graph.mesh, TopologyGraph )
				{
					for( uint32_t j = 0; j < orig_topo_graph.mesh[ v1 ].app_ids.size(); j++ )
					{
						if( d_app_ids[ k ].first == orig_topo_graph.mesh[ v1 ].app_ids[ j ] )
						{
							if( orig_topo_graph.shortest_path->at( orig_topo_graph.mesh[ v ].id ).at( orig_topo_graph.mesh[ v1 ].id ) > 0 )
							{
								orig_topo_graph.mesh[ v ].comm_time += d_app_ids[ k ].second /
											( orig_topo_graph.shortest_path->at( orig_topo_graph.mesh[ v ].id ).at( orig_topo_graph.mesh[ v1 ].id ) * BW_SCALE_FACTOR );

								//cout << "COMM_T: " << d_app_ids[ k ].second /
									//		( orig_topo_graph.shortest_path->at( orig_topo_graph.mesh[ v ].id ).at( orig_topo_graph.mesh[ v1 ].id ) * BW_SCALE_FACTOR ) << endl;
							}
							cond = true;
							break;
						}
					}

					if( cond )
						break;
				}
			}
		}
	}


	/*
	BGL_FORALL_VERTICES( v, orig_topo_graph.mesh, TopologyGraph )
	{
		for( uint32_t i = 0; i < orig_topo_graph.mesh[ v ].app_ids.size(); i++ )
		{
			BGL_FORALL_VERTICES( a_v, app->app_graph, ApplicationGraphType )
			{
				if( app->app_graph[ a_v ].id == orig_topo_graph.mesh[ v ].app_ids[ i ] )
				{
					BGL_FORALL_OUTEDGES( v, e, orig_topo_graph.mesh, TopologyGraph )
					{
						BGL_FORALL_OUTEDGES( a_v, a_e, app->app_graph, ApplicationGraphType )
						{
							for( uint32_t j = 0; j < orig_topo_graph.mesh[ target( e, orig_topo_graph.mesh ) ].app_ids.size(); j++ )
							{
								if( app->app_graph[ target( a_e, app->app_graph ) ].id == orig_topo_graph.mesh[ target( e, orig_topo_graph.mesh ) ].app_ids[ j ] )
								{
									orig_topo_graph.mesh[ v ].comm_time += app->app_graph[ a_e ].weight / orig_topo_graph.mesh[ e ].bandwidth;
									break;
								}
							}
						}
					}
				}
			}
		}
	}
	*/

	fstream out;
	out.open( "Map.out", fstream::out | fstream::app );

	double_t tot_time = 0;
	BGL_FORALL_VERTICES( v, orig_topo_graph.mesh, TopologyGraph )
	{
		/*
		uint32_t compute_req = 1;
		for( uint32_t i = 0; i < orig_topo_graph.mesh[ v ].compute_req.size(); i++ )
		{
			compute_req *= orig_topo_graph.mesh[ v ].compute_req[ i ];
			cout << orig_topo_graph.mesh[ v ].compute_req[ i ] << " ";
		}
		cout << endl;
		*/

		//orig_topo_graph.mesh[ v ].comp_time = orig_topo_graph.mesh[ v ].compute_req / orig_topo_graph.mesh[ v ].compute_power;//( orig_topo_graph.mesh[ v ].constraint[ MIPS ] * orig_topo_graph.mesh[ v ].constraint[ VEC ] );
		//cout << "COMP_T:" << orig_topo_graph.mesh[ v ].compute_req << " " <<  orig_topo_graph.mesh[ v ].compute_power << " " << orig_topo_graph.mesh[ v ].compute_req / orig_topo_graph.mesh[ v ].compute_power << endl;
		//cout << "C: " << orig_topo_graph.mesh[ v ].comp_time << " " << orig_topo_graph.mesh[ v ].comm_time << endl;

		if( ( orig_topo_graph.mesh[ v ].comp_time + orig_topo_graph.mesh[ v ].comm_time ) > tot_time )
		{
			tot_time = ( orig_topo_graph.mesh[ v ].comp_time + orig_topo_graph.mesh[ v ].comm_time );
		}


		out << "N" << orig_topo_graph.mesh[ v ].id + 1 << " <- ";
		orig_topo_graph.mesh[ v ].label += " A_IDs:";
		for( uint32_t i = 0; i < orig_topo_graph.mesh[ v ].app_ids.size(); i++ )
		{
			orig_topo_graph.mesh[ v ].label +=  " " + lexical_cast< string >( orig_topo_graph.mesh[ v ].app_ids[ i ] );
			out <<  orig_topo_graph.mesh[ v ].app_ids[ i ] << ", ";
		}
		out << endl;

		orig_topo_graph.mesh[ v ].label += " COMP_T=" + lexical_cast< string >( orig_topo_graph.mesh[ v ].comp_time );
		orig_topo_graph.mesh[ v ].label += " COMM_T=" + lexical_cast< string >( orig_topo_graph.mesh[ v ].comm_time );

		cout << "ID= " <<  orig_topo_graph.mesh[ v ].id << ", Comp=" <<  orig_topo_graph.mesh[ v ].comp_time << ", Comm=" <<  orig_topo_graph.mesh[ v ].comm_time << endl;

		orig_topo_graph.mesh[ v ].comp_time = 0; orig_topo_graph.mesh[ v ].comm_time = 0;
	}

	orig_topo_graph.PrintGraphViz( "Level_Topo_App_Map.dot" );

	cout << "Hurray!!! Everything Done!!! " << endl;
	cout << fixed << "The max app latency is " << tot_time << "s" << endl;
	out << fixed <<"The max app latency is " << tot_time << "s" << endl;

	time = tot_time;

	out.close();
}

#endif
