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
#include <boost/bind.hpp>

#include <cstdlib>
#include <utility>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "ApplicationGraph.hpp"

#ifdef SHORT_PATH
	#define DEBUG_SHORT_PATH 1
#else
	#define DEBUG_SHORT_PATH 0
#endif

using namespace boost;
using namespace std;

//MIPS is scaled down by a 1000 so it is in KIPS
const vector< vector< uint32_t > >CONST = { { 100000, 1000, 1 }, { 1000, 100000, 1 } };
const vector< string >CONST_NAME = { "MIPS", "VEC", "NUM" };
typedef enum ConstId{ MIPS, VEC, NUM } ConstIdType;
typedef enum ProcId{ C, G, M } ProcIdType;

#define CMP_SCALE_FACTOR 1
#define BW_SCALE_FACTOR 1

#define CMP_RANGE 100
#define BW_RANGE 1000

//Bandwidth in KB/s
const vector< float_t >BANDWIDTH = { 100000, 16000, 100000 };
const vector< string >BANDWIDTH_NAME = { "CORE->CORE", "CORE->GPU", "CPU->CPU" };
typedef enum BwId{ CORE, PCIE, NET } BwIdType;


#define INF 99999999999999999

typedef struct
{
	float_t bandwidth;
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

	//vector< uint32_t > app_ids;
	uint32_t compute_req;

	double_t comp_time;
	double_t comm_time;

	uint32_t compute_power;

	//ApplicationGraph *mapped_graph;
	vector< graph_traits< ApplicationGraphType >::vertex_descriptor > *app_id_ref;
}TopoNode;

// Graph type
typedef adjacency_list< vecS, vecS, undirectedS, TopoNode, TopoEdge > TopologyGraph;

typedef graph_traits< TopologyGraph >::vertex_descriptor TopoVertex;

// Edge pair type
// used initally for the construction of the graph edges
typedef std::pair< int32_t, int32_t > E;

typedef pair< uint32_t, float_t > BwType;
struct CompareSecond
{
    bool operator()( const BwType& left, const BwType& right ) const
    {
        return left.second < right.second;
    }
};


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

		/**
		 * @brief
		 */
		vector< vector< float_t > > *shortest_path;

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
				vector< vector< uint32_t >* > *parts_level, vector< uint32_t > *prev_level_order,
				vector< uint32_t > *curr_level_order );

		/**
		*	@brief
		*
		*	desc
		*
		*	@param
		*/
		void GenerateMetisFile( string filename );

		/**
		*	@brief
		*
		*	desc
		*
		*	@param
		*/
		void ComputeComm();

		/**
		*	@brief
		*
		*	desc
		*
		*	@param
		*/
		void SetAccuBw( TopoGen *topo_graph_obj );
};


#endif
