// Heterogeneous Architecture GRaph Partitioner --- 1.0.0.1, Aug 2012
// 16.08.2012

/** @file ApplicationDendogram.hpp
 *
 */

#ifndef APPLICATION_DENDOGRAM_HPP
#define APPLICATION_DENDOGRAM_HPP

#include <string>

#include "ApplicationGraph.hpp"
//#include "ZoltanInterface.hpp"
#include "MetisInterface.hpp"
#include "TopologyDendogram.hpp"

using namespace std;

/**
 *  @brief Represents
 *
 *  This class consists
 */

class ApplicationDendogram
{
	// Variables
	public:

	private:
		/**
		 *  @brief
		 */
		vector< vector< ApplicationGraph* > > app_dendogram_obj;


	// Functions
	public:
		/**
		 * A constructor.
		 */
		ApplicationDendogram();

		/*
		 * A destructor.
		 */
		//~ApplicationDendogram();

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		void ConstructDendogram( vector< vector< vector< uint32_t >* >* > *parts_vector,
													vector< vector< vector< vector< float_t >* >* >* > *tp_wgts_vector,
													//MPI_Comm mpi_comm,
													TopologyDendogram *t,
													int argc, char **argv );

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		bool SetApplication( string filename );

	private:
};


#endif
