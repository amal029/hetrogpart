// Heterogeneous Architecture GRaph Partitioner --- 1.0.0.1, Aug 2012
// 16.08.2012

/**
 * @file TopoGen.cpp
 *
 */

#ifndef TOPO_GEN_CPP
#define TOPO_GEN_CPP

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

	vector < E > new_edge_vector;
	for( uint32_t j = 0; j < edge_vector.size(); j++ )
	{
		if( edge_vector[ j ].first != -1 )
		{
			new_edge_vector.push_back( edge_vector[ j ] );
		}
	}

	mesh = TopologyGraph( &new_edge_vector[ 0 ], &new_edge_vector[ new_edge_vector.size() ], no_of_nodes );

	uint32_t index = 0;
	BGL_FORALL_VERTICES( v, mesh, TopologyGraph )
	{
		mesh[ v ].id = index;
		mesh[ v ].label = "CPU" + lexical_cast< string >( index );
		mesh[ v ].shape = "square";
		mesh[ v ].type = C;
		mesh[ v ].ids.push_back( index );

		for( uint32_t i = 0; i < CONST[ mesh[ v ].type ].size(); i++ )
		{
			mesh[ v ].constraint.push_back( CONST[ mesh[ v ].type ][ i ] );
			mesh[ v ].label += "   " + lexical_cast< string >( CONST_NAME[ i ] ) + " " +
								lexical_cast< string >( CONST[ mesh[ v ].type ][ i ] );
		}

		uint32_t temp_const_mips = CONST[ mesh[ v ].type ][ MIPS ];
		uint32_t temp_const_vec = CONST[ mesh[ v ].type ][ VEC ];

		if( temp_const_mips == 0 ) temp_const_mips = 1;
		if( temp_const_vec == 0 ) temp_const_vec = 1;

		mesh[ v ].compute_power = temp_const_mips * temp_const_vec;

		index++;
	}

	BGL_FORALL_EDGES( e, mesh, TopologyGraph )
	{
		mesh[ e ].bandwidth = 1;
		mesh[ e ].label = "eth";
		mesh[ e ].label += " " + lexical_cast< string >( mesh[ e ].bandwidth );
	}

	for( uint32_t i = 0; i < CONST[ 0 ].size(); i++ )
	{
		const_map.push_back( ( ConstIdType ) i );
	}

	no_of_constraints = CONST[ 0 ].size();

	PrintGraphViz( "out.dot" );
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

int32_t TopoGen :: DeviationInComputePower()
{
	uint32_t abs_diff = 0;
	uint32_t desired_compute_power = 0;

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
		cout << "Error: Topology Partition size requested is not possible " << partitions->size() << endl;
		return -1;
	}

	topo_graph_obj->mesh = TopologyGraph( partitions->size() );

	uint32_t id = 0;
	BGL_FORALL_VERTICES( v, topo_graph_obj->mesh, TopologyGraph )
	{
		topo_graph_obj->mesh[ v ].id = id;
		topo_graph_obj->mesh[ v ].label = "M_N " + lexical_cast< string >( id );
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
				if( /*( find( new_edges.begin(), new_edges.end(),
						make_pair( mesh[ u_orig ].id, mesh[ v_orig ].id ) ) == new_edges.end() ) &&
					( find( new_edges.begin(), new_edges.end(),
						make_pair( mesh[ v_orig ].id, mesh[ u_orig ].id ) ) == new_edges.end() )*/ 1 )
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
	topo_graph_obj->const_map.resize( no_of_constraints, ( ConstIdType ) 0 );

	for( uint32_t j = 0; j < no_of_constraints; j++ )
	{
		topo_graph_obj->const_map[ j ] = const_map[ j ];
	}
	topo_graph_obj->ResetConstraints();
	topo_graph_obj->ComputeTotalPower();
	topo_graph_obj->GenParams();

	return 1;
}


#endif
