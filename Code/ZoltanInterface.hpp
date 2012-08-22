// Heterogeneous Architecture GRaph Partitioner --- 1.0.0.1, Aug 2012
// 20.08.2012

/** @file ZoltanInterface.hpp
 *
 */

#ifndef ZOLTAN_INTERFACE_HPP
#define ZOLTAN_INTERFACE_HPP

//#define ZOLTAN_ID_PTR int*
//#define ZOLTAN_OK 0
//#define ZOLTAN_FATAL 1

#include <boost/mpi.hpp>
#include <mpi.h>
#include <zoltan_cpp.h>

#include "ApplicationGraph.hpp"

using namespace boost;
using namespace boost::mpi;
using namespace std;

/**
 *  @brief Represents
 *
 *  This class consists
 */

class ZoltanInterface
{
	// Variables
	public:

	private:
		/**
		 *  @brief
		 */
		Zoltan *zz;

		/**
		 *  @brief
		 */
		ApplicationGraph *app_graph_obj;

		/**
		 *  @brief
		 */
		uint32_t rank;

		/**
		 *  @brief
		 */
		uint32_t world_size;

		/**
		 *  @brief
		 */
		communicator world;

	// Functions
	public:
		/**
		 * A constructor.
		 */
		ZoltanInterface();

		/*
		 * A destructor.
		 */
		//~ZoltanInterface();

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		int Create( MPI_Comm mpi_comm, int argc, char* argv[] );

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		void DestroyZoltan();

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		void SetParmetis(  ApplicationGraph *app_graph_obj_ref );

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		int PartitionGraph( ApplicationGraph *app_graph_obj_ref );

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		static int GetNoOfAppNodes( void *data, int *ierr );

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		static void GetAppNodeList( void *data, int sizeGID, int sizeLID,
							ZOLTAN_ID_PTR globalID, ZOLTAN_ID_PTR localID,
							int wgt_dim, float *obj_wgts, int *ierr );

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		static void GetNumAppEdges( void *data, int sizeGID, int sizeLID,
							int num_obj,
							ZOLTAN_ID_PTR globalID, ZOLTAN_ID_PTR localID,
							int *num_edges, int *ierr );

		/**
		 *  @brief
		 *
		 *  desc
		 *
		 *  @param
		 */
		static void GetAppEdgeList( void *data, int sizeGID, int sizeLID, int num_obj,
							ZOLTAN_ID_PTR globalID, ZOLTAN_ID_PTR localID,
							int *num_edges, ZOLTAN_ID_PTR nborGID, int *nborProc,
							int wgt_dim, float *ewgts, int *ierr );



	private:
};


#endif
