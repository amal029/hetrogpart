// Heterogeneous Architecture GRaph Partitioner --- 1.0.0.1, Aug 2012
// 16.08.2012

/**
 * @file TopoGen.cpp
 *
 */

#ifndef TOPO_GEN_CPP
#define TOPO_GEN_CPP

#include <openssl/md5.h>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

#include "TopoGen.hpp"

/*
 *  @brief Contains actual code of respective functions
 *         present in the header.
 *
 *  No detailed comments will be present, refer to the header
 *  for any documentation regarding the class and its inherent
 *  functions and attributes.
 */

TopoGen :: TopoGen()
{
	dim_1 = 0;
	dim_2 = 0;
	no_of_nodes = 0;
	total_compute_power = 0;
	level = 0;
}

//ConvertDimension( uint32_t dim_1_p, uint32_t dim_2_p )
int32_t TopoGen :: ConvertDimension( uint32_t dim_1_p, uint32_t dim_2_p )
{
	return ( ( dim_1_p * dim_2 ) + ( dim_2_p ) );
}
//end of ConvertDimension( uint32_t dim_1_p, uint32_t dim_2_p )

//Create( uint32_t dim_1_p, uint32_t dim_2_p )
void TopoGen :: Create( uint32_t dim_1_p, uint32_t dim_2_p )
{
	dim_1 = dim_1_p;
	dim_2 = dim_2_p;

	no_of_nodes = ( dim_1 * dim_2 );

	TwoAryTwoMesh();
	ComputeTotalPower();
}
//end of Create( uint32_t dim_1_p, uint32_t dim_2_p )


//TwoAryTwoMesh()
void TopoGen :: TwoAryTwoMesh()
{
    cout << "coming into generator" << endl;

	vector < E > edge_vector;

	uint32_t dim_1_start = 0;
	uint32_t dim_1_limit = dim_1 - 1;

	uint32_t dim_2_start = 0;
	uint32_t dim_2_limit = dim_2 - 1;

	uint32_t i, j;
	for( i = 0; i < dim_1; i++ )
	{
		for( j = 0; j < dim_2; j++ )
		{
			if( i == dim_1_start || i == dim_1_limit || j == dim_2_start || j == dim_2_limit )
			{
				if( i == dim_1_start && j == dim_2_start )
				{
					edge_vector.push_back( E( ConvertDimension( i, j ), ConvertDimension( i, ( j + 1 ) ) ) );
					edge_vector.push_back( E( ConvertDimension( i, j ), ConvertDimension( ( i + 1 ), j ) ) );
				}
				else if( i == dim_1_start && j == dim_2_limit )
				{
					edge_vector.push_back( E( ConvertDimension( i, j ), ConvertDimension( i, ( j - 1 ) ) ) );
					edge_vector.push_back( E( ConvertDimension( i, j ), ConvertDimension( ( i + 1 ), j ) ) );
				}
				else if( i == dim_1_limit && j == dim_2_start )
				{
					edge_vector.push_back( E( ConvertDimension( i, j ), ConvertDimension( ( i - 1 ), j ) ) );
					edge_vector.push_back( E( ConvertDimension( i, j ), ConvertDimension( i, ( j + 1 ) ) ) );
				}
				else if( i == dim_1_limit && j == dim_2_limit )
				{
					edge_vector.push_back( E( ConvertDimension( i, j ), ConvertDimension( i, ( j - 1 ) ) ) );
					edge_vector.push_back( E( ConvertDimension( i, j ), ConvertDimension( ( i - 1 ), j ) ) );
				}
				else if( i == dim_1_start )
				{
					edge_vector.push_back( E( ConvertDimension( i, j ), ConvertDimension( i, ( j - 1 ) ) ) );
					edge_vector.push_back( E( ConvertDimension( i, j ), ConvertDimension( i, ( j + 1 ) ) ) );
					edge_vector.push_back( E( ConvertDimension( i, j ), ConvertDimension( ( i + 1 ), j ) ) );
				}
				else if( i == dim_1_limit )
				{
					edge_vector.push_back( E( ConvertDimension( i, j ), ConvertDimension( i, ( j - 1 ) ) ) );
					edge_vector.push_back( E( ConvertDimension( i, j ), ConvertDimension( i, ( j + 1 ) ) ) );
					edge_vector.push_back( E( ConvertDimension( i, j ), ConvertDimension( ( i - 1 ), j ) ) );
				}
				else if( j == dim_1_start )
				{
					edge_vector.push_back( E( ConvertDimension( i, j ), ConvertDimension( ( i - 1 ), j ) ) );
					edge_vector.push_back( E( ConvertDimension( i, j ), ConvertDimension( ( i + 1 ), j ) ) );
					edge_vector.push_back( E( ConvertDimension( i, j ), ConvertDimension( i, ( j + 1 ) ) ) );
				}
				else if( j == dim_1_limit )
				{
					edge_vector.push_back( E( ConvertDimension( i, j ), ConvertDimension( ( i - 1 ), j ) ) );
					edge_vector.push_back( E( ConvertDimension( i, j ), ConvertDimension( ( i + 1 ), j ) ) );
					edge_vector.push_back( E( ConvertDimension( i, j ), ConvertDimension( i, ( j - 1 ) ) ) );
				}
			}
			else
			{
				edge_vector.push_back( E( ConvertDimension( i, j ), ConvertDimension( ( i - 1 ), j ) ) );
				edge_vector.push_back( E( ConvertDimension( i, j ), ConvertDimension( ( i + 1 ), j ) ) );
				edge_vector.push_back( E( ConvertDimension( i, j ), ConvertDimension( i, ( j - 1 ) ) ) );
				edge_vector.push_back( E( ConvertDimension( i, j ), ConvertDimension( i, ( j + 1 ) ) ) );
			}
		}
	}

    cout << "Created edges" << endl;

	for( uint32_t i = 0; i < ( dim_1 * dim_2 ); i++ )
	{
		for( uint32_t j = 0; j < edge_vector.size(); j++ )
		{
			if( edge_vector[ j ].first == ( int32_t ) i )
			{
				for( uint32_t k = 0; k < edge_vector.size(); k++ )
				{
					if( edge_vector[ k ].second == edge_vector[ j ].first && edge_vector[ k ].first == edge_vector[ j ].second )
					{
						edge_vector[ k ].first = -1;
					}
				}
			}
		}
	}

    cout << "Removing duplicate edges... done" << endl;

	vector < E > new_edge_vector;
	for( uint32_t j = 0; j < edge_vector.size(); j++ )
	{
		if( edge_vector[ j ].first != -1 )
		{
			new_edge_vector.push_back( edge_vector[ j ] );
		}
	}

    cout << "Finalizing data structs for topo graph creation" << endl;

	mesh = TopologyGraph( &new_edge_vector[ 0 ], &new_edge_vector[ new_edge_vector.size() ], no_of_nodes );

    cout << "created topo graph" << endl;

	uint32_t index = 0;
	BGL_FORALL_VERTICES( v, mesh, TopologyGraph )
	{
		mesh[ v ].id = index;
		mesh[ v ].label = "CPU" + lexical_cast< string >( index );
		mesh[ v ].shape = "square";

		if( rand() % 2 == 0 )
			mesh[ v ].type = G;
		else
			mesh[ v ].type = C;

		mesh[ v ].ids.push_back( index );

		//for( uint32_t i = 0; i < CONST[ mesh[ v ].type ].size(); i++ ) //orig
		for( uint32_t i = 0; i < 1; i++ ) //Strip SIMD
		{
			mesh[ v ].constraint.push_back( ( CONST[ mesh[ v ].type ][ i ] / CMP_RANGE ) + ( rand() % ( ( uint32_t ) CONST[ mesh[ v ].type ][ i ] ) ) );

			/*
            if (mesh[ v ].type == C) {
				if (i == 0)
					mesh[ v ].constraint.push_back(10 + (rand() % 100));
				else
					mesh[ v ].constraint.push_back(100000 + (rand() % 1000001));
			}
			else {
				if (i == 1)
					mesh[ v ].constraint.push_back(100 + (rand() % 1001));
				else
					mesh[ v ].constraint.push_back(100000 + (rand() % 1000001));
			}
            */

			//mesh[ v ].label += "   " + lexical_cast< string >( CONST_NAME[ i ] ) + " " +
				//				lexical_cast< string >( CONST[ mesh[ v ].type ][ i ] );
			mesh[ v ].label += "  " + lexical_cast< string >( CONST_NAME[ i ] ) + " " +
								lexical_cast< string >( mesh[ v ].constraint[ i ] );
		}
        //mesh[ v ].constraint[ NUM ] = 1; //orig

		uint32_t temp_const_mips = CONST[ mesh[ v ].type ][ MIPS ];
		uint32_t temp_const_vec = CONST[ mesh[ v ].type ][ VEC ];

		if( temp_const_mips == 0 ) temp_const_mips = 1;
		if( temp_const_vec == 0 ) temp_const_vec = 1;

		mesh[ v ].compute_power = temp_const_mips * temp_const_vec;
		mesh[ v ].comp_time = 0;
		mesh[ v ].comm_time = 0;
		mesh[ v ].compute_req = 0;

		index++;
	}

	BGL_FORALL_EDGES( e, mesh, TopologyGraph )
	{
		//mesh[ e ].bandwidth = BANDWIDTH[ NET ];
		mesh[ e ].bandwidth = ( BANDWIDTH[ NET ] / BW_RANGE ) + ( rand() % ( ( uint32_t ) BANDWIDTH[ NET ] ) );
		//Cartman was here.
		//mesh[ e ].bandwidth = 1 + ( rand() % 100 );
		mesh[ e ].label = "eth";
		mesh[ e ].label += " " + lexical_cast< string >( mesh[ e ].bandwidth );
	}

	//for( uint32_t i = 0; i < CONST[ 0 ].size(); i++ ) //orig
	for( uint32_t i = 0; i < 1; i++ ) //Strip SIMD
	{
		const_map.push_back( ( ConstIdType ) i );
	}

	//no_of_constraints = CONST[ 0 ].size(); //orig
	no_of_constraints = 1; //Strip SIMD
	no_of_nodes = num_vertices( mesh );
	no_of_edges = num_edges( mesh );

    cout << "Allocations for contraint wgts done" << endl;

    GenerateMetisFile( "met.grf" );

    cout << "Metis topology file generated" << endl;

	ComputeComm();

    cout << "Creation of comm table done" << endl;

	//PrintGraphViz( "out.dot" );
}
//end of TwoAryTwoMesh()

void TopoGen :: GenParams()
{
	BGL_FORALL_EDGES( e, mesh, TopologyGraph )
	{
		mesh[ e ].label += " " + lexical_cast< string >( mesh[ e ].bandwidth );
	}

	BGL_FORALL_VERTICES( v, mesh, TopologyGraph )
	{
		for( uint32_t i = 0; i < mesh[ v ].constraint.size(); i++ )
		{
			mesh[ v ].label += "   " + lexical_cast< string >( CONST_NAME[ i ] ) + " " +
								lexical_cast< string >( mesh[ v ].constraint[ i ] );
		}

		mesh[ v ].label += "   O_N: ";

		for( uint32_t i = 0; i < mesh[ v ].ids.size(); i++ )
		{
			mesh[ v ].label += lexical_cast< string >( mesh[ v ].ids[ i ] ) + ",";
		}

		uint32_t temp_const_mips = mesh[ v ].constraint[ MIPS ];
		uint32_t temp_const_vec = mesh[ v ].constraint[ VEC ];

		if( temp_const_mips == 0 ) temp_const_mips = 1;
		if( temp_const_vec == 0 ) temp_const_vec = 1;

		mesh[ v ].compute_power = temp_const_mips * temp_const_vec;
	}
}

void TopoGen :: PrintGraphViz( string output_file )
{
	// Output the graph in DOT format
	dynamic_properties dp;
	dp.property( "label", get( &TopoNode::label, mesh ) );
	dp.property( "id", get( &TopoNode::id, mesh ) );
	dp.property( "shape", get( &TopoNode::shape, mesh ) );

	dp.property( "bandwidth", get( &TopoEdge::bandwidth, mesh ) );
	dp.property( "label", get( &TopoEdge::label, mesh ) );

	ofstream out( output_file.c_str() );
	write_graphviz_dp( out, mesh, dp, string(), get( vertex_index, mesh ) );
	out.close();
}

void TopoGen :: ComputeTotalPower()
{
	total_compute_power = 0;
	BGL_FORALL_VERTICES( v, mesh, TopologyGraph )
	{
		total_compute_power += mesh[ v ].constraint[ MIPS ] * mesh[ v ].constraint[ VEC ];
	}
}

bool TopoGen :: PermuteConstraints( vector< ConstIdType > *const_map_p )
{
	vector< ConstIdType > *temp_const_map;

	if( const_map_p != NULL )
	{
		temp_const_map = const_map_p;
	}
	else
	{
		temp_const_map = new vector< ConstIdType >;
	}

	*temp_const_map = const_map;
	bool cond = next_permutation( temp_const_map->begin(), temp_const_map->end() );

	if( cond )
		RearrangeConstraints( *temp_const_map );

	return cond;
}

void TopoGen :: RearrangeConstraints( vector< ConstIdType > const_map_p )
{
	BGL_FORALL_VERTICES( v, mesh, TopologyGraph )
	{
		vector< uint32_t > t_constraint;
		for( uint32_t i = 0; i < const_map_p.size(); i++ )
		{
			/*
			 * finds the pos of the constraint by searching thru the
			 * const map with the a given const id.
			 *
			 * Creates a new vector that is filled with ids according
			 * to the new constraint map
			 */
			t_constraint.push_back( mesh[ v ].constraint[
				( find( const_map.begin(), const_map.end(), const_map_p[ i ] )
					- const_map.begin() ) ] );
		}
		mesh[ v ].constraint.clear();
		mesh[ v ].constraint = t_constraint;
	}
	const_map = const_map_p;
}

void TopoGen :: ResetConstraints()
{
	vector< ConstIdType > temp_const_map;
	temp_const_map = const_map;
	sort( temp_const_map.begin(), temp_const_map.end() );
	RearrangeConstraints( temp_const_map );
}

float_t TopoGen :: DeviationInComputePower()
{
	float_t abs_diff = 0;
	float_t desired_compute_power = 0;

	if( no_of_nodes > 0 )
	{
		desired_compute_power = total_compute_power / no_of_nodes;

		BGL_FORALL_VERTICES( v, mesh, TopologyGraph )
		{
			abs_diff += fabs( mesh[ v ].compute_power - desired_compute_power );
		}
	}

	return abs_diff;
}

int32_t TopoGen :: GenerateTopoSubGraph( vector< vector< uint32_t > > *partitions,
											TopoGen *topo_graph_obj )
{
	if( partitions->size() == 0 )
	{
		cout << "ERROR: Topology Partition size requested is not possible " << partitions->size() << endl;
		return -1;
	}

	topo_graph_obj->mesh = TopologyGraph( partitions->size() );

	uint32_t id = 0;
	BGL_FORALL_VERTICES( v, topo_graph_obj->mesh, TopologyGraph )
	{
		topo_graph_obj->mesh[ v ].id = id;
		topo_graph_obj->mesh[ v ].label = "M_N " + lexical_cast< string >( id ) + " D_F_IDs ";
		topo_graph_obj->mesh[ v ].shape = "square";
		topo_graph_obj->mesh[ v ].type = M;
		topo_graph_obj->mesh[ v ].constraint.resize( no_of_constraints, 0 );

		/*
		 * Iterate through the nodes in the original graph and append
		 * all the ids from that graph onto new graph's respective nodes
		 */
		for( uint32_t i = 0; i < partitions->at( id ).size(); i++ )
		{
			BGL_FORALL_VERTICES( v_orig, mesh, TopologyGraph )
			{
				if( mesh[ v_orig ].id == partitions->at( id ).at( i ) )
				{
					for( uint32_t j = 0; j < mesh[ v_orig ].ids.size(); j++ )
					{
						topo_graph_obj->mesh[ v ].ids.push_back( mesh[ v_orig ].ids[ j ] );
					}

					for( uint32_t j = 0; j < no_of_constraints; j++ )
					{
						topo_graph_obj->mesh[ v ].constraint[ j ] += mesh[ v_orig ].constraint[ j ];
					}

					topo_graph_obj->mesh[ v ].d_ids.push_back( mesh[ v_orig ].id );
					topo_graph_obj->mesh[ v ].label += lexical_cast< string >( mesh[ v_orig ].id ) + " ";
					break;
				}
			}
		}
		id++;
	}

	/*
	 * Find out the edges that belong to different partions and
	 * add them to a list
	 */
	vector< E > new_edges;
	BGL_FORALL_VERTICES( u_orig, mesh, TopologyGraph )
	{
		BGL_FORALL_ADJ( u_orig, v_orig, mesh, TopologyGraph )
		{
			uint32_t part_a = 0;

			for( uint32_t i = 0; i < partitions->size(); i++ )
			{
				for( uint32_t j = 0; j < partitions->at( i ).size(); j++ )
				{
					if( partitions->at( i ).at( j ) == mesh[ u_orig ].id )
					{
						part_a = i;
						break;
					}
				}
			}

			uint32_t part_b = 0;

			for( uint32_t i = 0; i < partitions->size(); i++ )
			{
				for( uint32_t j = 0; j < partitions->at( i ).size(); j++ )
				{
					if( partitions->at( i ).at( j ) == mesh[ v_orig ].id )
					{
						part_b = i;
						break;
					}
				}
			}

			if( part_a != part_b )
			{
				/*
				 * Make sure the edges that are being added are not the ones
				 * that are already present in the list including bidirectional
				 * edges
				 */
				bool edge_found = false;
				for( uint32_t e_index = 0; e_index < new_edges.size(); e_index++ )
				{
					if( new_edges[ e_index ] == E( mesh[ u_orig ].id, mesh[ v_orig ].id ) ||
						new_edges[ e_index ] ==	E( mesh[ v_orig ].id, mesh[ u_orig ].id ) )
					{
						edge_found = true;
					}
				}

				if( !edge_found )
				{
					new_edges.push_back( make_pair( mesh[ u_orig ].id, mesh[ v_orig ].id ) );

					/*
					 * Find the edge corresponding to the unique pair we
					 * found in the last loop
					 */
					BGL_FORALL_OUTEDGES( u_orig, o_orig, mesh, TopologyGraph )
					{
						if( mesh[ target( o_orig, mesh ) ].id == mesh[ v_orig ].id )
						{
							graph_traits< TopologyGraph >::vertex_descriptor v_s = 0, v_d = 0;

							/*
							 * Find the vertex ids that belong to the edge that has
							 * to be added to the new graph
							 */
							bool found_s = false, found_d = false;
							BGL_FORALL_VERTICES( v_sub, topo_graph_obj->mesh, TopologyGraph )
							{
								if( topo_graph_obj->mesh[ v_sub ].id == part_a )
								{
									found_s = true;
									v_s = v_sub;
								}

								if( topo_graph_obj->mesh[ v_sub ].id == part_b )
								{
									found_d = true;
									v_d = v_sub;
								}

								if( found_s && found_d )
									break;
							}

							/**
							 * Avinash: here is where i calculate comm cost
							 */
  							/*
							 * Search through the new graph and if the edge is already present
							 * just increase the weight of that edge
							 */
							bool found = false;
							BGL_FORALL_EDGES( e, topo_graph_obj->mesh, TopologyGraph )
							{
								if( ( topo_graph_obj->mesh[ source( e, topo_graph_obj->mesh ) ].id == topo_graph_obj->mesh[ v_s ].id &&
										topo_graph_obj->mesh[ target( e, topo_graph_obj->mesh ) ].id == topo_graph_obj->mesh[ v_d ].id ) ||
									( topo_graph_obj->mesh[ source( e, topo_graph_obj->mesh ) ].id == topo_graph_obj->mesh[ v_d ].id &&
										topo_graph_obj->mesh[ target( e, topo_graph_obj->mesh ) ].id == topo_graph_obj->mesh[ v_s ].id ) )
								{
									found = true;
									topo_graph_obj->mesh[ e ].bandwidth += mesh[ o_orig ].bandwidth;
									break;
								}
							}

							/*
							 * If the edge is not found create a new one and then add it to
							 * the new graph
							 */
							if( !found )
							{
								std::pair< graph_traits< TopologyGraph >::edge_descriptor, bool > edge_pair;
								edge_pair = add_edge( v_s, v_d, topo_graph_obj->mesh );
								//Older communication calculation
								topo_graph_obj->mesh[ edge_pair.first ].bandwidth = mesh[ o_orig ].bandwidth;
							}

							break;
						}
					}
				}
			}
		}
	}

	topo_graph_obj->no_of_nodes = num_vertices( topo_graph_obj->mesh );
	topo_graph_obj->no_of_edges = num_edges( topo_graph_obj->mesh );
	topo_graph_obj->no_of_constraints = no_of_constraints;
	topo_graph_obj->const_map = const_map;
	//topo_graph_obj->const_map.resize( no_of_constraints, ( ConstIdType ) 0 );

	topo_graph_obj->ResetConstraints();
	topo_graph_obj->ComputeTotalPower();
	topo_graph_obj->GenParams();

	return 1;
}

void TopoGen :: SetAccuBw( TopoGen *topo_graph_obj )
{
	BGL_FORALL_EDGES( e, topo_graph_obj->mesh, TopologyGraph )
	{
		vector< pair< uint32_t, float_t > > bw_vec;

		for( uint32_t i = 0; i < topo_graph_obj->mesh[ source( e, topo_graph_obj->mesh ) ].ids.size(); i++ )
		{
			for( uint32_t j = 0; j < topo_graph_obj->mesh[ target( e, topo_graph_obj->mesh ) ].ids.size(); j++ )
			{
				bw_vec.push_back( make_pair( topo_graph_obj->mesh[ target( e, topo_graph_obj->mesh ) ].ids[ j ],
					shortest_path->at( topo_graph_obj->mesh[ source( e, topo_graph_obj->mesh ) ].ids[ i ] ).at( topo_graph_obj->mesh[ target( e, topo_graph_obj->mesh ) ].ids[ j ] ) ) );

				if( DEBUG_SHORT_PATH )
					cout << bw_vec.back().first << ":" << bw_vec.back().second << " ";
			}
		}
		if( DEBUG_SHORT_PATH )
			cout << endl;

		sort( bw_vec.begin(), bw_vec.end(), bind( &pair< uint32_t, float_t >::first, _1 ) < bind( &pair< uint32_t, float_t >::first, _2 ) );
		mesh[ e ].bandwidth = 0;

		while( !bw_vec.empty() )
		{
			pair< uint32_t, float_t > min_bw = bw_vec.back();
			while( !bw_vec.empty() && bw_vec.back().first == min_bw.first )
			{
				min_bw = min( min_bw, bw_vec.back(), CompareSecond() );
				bw_vec.pop_back();
			}
			if( DEBUG_SHORT_PATH )
				cout << "m=" << min_bw.first << ":" << min_bw.second << " ";
			topo_graph_obj->mesh[ e ].bandwidth += min_bw.second;
		}

		if( DEBUG_SHORT_PATH )
			cout << topo_graph_obj->mesh[ source( e, topo_graph_obj->mesh ) ].id << "->" <<  topo_graph_obj->mesh[ target( e, topo_graph_obj->mesh ) ].id << ":" << topo_graph_obj->mesh[ e ].bandwidth << endl;

		mesh[ e ].label = " " + lexical_cast< string >( mesh[ e ].bandwidth );
	}
}

void TopoGen :: GenTpWgts( TopoGen *prev_level, vector< vector< vector< float_t >* >* > *tp_wgts_level,
				vector< vector< uint32_t >* > *parts_level, vector< uint32_t > *prev_level_order,
				vector< uint32_t > *curr_level_order )
{
	if( prev_level == NULL || this == NULL )
	{
		cout << "Prev graph invalid" << endl;
		return;
	}
	/*
	for( uint32_t p_id = 0; p_id < prev_level->no_of_nodes; p_id++ )
	{
		vector< vector< float_t >* > *tp_wgts_nodes = new vector< vector< float_t >* >;

		BGL_FORALL_VERTICES( v, mesh, TopologyGraph )
		{
			vector< float_t > *tp_wgts = new vector< float_t >;

			bool quit = false;
			BGL_FORALL_VERTICES( p_v, prev_level->mesh, TopologyGraph )
			{
				if( prev_level->mesh[ p_v ].id == p_id )
				{
					for( uint32_t i = 0; i < mesh[ v ].ids.size(); i++ )
					{
						for( uint32_t j = 0; j < prev_level->mesh[ p_v ].ids.size(); j++ )
						{
							if( mesh[ v ].ids[ i ] == prev_level->mesh[ p_v ].ids[ j ] )
							{
								//fill each node's contraints
								for( uint cst_id = 0; cst_id < mesh[ v ].constraint.size(); cst_id++ )
								{
									tp_wgts->push_back( lexical_cast< float_t >( mesh[ v ].constraint[ i ] ) );
								}
								break;
							}
						}
						if( quit )
						{
							break;
						}
					}
				}

				if( quit )
				{
					break;
				}
			}

			cout << "N" << mesh[ v ].id << " P";
			for( uint32_t t = 0; t <   mesh[ v ].p_ids.size(); t++ )
				cout <<  mesh[ v ].p_ids[ t ] << " ";
			cout << " C";
			for( uint32_t t = 0; t <   mesh[ v ].constraint.size(); t++ )
				cout <<  mesh[ v ].constraint[ t ] << " ";

			//fill the nodes
			tp_wgts_nodes->push_back( tp_wgts );
		}
		cout << endl;

		//fill the partitions
		tp_wgts_level->push_back( tp_wgts_nodes );
	}
	*/

	/*
	BGL_FORALL_VERTICES( v, mesh, TopologyGraph )
	{
		cout << "N" << mesh[ v ].id << "  D";
		for( uint32_t t = 0; t <   mesh[ v ].d_ids.size(); t++ )
			cout <<  mesh[ v ].d_ids[ t ] << " ";
		cout << " C";
		for( uint32_t t = 0; t < mesh[ v ].constraint.size(); t++ )
			cout <<  mesh[ v ].constraint[ t ] << " ";
	}
	cout << endl;
	cout << endl;
	*/
	/*
	cout << "prev" << endl;
	BGL_FORALL_VERTICES( v, prev_level->mesh, TopologyGraph )
	{
		cout << prev_level->mesh[ v ].id << " d:";
		for( uint32_t i = 0; i < prev_level->mesh[ v ].d_ids.size(); i++ )
		{
			cout << prev_level->mesh[ v ].d_ids[ i ] << " ";
		}
		cout << endl;
	}

	cout << "curr" << endl;
	BGL_FORALL_VERTICES( v, mesh, TopologyGraph )
	{
		cout << mesh[ v ].id << " ";// << " d:";
		for( uint32_t i = 0; i < mesh[ v ].d_ids.size(); i++ )
		{
			//cout << mesh[ v ].d_ids[ i ] << " ";
		}
		//cout << endl;
	}
	cout << endl;
	*/

	for( uint32_t pl = 0; pl < prev_level_order->size(); pl++ )
	{
		BGL_FORALL_VERTICES( p_v, prev_level->mesh, TopologyGraph )
		{
			if( prev_level_order->at( pl ) ==  prev_level->mesh[ p_v ].id )
			{
				vector< vector< float_t >* > *tp_wgts_nodes = new vector< vector< float_t >* >;
				vector< uint32_t > *parts_nodes = new vector< uint32_t >;

				for( uint32_t n_id = 0; n_id < prev_level->mesh[ p_v ].d_ids.size() ; n_id++ )
				{
					vector< float_t > *tp_wgts = new vector< float_t >;

					BGL_FORALL_VERTICES( v, mesh, TopologyGraph )
					{
						if( mesh[ v ].id == prev_level->mesh[ p_v ].d_ids[ n_id ] )
						{
							//fill each node's contraints
							for( uint cst_id = 0; cst_id < mesh[ v ].constraint.size(); cst_id++ )
							{
								tp_wgts->push_back( lexical_cast< float_t >( mesh[ v ].constraint[ cst_id ] ) );
							}
							parts_nodes->push_back( mesh[ v ].id );
							//cout << mesh[ v ].id << " ";
							curr_level_order->push_back( mesh[ v ].id );

							break;
						}
					}
					tp_wgts_nodes->push_back( tp_wgts );
				}
				//cout << endl;

				//fill the partitions
				tp_wgts_level->push_back( tp_wgts_nodes );
				parts_level->push_back( parts_nodes );
				break;
			}
		}
	}

	/*
	for( uint32_t i = 0; i < tp_wgts_level->size(); i++ )
	{
		cout << "P" << i << "  ";
		for( uint32_t j = 0; j < tp_wgts_level->at( i )->size(); j++ )
		{
			cout << " N" << parts_level->at( i )->at( j ) << " ";

			for( uint32_t k = 0; k < tp_wgts_level->at( i )->at( j )->size(); k++ )
			{
				cout << tp_wgts_level->at( i )->at( j )->at( k ) << " ";
			}
			cout << ", ";
		}
		cout << endl;
	}
	cout << endl;
	*/

	for( uint32_t i = 0; i < tp_wgts_level->size(); i++ )
	{
        int sum_vec_size = 0;

		if( tp_wgts_level->at( i )->size() > 0 )
            sum_vec_size = tp_wgts_level->at( i )->at( 0 )->size();

        vector< float_t > sum( sum_vec_size, 0 );

		for( uint32_t j = 0; j < tp_wgts_level->at( i )->size(); j++ )
		{
			for( uint32_t k = 0; k < tp_wgts_level->at( i )->at( j )->size(); k++ )
			{
				sum[ k ] += tp_wgts_level->at( i )->at( j )->at( k );
			}
		}

		for( uint32_t j = 0; j < tp_wgts_level->at( i )->size(); j++ )
		{
            //cout << "j:" <<  tp_wgts_level->at( i )->size() << endl;
			for( uint32_t k = 0; k < tp_wgts_level->at( i )->at( j )->size(); k++ )
			{
                //cout << "k:" << tp_wgts_level->at( i )->at( j )->size() << endl;
                //cout << i << " " << j << " " << k << endl;
                //cout << "tp" << tp_wgts_level->at( i )->at( j )->at( k ) << endl;
                //cout << "sum" << sum[ k ] << endl;
                tp_wgts_level->at( i )->at( j )->at( k ) =  tp_wgts_level->at( i )->at( j )->at( k ) / sum[ k ];
			}
		}
	}

	for( uint32_t i = 0; i < tp_wgts_level->size(); i++ )
	{
		cout << "P" << i << " [";
		for( uint32_t j = 0; j < tp_wgts_level->at( i )->size(); j++ )
		{
			cout << " N" << parts_level->at( i )->at( j ) << " ";

			for( uint32_t k = 0; k < tp_wgts_level->at( i )->at( j )->size(); k++ )
			{
				cout << setprecision( 2 ) << tp_wgts_level->at( i )->at( j )->at( k );

				if( k < ( tp_wgts_level->at( i )->at( j )->size() - 1 ) )
					cout << ",";
			}

			if( j < ( tp_wgts_level->at( i )->size() - 1 ) )
				cout << " |";
		}
		cout << " ]";

		if( i < ( tp_wgts_level->size() - 1 ) )
			cout << " || ";
	}
	cout << endl;
}

void TopoGen :: GenerateMetisFile( string filename )
{
	ofstream output_file;
	output_file.open( filename.c_str() );

	output_file << no_of_nodes << " " << no_of_edges << " 011 " << no_of_constraints << endl;

	BGL_FORALL_VERTICES( v, mesh, TopologyGraph )
	{
		for( uint32_t i = 0; i < no_of_constraints; i++ )
			output_file << mesh[ v ].constraint[ i ] << " ";

		BGL_FORALL_OUTEDGES( v, e, mesh, TopologyGraph )
		{
			output_file << ( mesh[ target( e, mesh ) ].id + 1 ) << " " << ( uint32_t ) mesh[ e ].bandwidth << " ";
		}

		output_file << endl;
	}
    
    output_file.close();
}

//Floyd–Warshall algorithm
//Really slow for large sizes
void TopoGen :: ComputeComm()
{
	shortest_path = new vector< vector< float_t > >;
	shortest_path->resize( no_of_nodes );

	for( uint32_t i = 0; i < no_of_nodes; i++ )
	{
		shortest_path->at( i ).resize( no_of_nodes );
	}

    string md5_sum;

    unsigned long file_size;
    vector< char > buffer;

    ifstream metgrf;
    metgrf.open( "met.grf", ios::binary );

    if( metgrf.is_open() )
    {       
        metgrf.seekg( 0, ios::end );
        file_size = metgrf.tellg();
        cout << "file size: " << file_size << endl;
        metgrf.seekg( 0, ios::beg );
        buffer.resize( file_size );
        metgrf.read( &buffer[ 0 ], file_size );

        unsigned char result[ MD5_DIGEST_LENGTH ];        
        MD5( ( unsigned char* ) &buffer[ 0 ], file_size, result );

        for( int i = 0; i < MD5_DIGEST_LENGTH; i++ )
        {
            char buf[ ( MD5_DIGEST_LENGTH * 2 ) + 1 ];
            sprintf( buf, "%02x", result[ i ] );
            md5_sum.append( buf );
        }
    }

    cout << "Checking to see if comm values already exist" << endl;   
    ifstream comm_file( lexical_cast< string >( no_of_nodes ) + ".comm" );
    if( comm_file )
    {
        string chk_sum;        

        boost::archive::text_iarchive infs( comm_file );
        infs >> chk_sum;
        cout << chk_sum << endl;

        if( chk_sum == md5_sum )
        {
            cout << "Communication values already exists for specified arch, reloading them!!!" << endl;
            infs >> shortest_path;
            
            comm_file.close(); 
            return;
        }
        comm_file.close();
    }

	for( uint32_t i = 0; i < no_of_nodes; i++ )
	{
		for( uint32_t j = 0; j < no_of_nodes; j++ )
		{
			shortest_path->at( i ).at( j ) = INF;
		}
	}

	BGL_FORALL_EDGES( e, mesh, TopologyGraph )
	{
		shortest_path->at( mesh[ source( e, mesh ) ].id ).at( mesh[ target( e, mesh ) ].id ) = ( mesh[ e ].bandwidth == 0 ? INF : 1 / mesh[ e ].bandwidth );
		shortest_path->at( mesh[ target( e, mesh ) ].id ).at( mesh[ source( e, mesh ) ].id ) = ( mesh[ e ].bandwidth == 0 ? INF : 1 / mesh[ e ].bandwidth );
	}

    cout << "Not found/exists... Alloc done for " << no_of_nodes << " nodes" << endl;

	for( uint32_t k = 0; k < no_of_nodes; k++ )
	{
		for( uint32_t i = 0; i < no_of_nodes; i++ )
		{
			for( uint32_t j = 0; j < no_of_nodes; j++ )
			{
				shortest_path->at( i ).at( j ) = min( shortest_path->at( i ).at( j ), shortest_path->at( i ).at( k ) + shortest_path->at( k ).at( j ) );
			}
		}
	}

    cout << "Min path calc done" << endl;

	for( uint32_t i = 0; i < no_of_nodes; i++ )
	{
		for( uint32_t j = 0; j < no_of_nodes; j++ )
		{
			shortest_path->at( i ).at( j ) = ( 1 / shortest_path->at( i ).at( j ) );
		}
	}

	for( uint32_t i = 0; i < no_of_nodes; i++ )
	{
		for( uint32_t j = 0; j < no_of_nodes; j++ )
		{
			if( i == j )
				shortest_path->at( i ).at( j ) = 0;
		}
	}

    std::ofstream ofs2( lexical_cast< string >( no_of_nodes ) + ".comm" );
    boost::archive::text_oarchive oa2(ofs2);
    
    oa2 << md5_sum;
    oa2 << shortest_path;
    
    ofs2.close();

	if( DEBUG_SHORT_PATH )
	{
		cout << "shortest_path" << endl << "    ";
		for( uint32_t i = 0; i < no_of_nodes; i++ )
		{
			cout << setw( 6 ) << i << " ";
		}
		cout << endl;
		for( uint32_t i = 0; i < no_of_nodes; i++ )
		{
			cout << setw(2) << i << ": ";
			for( uint32_t j = 0; j < no_of_nodes; j++ )
			{
				cout << setw( 6 ) << setprecision( 4 ) << shortest_path->at( i ).at( j ) << " ";
			}
			cout << endl;
		}
	}
}

/* avinash: comm func
uint32_t TopoGen :: CalculateComm( TopologyGraph orig, TopologyGraph new, uint32_t s_id, uint32_t d_id )
{
}
*/

#endif
