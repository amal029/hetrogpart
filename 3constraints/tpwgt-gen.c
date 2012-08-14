/*
 * topo-gen.cpp
 *
 * <servesh@franny>
 *
 * A topology graph generator in metis graph format
 *
 */


#include <stdio.h>

#define M 2
#define N 3
#define NEW_X_MIN 0.1
#define NEW_X_MAX 0.9

float C1 [M][N] = {{610,2054,21080},{710,1031,10940}};

// float C1[M][N] = {{410,1028,10640},{210,1026,10440}, {700,7,700},{10,1024,10240}};

// float C1[M][N] = {/*{10,1024,10240},{400,4,400}};{400,4,400},{200,2,200}};*/{10,1024,10240},{300,3,300}};/*,{10,1024,10240}};*/

/* float C1[M][N] = {{100, 1, 100}, {100, 1, 100}, {100, 1, 100}}; */

int main(int argc, char **argv)
{
  float min[N];
  float max[N];
  float Y[M][N];
  for( int i = 0; i < N; ++i){
    min[i] = 0.0;
    max[i] = 0.0;
    for (int j =0; j< M; ++j){
      if (min[i] >= C1[j][i]) min[i] = C1[j][i];
      if (max[i] <= C1[j][i]) max[i] = C1[j][i];
    }
    //TUTU
    for (int j=0;j<M;++j){
      float diff = max[i]-min[i];
      if (diff != 0)
	Y[j][i] = ((C1[j][i] - min[i])/diff)*(NEW_X_MAX-NEW_X_MIN)+NEW_X_MIN;
      else
	Y[j][i] = (NEW_X_MAX+NEW_X_MIN)/2;
    }
  }


  /* for (int j=0;j<N;++j){ */
  /*   float tutu = 0.0; */
  /*   for(int i=0;i<M;++i){ */
  /*     tutu += Y[i][j]; */
  /*   } */
  /*   for(int i=0;i<M;++i) */
  /*     Y[i][j] = Y[i][j]*(1/tutu); */
  /* } */

  FILE *fd = NULL;
  fd = fopen(argv[1],"w");
  for (int i=0;i<M;++i)
    for (int j=0;j<N;++j)
      fprintf(fd,"%d:%d = %f\n",i,j,Y[i][j]);
  fclose(fd);
}
