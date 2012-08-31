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
												vector< vector< uint32_t >* > *tp_wgts,
												vector< vector< uint32_t >* > *parts,
												MPI_Comm mpi_comm,
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

	BuildDendogram( parts_req, tp_wgts, parts, mpi_comm, argc, argv );
}

//BuildDendogram()
int32_t TopologyDendogram :: BuildDendogram( vector< uint32_t > parts_req,
												vector< vector< uint32_t >* > *tp_wgts,
												vector< vector< uint32_t >* > *level_parts,
												MPI_Comm mpi_comm,
												int argc, char **argv )
{
	cout << "Requested parts : ";
	for( uint32_t i = 0; i < parts_req.size(); i++ )
	{
		cout << parts_req[ i ] << " ";
	}
	cout << endl;

	vector< TopoGen* > topo_graph_vector;

	prev_level_graph = &topo_graph;
	for( uint32_t level = 0; level < dendogram_height; level++ )
	{
		int32_t min_dev_compute_power = -1;
		vector< uint32_t > *min_level_parts;

		cout << "Computing topology for level " << level << endl;

		uint32_t permute = 0;
		uint32_t c_permute = 0;

		do
		{
			ZoltanInterface ztn;
			TopoGen *topo_graph = NULL;
			vector< uint32_t > *temp_level_parts = NULL;
			vector< vector< uint32_t > > parts( parts_req[ level ] );
			int32_t temp_power = -1;

			ztn.Create( mpi_comm, argc, argv );
			ztn.SetParmetis();

			temp_level_parts = new vector< uint32_t >;

			ztn.PartitionGraph( prev_level_graph, parts_req[ level ], &parts, temp_level_parts );
			topo_graph = new TopoGen();

			cout << "Req=" << parts_req[ level ] << " Got=" << temp_level_parts->size() << " of each " ;
			for( uint32_t i = 0; i < temp_level_parts->size(); i++ )
			{
				cout << temp_level_parts->at( i ) << " ";
			}
			cout << endl;


			if( prev_level_graph->GenerateTopoSubGraph( &parts, topo_graph ) )
			{
				temp_power = topo_graph->DeviationInComputePower();

				if( ( min_dev_compute_power == -1 && temp_power != -1 ) ||
					( temp_power != -1 && temp_power < min_dev_compute_power ) )
				{
					min_dev_compute_power = temp_power;
					curr_level_graph = topo_graph;
					min_level_parts = temp_level_parts;
					c_permute = permute;
				}
				else
				{
					delete topo_graph;
					delete temp_level_parts;
				}
			}

			cout << "Completed permutation " << permute << endl;
			permute++;

			ztn.DestroyZoltan();
		}
		while( prev_level_graph->PermuteConstraints( NULL ) );

		cout << "Chose permutation " << c_permute << endl;

		vector< uint32_t > *level_tp_wgts;
		level_tp_wgts = new vector< uint32_t >;
		//curr_level_graph->GenTpWgts( level_tp_wgts );

		tp_wgts->push_back( level_tp_wgts );
		level_parts->push_back( min_level_parts );
		topo_graph_vector.push_back( curr_level_graph );
		prev_level_graph = curr_level_graph;
	}

	for( uint32_t i = 0; i < topo_graph_vector.size(); i++ )
	{
		topo_graph_vector[ i ]->PrintGraphViz( "Level_" + lexical_cast< string >( i ) + "_TopoGraph.dot" );
		delete( topo_graph_vector[ i ] );
	}

	return 1;
}
//BuildDendogram()

#endif
