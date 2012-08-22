// Heterogeneous Architecture GRaph Partitioner --- 1.0.0.1, Aug 2012
// 16.08.2012

/**
 * @file Topogen.hpp
 *
 */

#ifndef TOPOGEN_CPP
#define TOPOGEN_CPP

#include "Topogen.hpp"

/*
 *  @brief Contains actual code of respective functions
 *         present in the header.
 *
 *  No detailed comments will be present, refer to the header
 *  for any documentation regarding the class and its inherent
 *  functions and attributes.
 */

//TwoAryTwoMesh()
int Topogen :: TwoAryTwoMesh()
{
	int i, j;
	for( i = 0; i < DIM_1; i++ )
	{
		for( j = 0; j < DIM_2; j++ )
		{
			int counter = 0;

			if( i == DIM_1_START || i == DIM_1_LIMIT || j == DIM_2_START || j == DIM_2_LIMIT )
			{
				if( i == DIM_1_START && j == DIM_2_START )
				{
					MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( i, ( j + 1 ) );
					MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( ( i + 1 ), j );
				}
				else if( i == DIM_1_START && j == DIM_2_LIMIT )
				{
					MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( i, ( j - 1 ) );
					MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( ( i + 1 ), j );
				}
				else if( i == DIM_1_LIMIT && j == DIM_2_START )
				{
					MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( ( i - 1 ), j );
					MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( i, ( j + 1 ) );
				}
				else if( i == DIM_1_LIMIT && j == DIM_2_LIMIT )
				{
					MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( i, ( j - 1 ) );
					MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( ( i - 1 ), j );
				}
				else if( i == DIM_1_START )
				{
					MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( i, ( j - 1 ) );
					MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( i, ( j + 1 ) );
					MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( ( i + 1 ), j );
				}
				else if( i == DIM_1_LIMIT )
				{
					MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( i, ( j - 1 ) );
					MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( i, ( j + 1 ) );
					MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( ( i - 1 ), j );
				}
				else if( j == DIM_1_START )
				{
					MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( ( i - 1 ), j );
					MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( ( i + 1 ), j );
					MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( i, ( j + 1 ) );
				}
				else if( j == DIM_1_LIMIT )
				{
					MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( ( i - 1 ), j );
					MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( ( i + 1 ), j );
					MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( i, ( j - 1 ) );
				}
			}
			else
			{
				MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( ( i - 1 ), j );
				MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( ( i + 1 ), j );
				MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( i, ( j - 1 ) );
				MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( i, ( j + 1 ) );
			}
		}
	}
}
//end of TwoAryTwoMesh()

#endif
