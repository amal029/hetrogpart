// Heterogeneous Architecture GRaph Partitioner --- 1.0.0.1, Aug 2012
// 16.08.2012

/** @file MetisInterface.hpp
 *
 */

#ifndef METIS_INTERFACE_HPP
#define METIS_INTERFACE_HPP

#include "metis.h"
#include "TopoGen.hpp"
#include "ApplicationGraph.hpp"

typedef enum Partioner{ GRAPH_K_WAY, GRAPH_RECURSIVE } PartionerType;

/**
 *  @brief Represents
 *
 *  This class consists
 */

class MetisInterface
{
	// Variables
	public:

	private:
		/**
		 *  @brief
		 */
		PartionerType type;

	// Functions
	public:
		/**
		 * A constructor.
		 */
		MetisInterface( PartionerType type_p );

		/*
		 * A destructor.
		 */
		//~Template();

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		int32_t PartitionGraph( TopoGen *topo_graph, uint32_t nparts,
									vector< vector< uint32_t > > *partitions, vector< uint32_t > *parts );

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		int32_t PartitionGraph( ApplicationGraph *app_graph_obj, uint32_t nparts,
									vector< vector< float >* > *tp_wgts, vector< vector< uint32_t > > *partitions );

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		//int32_t MetisPart( TopoGen *topo_graph, uint32_t nparts,
		//					vector< idx_t > *xadj, vector< idx_t > *adjncy, vector< idx_t > *vwgt,
		//					vector< idx_t > *adjwgt, vector< real_t > *tpwgts, vector< idx_t > *partitions );

		int32_t MetisPart( TopoGen *topo_graph, uint32_t nparts,
										string options, vector< idx_t > *partitions );

		int32_t MetisPart( ApplicationGraph *app_graph, uint32_t nparts,
										vector< real_t > *tpwgts, string options, vector< idx_t > *partitions );

	private:
};


#endif
