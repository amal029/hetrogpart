#include<stdio.h>
#include<stdlib.h>


#define KARRY 4
#define KARRY2 4
#define DIM  2

#define FORMAT "010"
#define NUM_CONSTRAINTS 3
#define NUM_FAC 3

#define TRUE 1
#define FALSE 0

#define CONV_DN( i, j ) i * KARRY2 + j + 1
#define KARRY_START 0
#define KARRY_LIMIT KARRY - 1

#define KARRY2_START 0
#define KARRY2_LIMIT KARRY2 - 1

#define GPUSIZE 3
#define GPU_MIPS 10
#define GPU_VEC  1024
int GPUS[GPUSIZE][2] = {{0,0},{0,3},{3,3}};


#ifdef COMM
typedef struct {
  int conn;
  int weight;
}EW;
#endif

typedef struct{
  int mips;
  int vec;
  int number;
#ifndef COMM
  int EDGES[(DIM-1)*KARRY];
#else
  EW EDGES [(DIM-1)*KARRY];
#endif

}NODE;

NODE MESH[KARRY][KARRY2];

int k_arry_n_mesh()
{
  int i, j;
  for( i = 0; i < KARRY; i++ )
    {
      for( j = 0; j < KARRY2; j++ )
	{
	  int counter = 0;

	  if( i == KARRY_START || i == KARRY_LIMIT || j == KARRY_START || j == KARRY_LIMIT )
	    {
	      if( i == KARRY_START && j == KARRY2_START )
		{
		  MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( i, ( j + 1 ) );
		  MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( ( i + 1 ), j );
		}
	      else if( i == KARRY_START && j == KARRY2_LIMIT )
		{
		  MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( i, ( j - 1 ) );
		  MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( ( i + 1 ), j );
		}
	      else if( i == KARRY_LIMIT && j == KARRY2_START )
		{
		  MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( ( i - 1 ), j );
		  MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( i, ( j + 1 ) );
		}
	      else if( i == KARRY_LIMIT && j == KARRY2_LIMIT )
		{
		  MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( i, ( j - 1 ) );
		  MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( ( i - 1 ), j );
		}
	      else if( i == KARRY_START )
		{
		  MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( i, ( j - 1 ) );
		  MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( i, ( j + 1 ) );
		  MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( ( i + 1 ), j );
		}
	      else if( i == KARRY_LIMIT )
		{
		  MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( i, ( j - 1 ) );
		  MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( i, ( j + 1 ) );
		  MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( ( i - 1 ), j );
		}
	      else if( j == KARRY_START )
		{
		  MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( ( i - 1 ), j );
		  MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( ( i + 1 ), j );
		  MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( i, ( j + 1 ) );
		}
	      else if( j == KARRY_LIMIT )
		{
		  MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( ( i - 1 ), j );
		  MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( ( i + 1 ), j );
		  MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( i, ( j - 1 ) );
		}
	    }
	  else
	    {
	      MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( ( i - 1 ), j );
	      MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( ( i + 1 ), j );
	      MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( i, ( j - 1 ) );
	      MESH[ i ][ j ].EDGES[ counter++ ] = CONV_DN( i, ( j + 1 ) );
	    }
	}
    }
}

void
computeAss (void){
  int i,j;
  for(i=0;i<KARRY;++i){
    for(j=0;j<KARRY2;++j){
      MESH[i][j].mips = 100;
      MESH[i][j].vec = 0;
      MESH[i][j].number = (rand()%10)+1;
      int k = 0;
      for (k=0;k<GPUSIZE;++k){
	if (GPUS[k][0] == i && GPUS[k][1] == j){
	  MESH[i][j].mips = GPU_MIPS;
	  MESH[i][j].vec = GPU_VEC;
	  MESH[i][j].number = (rand()%50)+1;
	}
      }
    }
  }
}

/* Write the output file */
int
print(char *of, int counter, unsigned char doit){
  char file [256], file2[256];
  FILE *fd =NULL, *fd2 = NULL; 
  //FIRST LINE IN GRF FILE
  int i=0,j=0;
  for (i=0;i<KARRY;++i){
    for (j=0;j<KARRY2;++j){
      int k=0;
      switch (counter){
      case 0:
	//DEBUG
	if(doit == TRUE){
	  fprintf(stderr,"%d:%d\n",i,j);
	  snprintf(file,255,"%s.%s",of,"MIPS.VEC.NUM");
	  snprintf(file2,255,"%s.%s",of,"MIPS.NUM.VEC");
	  fd = fopen(file,"w");
	  fd2 = fopen(file2,"w");
	  fprintf(fd,"%d %d %s %d\n",(KARRY*KARRY2), ((KARRY*(KARRY2-1))+(KARRY2*(KARRY-1))), FORMAT, NUM_CONSTRAINTS);
	  fprintf(fd2,"%d %d %s %d\n",(KARRY*KARRY2), ((KARRY*(KARRY2-1))+(KARRY2*(KARRY-1))), FORMAT, NUM_CONSTRAINTS);
	}
	/* Build the constraint weights */
	/* MIPS VEC NUM */
	fprintf(fd,"%d %d %d", MESH[i][j].mips, MESH[i][j].vec, MESH[i][j].number);
	fprintf(fd2,"%d %d %d", MESH[i][j].mips, MESH[i][j].number, MESH[i][j].vec);
	doit = FALSE;
	break;
      case 1:
	if(doit == TRUE){
	  snprintf(file,255,"%s.%s",of,"VEC.MIPS.NUM");
	  snprintf(file2,255,"%s.%s",of,"VEC.NUM.MIPS");
	  fd = fopen(file,"w");
	  fd2 = fopen(file2,"w");
	  fprintf(fd,"%d %d %s %d\n",(KARRY*KARRY2), ((KARRY*(KARRY2-1))+(KARRY2*(KARRY-1))), FORMAT, NUM_CONSTRAINTS);
	  fprintf(fd2,"%d %d %s %d\n",(KARRY*KARRY2), ((KARRY*(KARRY2-1))+(KARRY2*(KARRY-1))), FORMAT, NUM_CONSTRAINTS);
	}
	fprintf(fd,"%d %d %d", MESH[i][j].vec, MESH[i][j].mips, MESH[i][j].number);
	fprintf(fd2,"%d %d %d", MESH[i][j].vec, MESH[i][j].number, MESH[i][j].mips);
	doit = FALSE;
	break;
      case 2:
	if(doit == TRUE){
	  snprintf(file,255,"%s.%s",of,"NUM.VEC.MIPS");
	  snprintf(file2,255,"%s.%s",of,"NUM.MIPS.VEC");
	  fd = fopen(file,"w");
	  fd2 = fopen(file2,"w");
	  fprintf(fd,"%d %d %s %d\n",(KARRY*KARRY2), ((KARRY*(KARRY2-1))+(KARRY2*(KARRY-1))), FORMAT, NUM_CONSTRAINTS);
	  fprintf(fd2,"%d %d %s %d\n",(KARRY*KARRY2), ((KARRY*(KARRY2-1))+(KARRY2*(KARRY-1))), FORMAT, NUM_CONSTRAINTS);
	}
	fprintf(fd,"%d %d %d", MESH[i][j].number, MESH[i][j].vec, MESH[i][j].mips);
	fprintf(fd2,"%d %d %d", MESH[i][j].number, MESH[i][j].mips, MESH[i][j].vec);
	doit = FALSE;
	break;
      default: fprintf(stderr,"FUCKED!!\n"); exit(1);
      }
      for(;k<(DIM-1)*KARRY2;++k){
#ifndef COMM
	if(MESH[i][j].EDGES[k] != 0){
	  fprintf(fd," %d", MESH[i][j].EDGES[k]);
	  fprintf(fd2," %d", MESH[i][j].EDGES[k]);
	}
#else
	if(MESH[i][j].EDGES[k].conn != 0){
	  fprintf(fd," %d %d", MESH[i][j].EDGES[k].conn, MESH[i][j].EDGES[k].weight);
	  fprintf(fd2," %d %d", MESH[i][j].EDGES[k].conn, MESH[i][j].EDGES[k].weight);
	}
#endif
      }
      fprintf(fd,"\n"); fprintf(fd2,"\n");
    }
  }
  fclose(fd); fclose(fd2);
}

/* This is the main function */
int
main(int argc, char **argv){
  
  srand(time(0));

  if (argc <= 1){
    fprintf(stderr,"Please run.... topo-gen <file-name>\n\n\n");
    exit(0);
  }
  
  //Intialize
  computeAss();
  int i=0,j=0;
  //STUPID SERVESH CODE
  k_arry_n_mesh ();
  //PRINT GRF FILE
  char file[256];
  int counter=0;
  for (counter=0;counter<NUM_FAC;++counter)
    print(argv[1],counter,TRUE);

}
