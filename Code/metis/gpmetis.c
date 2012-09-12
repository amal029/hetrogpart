/*
 * Copyright 1994-2011, Regents of the University of Minnesota
 *
 * gpmetis.c
 *
 * Drivers for the partitioning routines
 *
 * Started 8/28/94
 * George
 *
 * $Id: gpmetis.c 10567 2011-07-13 16:17:07Z karypis $
 *
 */

#include "metisbin.h"


idx_t* gpmetis( int argc, char **argv )
/*************************************************************************/
/*! Let the game begin! */
/*************************************************************************/
//int main(int argc, char *argv[])
{
  idx_t i;
  char *curptr, *newptr;
  idx_t options[METIS_NOPTIONS];
  graph_t *graph;
  idx_t *part;
  idx_t objval;
  params_t *params;
  int status=0;

  //printf( "argc: %d\n", argc );
    //printf( "gk_optind %d\n", gk_optind );

    gk_optind = 1;

    for( i = 0; i < argc; i++ )
    {
        //printf( "%s*\n", argv[ i ] );
    }


  params = parse_cmdline(argc, argv);

  gk_startcputimer(params->iotimer);
  graph = ReadGraph(params);

  ReadTPwgts(params, graph->ncon);
  gk_stopcputimer(params->iotimer);

  /* Check if the graph is contiguous */
  if (params->contig && !IsConnected(graph, 0)) {
    printf("***The input graph is not contiguous.\n"
           "***The specified -contig option will be ignored.\n");
    params->contig = 0;
  }

  /* Get ubvec if supplied */
  if (params->ubvecstr) {
    params->ubvec = rmalloc(graph->ncon, "main");
    curptr = params->ubvecstr;
    for (i=0; i<graph->ncon; i++) {
      params->ubvec[i] = strtoreal(curptr, &newptr);
      if (curptr == newptr)
        errexit("Error parsing entry #%"PRIDX" of ubvec [%s] (possibly missing).\n",
            i, params->ubvecstr);
      curptr = newptr;
    }
  }

  /* Setup iptype */
  if (params->iptype == -1) {
    if (params->ptype == METIS_PTYPE_RB) {
      if (graph->ncon == 1)
        params->iptype = METIS_IPTYPE_GROW;
      else
        params->iptype = METIS_IPTYPE_RANDOM;
    }
  }

  GPPrintInfo(params, graph);

  part = imalloc(graph->nvtxs, "main: part");

  METIS_SetDefaultOptions(options);
  options[METIS_OPTION_OBJTYPE] = params->objtype;
  options[METIS_OPTION_CTYPE]   = params->ctype;
  options[METIS_OPTION_IPTYPE]  = params->iptype;
  options[METIS_OPTION_RTYPE]   = params->rtype;
  options[METIS_OPTION_MINCONN] = params->minconn;
  options[METIS_OPTION_CONTIG]  = params->contig;
  options[METIS_OPTION_SEED]    = params->seed;
  options[METIS_OPTION_NITER]   = params->niter;
  options[METIS_OPTION_NCUTS]   = params->ncuts;
  options[METIS_OPTION_UFACTOR] = params->ufactor;
  options[METIS_OPTION_DBGLVL]  = params->dbglvl;

  gk_malloc_init();
  gk_startcputimer(params->parttimer);

  switch (params->ptype) {
    case METIS_PTYPE_RB:
      status = METIS_PartGraphRecursive(&graph->nvtxs, &graph->ncon, graph->xadj,
                   graph->adjncy, graph->vwgt, graph->vsize, graph->adjwgt,
                   &params->nparts, params->tpwgts, params->ubvec, options,
                   &objval, part);
      break;

    case METIS_PTYPE_KWAY:
      status = METIS_PartGraphKway(&graph->nvtxs, &graph->ncon, graph->xadj,
                   graph->adjncy, graph->vwgt, graph->vsize, graph->adjwgt,
                   &params->nparts, params->tpwgts, params->ubvec, options,
                   &objval, part);
      break;

  }

  gk_stopcputimer(params->parttimer);

  if (gk_GetCurMemoryUsed() != 0)
    printf("***It seems that Metis did not free all of its memory! Report this.\n");
  params->maxmemory = gk_GetMaxMemoryUsed();
  gk_malloc_cleanup(0);


  if (status != METIS_OK) {
    printf("\n***Metis returned with an error.\n");
  }
  else {
    if (!params->nooutput) {
      /* Write the solution */
      gk_startcputimer(params->iotimer);
      WritePartition(params->filename, part, graph->nvtxs, params->nparts);
      gk_stopcputimer(params->iotimer);
    }

    GPReportResults(params, graph, part, objval);
  }

  idx_t *r_part = ( idx_t* ) calloc( graph->nvtxs, sizeof( idx_t ) );

  for( i = 0; i < graph->nvtxs; i++ )
  {
       r_part[ i ] = part[ i ];
  }

  FreeGraph(&graph);
  gk_free((void **)&part, LTERM);
  gk_free((void **)&params->filename, &params->tpwgtsfile, &params->tpwgts,
      &params->ubvecstr, &params->ubvec, &params, LTERM);

  return r_part;
}


/*************************************************************************/
/*! This function prints run parameters */
/*************************************************************************/
void GPPrintInfo(params_t *params, graph_t *graph)
{
  idx_t i;

  if (params->ufactor == -1) {
    if (params->ptype == METIS_PTYPE_KWAY)
      params->ufactor = KMETIS_DEFAULT_UFACTOR;
    else if (graph->ncon == 1)
      params->ufactor = PMETIS_DEFAULT_UFACTOR;
    else
      params->ufactor = MCPMETIS_DEFAULT_UFACTOR;
  }

  FILE *fp;
  fp = fopen( "metis.log", "a" );

  fprintf( fp,"******************************************************************************\n");
  fprintf( fp,"%s", METISTITLE);
  fprintf( fp," (HEAD: %s, Built on: %s, %s)\n", SVNINFO, __DATE__, __TIME__);
  fprintf( fp," size of idx_t: %zubits, real_t: %zubits, idx_t *: %zubits\n",
      8*sizeof(idx_t), 8*sizeof(real_t), 8*sizeof(idx_t *));
  fprintf( fp,"\n");
  fprintf( fp,"Graph Information -----------------------------------------------------------\n");
  fprintf( fp," Name: %s, #Vertices: %"PRIDX", #Edges: %"PRIDX", #Parts: %"PRIDX"\n",
      params->filename, graph->nvtxs, graph->nedges/2, params->nparts);
  if (graph->ncon > 1)
    fprintf( fp," Balancing constraints: %"PRIDX"\n", graph->ncon);

  fprintf( fp,"\n");
  fprintf( fp,"Options ---------------------------------------------------------------------\n");
  fprintf( fp," ptype=%s, objtype=%s, ctype=%s, rtype=%s, iptype=%s\n",
      ptypenames[params->ptype], objtypenames[params->objtype], ctypenames[params->ctype],
      rtypenames[params->rtype], iptypenames[params->iptype]);

  fprintf( fp," dbglvl=%"PRIDX", ufactor=%.3f, minconn=%s, contig=%s, nooutput=%s\n",
      params->dbglvl,
      I2RUBFACTOR(params->ufactor),
      (params->minconn  ? "YES" : "NO"),
      (params->contig   ? "YES" : "NO"),
      (params->nooutput ? "YES" : "NO")
      );

  fprintf( fp," seed=%"PRIDX", niter=%"PRIDX", ncuts=%"PRIDX"\n",
      params->seed, params->niter, params->ncuts);

  if (params->ubvec) {
    fprintf( fp," ubvec=(");
    for (i=0; i<graph->ncon; i++)
      fprintf( fp,"%s%.2e", (i==0?"":" "), (double)params->ubvec[i]);
    fprintf( fp,")\n");
  }

  fprintf( fp,"\n");
  switch (params->ptype) {
    case METIS_PTYPE_RB:
      fprintf( fp,"Recursive Partitioning ------------------------------------------------------\n");
      break;
    case METIS_PTYPE_KWAY:
      fprintf( fp,"Direct k-way Partitioning ---------------------------------------------------\n");
      break;
  }

  fclose( fp );
}


/*************************************************************************/
/*! This function does any post-partitioning reporting */
/*************************************************************************/
void GPReportResults(params_t *params, graph_t *graph, idx_t *part, idx_t objval)
{
  gk_startcputimer(params->reporttimer);
  ComputePartitionInfo(params, graph, part);

  gk_stopcputimer(params->reporttimer);

  FILE *fp;
  fp = fopen( "metis.log", "a" );

  fprintf( fp,"\nTiming Information ----------------------------------------------------------\n");
  fprintf( fp,"  I/O:          \t\t %7.3"PRREAL" sec\n", gk_getcputimer(params->iotimer));
  fprintf( fp,"  Partitioning: \t\t %7.3"PRREAL" sec   (METIS time)\n", gk_getcputimer(params->parttimer));
  fprintf( fp,"  Reporting:    \t\t %7.3"PRREAL" sec\n", gk_getcputimer(params->reporttimer));
  fprintf( fp,"\nMemory Information ----------------------------------------------------------\n");
  fprintf( fp,"  Max memory used:\t\t %7.3"PRREAL" MB\n", (real_t)(params->maxmemory/(1024.0*1024.0)));
  fprintf( fp,"******************************************************************************\n");

  fclose( fp );

}
