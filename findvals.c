#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#include "utils.h"

#define NUM_THREADS 2

int sh_count=0;

int verbose = 0;

int main(int argc, char **argv)
{
  float ref=0;
  float tol=0;

  int i;
  for(i = 1;i < argc;i++){
    if(strcmp(argv[i],"-r") == 0)
            ref = strtof(argv[i + 1],NULL);     
    else if(strcmp(argv[i],"-v") == 0) 
            verbose = 1;
    else if(strcmp(argv[i],"-t") == 0)            
	    tol = strtof(argv[i + 1],NULL);
  }

  if (ref == 0 || tol == 0)
  {
    fprintf(stderr, "Usage: %s -r ref -t tol\n", argv[0]);
    exit(1);
  }

  struct tm *local;
  time_t start;
  time(&start); // read and record clock
  local = localtime(&start);
  printf("# Start time and date: %s", asctime(local));

  int rct, cct; /* row count and column count to be read in */

  scanf("%d", &rct);
  scanf("%d", &cct);
    
  float **sh_rows = (float **)malloc(rct * sizeof (float*));//sizeof(long int)
  if (sh_rows == 0)
  {
	  fprintf(stderr, "arr Couldn't alocate sufficient space.\n");
	  exit(1);
  }
  
  int k;
  for(k = 0;k < rct;k++)
  {
	  float* row = (float *) malloc(cct * sizeof(float));
	  if (row == 0)
	  {
	  fprintf(stderr, "row Couldn't alocate sufficient row space.\n");
	  exit(1);
	  }
	  sh_rows[k] = row;          
  }

  int ro,co;
  for(ro = 0;ro < rct;ro++){
	  for(co = 0;co < cct;co++){
		  scanf("%f", &sh_rows[ro][co]);
	  }
  }

  struct timeval tim;
  gettimeofday(&tim, NULL);
  double t1=tim.tv_sec+(tim.tv_usec/1000000.0);

  int r,c;
  #pragma omp parallel for private(r) private(c)
  for (r = 0; r < rct; r++)
  {
    for (c = 0; c < cct; c++)
    {
	if(approxEqual(sh_rows[r][c],ref,tol))
	{
	       if(verbose)
               {
			fprintf(stdout, "r=%d, cc=%d: %.6f (thread= %d)\n",r, c, sh_rows[r][c], omp_get_thread_num());
	       }
               #pragma omp atomic
               sh_count++; 
	}
                  
     }
  }
  fprintf(stdout, "Found %d approximate matches.\n", sh_count);

  gettimeofday(&tim, NULL);
  double t2=tim.tv_sec+(tim.tv_usec/1000000.0);
  printf("Elapsed time: %.6lf(s)\n", t2-t1);

  exit(0);
}
