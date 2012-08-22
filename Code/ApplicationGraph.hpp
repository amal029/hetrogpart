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

using namespace boost;
using namespace std;

typedef struct
{
	float_t weight;
	string label;
}AppEdge;

typedef struct
{
	uint32_t id;

	string name;
	string label;
	string shape;
	string type;
	string constraints;

	vector< float_t > constraint;
}AppNode;

// Graph type
typedef adjacency_list< vecS, vecS, undirectedS, AppNode, AppEdge > ApplicationGraphType;


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
		//int variable;

		/**
		 *  @brief
		 */
		ApplicationGraphType app_graph;

	// Functions
	public:
		friend class ZoltanInterface;

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
		int ReadGraph( char* file_name );

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		int PrintGraphViz( const char* output_file );

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		void ParseTokens();

	private:
};


#endif
