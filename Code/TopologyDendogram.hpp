// Heterogeneous Architecture GRaph Partitioner --- 1.0.0.1, Aug 2012
// 16.08.2012

/** @file TopologyDendogram.hpp
 *
 */

#ifndef TOPOLOGY_DENDOGRAM_HPP
#define TOPOLOGY_DENDOGRAM_HPP

#include "TopoGen.hpp"
#include "ZoltanInterface.hpp"

using namespace std;

/**
 *  @brief Represents
 *
 *  This class consists
 */

class TopologyDendogram
{
	// Variables
	public:

	private:
		/**
		 *  @brief
		 */
		uint32_t dendogram_height;

		/**
		 *  @brief
		 */
		TopoGen topo_graph;

		/**
		 *  @brief
		 */
		TopoGen *prev_level_graph;

		/**
		 *  @brief
		 */
		TopoGen *curr_level_graph;

		/**
		 *  @brief
		 */

		/**
		 *  @brief
		 */

		/**
		 *  @brief
		 */

	// Functions
	public:
		/**
		 * A constructor.
		 */
		TopologyDendogram();

		/*
		 * A destructor.
		 */
		//~TopologyDendogram();

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		void CalculateDendogramHeight( uint32_t number_of_nodes );

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		void CreateTopologyGraph( int dim1, int dim2,
									vector< vector< uint32_t >* > *tp_wgts,
									vector< vector< uint32_t >* > *parts,
									MPI_Comm mpi_comm,
									int argc, char **argv );

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		int32_t BuildDendogram( vector< uint32_t > parts_req,
								vector< vector< uint32_t >* > *tp_wgts,
								vector< vector< uint32_t >* > *level_parts,
								MPI_Comm mpi_comm,
								int argc, char **argv );


	private:
};


#endif
