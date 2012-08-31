// Heterogeneous Architecture GRaph Partitioner --- 1.0.0.1, Aug 2012
// 16.08.2012

/** @file ApplicationDendogram.hpp
 *
 */

#ifndef APPLICATION_DENDOGRAM_HPP
#define APPLICATION_DENDOGRAM_HPP

#include <string>

#include "ApplicationGraph.hpp"
#include "ZoltanInterface.hpp"

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
		void ConstructDendogram( vector< vector< uint32_t > > parts_req, MPI_Comm mpi_comm,
														int argc, char **argv );

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		void SetApplication( string filename );

	private:
};


#endif
