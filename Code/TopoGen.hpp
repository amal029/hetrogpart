// Heterogeneous Architecture GRaph Partitioner --- 1.0.0.1, Aug 2012
// 16.08.2012

/**
 * @file TopoGen.hpp
 *
 */
#ifndef TOPO_GEN_HPP
#define TOPO_GEN_HPP

#include <boost/graph/graphviz.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/iteration_macros.hpp>

#include <cstdlib>
#include <utility>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

using namespace boost;
using namespace std;

#define FORMAT "010"
#define NUM_CONSTRAINTS 3
#define NUM_FAC 3

#define GPUSIZE 3
#define GPU_MIPS 10
#define GPU_VEC  1024
//int GPUS[GPUSIZE][2] = {{0,0},{0,3},{3,3}};

typedef struct
{
	uint32_t bandwidth;
	string label;
}TopoEdge;

typedef struct
{
	uint32_t id;

	string label;
	string shape;
	string type;

	vector< uint32_t > constraint;
}TopoNode;

// Graph type
typedef adjacency_list< vecS, vecS, undirectedS, TopoNode, TopoEdge > TopologyGraph;

// Edge pair type
// used initally for the construction of the graph edges
typedef std::pair< int32_t, int32_t > E;

/**
 *  @brief Represents
 *
 *  This class consists
 */
class TopoGen
{
	// Variables
	public:
		/**
		 *  @brief
		 */
		TopologyGraph mesh;


		/**
		 *  @brief
		 */
		uint32_t no_of_nodes;
	private:

		/**
		 *  @brief
		 */
		uint32_t dim_1;

		/**
		 *  @brief
		 */
		uint32_t dim_2;
	// Functions
	public:
		/**
		 * A constructor.
		 */
		TopoGen();

		/*
		 * A destructor.
		 */
		//~TopoGen();

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		void Create( uint32_t dim_1_p, uint32_t dim_2_p );

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		int ConvertDimension( uint32_t dim_1_p, uint32_t dim_2_p );

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		int TwoAryTwoMesh();

		/**
		*	@brief
		*
		*	desc
		*
		*	@param
		*/
		int PrintGraphViz( const char* output_file );
};


#endif
