// Heterogeneous Architecture GRaph Partitioner --- 1.0.0.1, Aug 2012
// 16.08.2012

/**
 * @file Topogen.hpp
 *
 */

#ifndef TOPOGEN_HPP
#define TOPOGEN_HPP

#define DIM_1 4
#define DIM_2 4
#define DIM  2

#define FORMAT "010"
#define NUM_CONSTRAINTS 3
#define NUM_FAC 3

#define TRUE 1
#define FALSE 0

#define CONV_DN( i, j ) ( ( i * DIM_2 ) + ( j ) + 1 )

#define DIM_1_START 0
#define DIM_1_LIMIT DIM_1 - 1

#define DIM_2_START 0
#define DIM_2_LIMIT DIM_2 - 1

#define GPUSIZE 3
#define GPU_MIPS 10
#define GPU_VEC  1024
int GPUS[GPUSIZE][2] = {{0,0},{0,3},{3,3}};


/**
 *  @brief Represents
 *
 *  This class consists
 */

class Topogen
{
	// Variables
	public:

	private:
		/**
		 *  @brief
		 */
		vector < Vertex > map;

	// Functions
	public:
		/**
		 * A constructor.
		 */
		Topogen();

		/*
		 * A destructor.
		 */
		//~Topogen();

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		void create( );
};


#endif
