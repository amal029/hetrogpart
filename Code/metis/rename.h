/*
 * Copyright 1997, Regents of the University of Minnesota
 *
 * rename.h
 *
 * This file contains header files
 *
 * Started 10/2/97
 * George
 *
 * $Id: rename.h 10565 2011-07-13 16:07:36Z karypis $
 *
 */


#ifndef _LIBMETIS_RENAME_H_
#define _LIBMETIS_RENAME_H_


/* contig.c */
#define FindPartitionInducedComponents  libmetis__FindPartitionInducedComponents   
#define IsConnected                     libmetis__IsConnected

/* debug.c */
#define ComputeCut			libmetis__ComputeCut
#define ComputeVolume			libmetis__ComputeVolume

/* graph.c */
#define CreateGraph                     libmetis__CreateGraph
#define FreeGraph                       libmetis__FreeGraph

#define CreateMesh                      libmetis__CreateMesh
#define FreeMesh                        libmetis__FreeMesh

/* util.c */
#define iargmax_strd                    libmetis__iargmax_strd 

#endif


