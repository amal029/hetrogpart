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
}

//ConvertDimension( uint32_t dim_1_p, uint32_t dim_2_p )
int TopoGen :: ConvertDimension( uint32_t dim_1_p, uint32_t dim_2_p )
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
}
//end of Create( uint32_t dim_1_p, uint32_t dim_2_p )

//TwoAryTwoMesh()
int TopoGen :: TwoAryTwoMesh()
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
		mesh[ v ].type = "C";
		index++;
	}

	BGL_FORALL_EDGES( e, mesh, TopologyGraph )
	{
		mesh[ e ].bandwidth = 1;
		mesh[ e ].label = "eth";
	}

	PrintGraphViz( "out.dot" );

	return 0;
}
//end of TwoAryTwoMesh()

int TopoGen :: PrintGraphViz( const char* output_file )
{
	// Output the graph in DOT format
	dynamic_properties dp;
	dp.property( "label", get( &TopoNode::label, mesh ) );
	dp.property( "id", get( &TopoNode::id, mesh ) );
	dp.property( "shape", get( &TopoNode::shape, mesh ) );
	dp.property( "type", get( &TopoNode::type, mesh ) );

	dp.property( "bandwidth", get( &TopoEdge::bandwidth, mesh ) );
	dp.property( "label", get( &TopoEdge::label, mesh ) );

	ofstream out( output_file );
	write_graphviz_dp( out, mesh, dp, string(), get( vertex_index, mesh ) );
	out.close();

	return 0;
}

/*
int permuteConstraints()
{

}
*/

#endif
