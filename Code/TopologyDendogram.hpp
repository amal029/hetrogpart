// Heterogeneous Architecture GRaph Partitioner --- 1.0.0.1, Aug 2012
// 16.08.2012

/** @file TopologyDendogram.hpp
 *
 */

#ifndef TOPOLOGY_DENDOGRAM_HPP
#define TOPOLOGY_DENDOGRAM_HPP

#include "TopoGen.hpp"
//#include "ZoltanInterface.hpp"
#include "MetisInterface.hpp"
#include "ApplicationGraph.hpp"

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
		/**
		 *  @brief
		 */
		float_t time;

	private:
		/**
		 *  @brief
		 */
		uint32_t dendogram_height;

		/**
		 *  @brief
		 */
		TopoGen orig_topo_graph;

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
		vector< TopoGen* > topo_graph_vector;

		/**
		 *  @brief
		 */
		vector< vector< vector< vector< float_t >* >* >* > *tp_wgts_p;

		/**
		 *  @brief
		 */
		vector< vector< vector< uint32_t >* >* > *parts_p;

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
									vector< vector< vector< vector< float_t >* >* >* > *tp_wgts_vector,
									vector< vector< vector< uint32_t >* >* > *parts_vector,
									//MPI_Comm mpi_comm,
									int argc, char **argv );

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		int32_t BuildDendogram( vector< uint32_t > parts_req,
								vector< vector< vector< vector< float_t >* >* >* > *tp_wgts_vector,
								vector< vector< vector< uint32_t >* >* > *parts_vector,
								//MPI_Comm mpi_comm,
								int argc, char **argv );

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		void GenTpWgts( vector< vector< vector< vector< float_t >* >* >* > *tp_wgts_vector,
									vector< vector< vector< uint32_t >* >* > *parts_vector );

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		void GenMapping( vector< vector< ApplicationGraph* > > *app_dendogram_obj );

	private:
};


#endif
