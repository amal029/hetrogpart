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

const vector< vector< uint32_t > >CONST = { { 100, 1, 1 }, { 10, 1024, 1 } };
const vector< string >CONST_NAME = { "MIPS", "VEC", "NUM" };
typedef enum ConstId{ MIPS, VEC, NUM } ConstIdType;
typedef enum ProcId{ C, G, M } ProcIdType;

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
	ProcIdType type;

	vector< uint32_t > constraint;
	vector< uint32_t > ids;
	vector< uint32_t > d_ids;

	vector< uint32_t > app_ids;
	vector< uint32_t > compute_req;

	double_t comp_time;
	double_t comm_time;

	uint32_t compute_power;
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

	private:
		/**
		 *  @brief
		 */
		uint32_t dim_1;

		/**
		 *  @brief
		 */
		uint32_t dim_2;

		/**
		 *  @brief
		 */
		TopologyGraph mesh;

		/**
		 *  @brief
		 */
		uint32_t no_of_nodes;

		/**
		 *  @brief
		 */
		uint32_t no_of_constraints;

		/**
		 *  @brief
		 */
		uint32_t no_of_edges;

		/**
		 * @brief
		 */
		uint32_t total_compute_power;

		/**
		 * @brief
		 */
		vector< ConstIdType > const_map;

		/**
		 * @brief
		 */
		uint32_t level;

	// Functions
	public:
		//friend class ZoltanInterface;
		friend class TopologyDendogram;
		friend class MetisInterface;
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
		int32_t ConvertDimension( uint32_t dim_1_p, uint32_t dim_2_p );

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		void TwoAryTwoMesh();

		/**
		*	@brief
		*
		*	desc
		*
		*	@param
		*/
		void PrintGraphViz( string output_file );

		/**
		*	@brief
		*
		*	desc
		*
		*	@param
		*/
		void ComputeTotalPower();

		/**
		*	@brief
		*
		*	desc
		*
		*	@param
		*/
		bool PermuteConstraints( vector< ConstIdType > *const_map_p );

		/**
		*	@brief
		*
		*	desc
		*
		*	@param
		*/
		void ResetConstraints();

		/**
		*	@brief
		*
		*	desc
		*
		*	@param
		*/
		void RearrangeConstraints( vector< ConstIdType > const_map_p );

		/**
		*	@brief
		*
		*	desc
		*
		*	@param
		*/
		float_t DeviationInComputePower();

		/**
		*	@brief
		*
		*	desc
		*
		*	@param
		*/
		int32_t GenerateTopoSubGraph( vector< vector< uint32_t > > *partitions,
											TopoGen *topo_graph_obj );

		/**
		*	@brief
		*
		*	desc
		*
		*	@param
		*/
		void GenParams();

		/**
		*	@brief
		*
		*	desc
		*
		*	@param
		*/
		void GenTpWgts( TopoGen *prev_level, vector< vector< vector< float_t >* >* > *tp_wgts_level,
				vector< vector< uint32_t >* > *parts_level );

		/**
		*	@brief
		*
		*	desc
		*
		*	@param
		*/
		void GenerateMetisFile( string filename );
};


#endif
