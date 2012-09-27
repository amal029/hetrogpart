// Heterogeneous Architecture GRaph Partitioner --- 1.0.0.1, Aug 2012
// 20.08.2012

/** @file ApplicationGraph.hpp
 *
 */

#ifndef APPLICATION_GRAPH_HPP
#define APPLICATION_GRAPH_HPP

#include <boost/graph/graphviz.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/iteration_macros.hpp>

#include <string>
#include <sstream>
#include <fstream>
#include <vector>

#include "metis.h"

using namespace boost;
using namespace std;

typedef struct
{
	uint64_t weight;
	string label;
}AppEdge;

typedef struct
{
	uint32_t id;
	uint32_t d_id;

	string name;
	string label;
	string shape;
	string type;
	string constraints;

	vector< float_t > constraint;
}AppNode;

// Graph type
typedef adjacency_list< vecS, vecS, undirectedS, AppNode, AppEdge > ApplicationGraphType;

// Edge pair type
typedef pair< int32_t, int32_t > E;

/**
 *  @brief Represents
 *
 *  This class consists
 */
class ApplicationGraph
{
	// Variables
	public:

	//private:
		/**
		 *  @brief
		 */
		vector < uint32_t > constraint_mask;

		/**
		 *  @brief
		 */
		uint32_t no_of_constraints;

		/**
		 *  @brief
		 */
		uint32_t no_of_vertices;

		/**
		 *  @brief
		 */
		uint32_t no_of_edges;

		/**
		 *  @brief
		 */
		vector< uint32_t > const_wgt_tot;

		/**
		 *  @brief
		 */
		ApplicationGraphType app_graph;

	// Functions
	public:
		//friend class ZoltanInterface;
		friend class ApplicationDendogram;
		friend class MetisInterface;
		friend class TopologyDendogram;

		/**
		 * A constructor.
		 */
		ApplicationGraph();

		/*
		 * A destructor.
		 */
		//~ApplicationGraph();

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		int ReadGraph( string file_name );

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		int PrintGraphViz( string output_file );

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		void ParseConstraints();

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		void SetApplicationParameters();

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		int32_t ReadMetisFormat( string file_name );

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		bool ReadApplication( string filename );

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		int32_t GenerateAppSubGraphs( vector< vector< uint32_t > > *partitions,
											vector< ApplicationGraph* > *app_graph_vector );

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		int32_t SetTpWgts( vector< vector< float_t >* > *tp_wgts, vector< int32_t > *part_ids,
										vector< int32_t > *wgt_idx, vector< float_t > *part_sizes );


		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		int32_t SetTpWgts( vector< vector< float_t >* > *tp_wgts, vector< real_t > *tpwgts );

		void GenerateMetisFile( string filename );

		void GenerateMetisFileAddDummyNode( string filename );

	private:
};


#endif
