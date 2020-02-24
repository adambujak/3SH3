#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <stdbool.h>

typedef struct
{
  int    thread_id;
  int ** array;
  int    size;
} thread_data_t;

typedef enum
{
  SORT_MODE_ROW  = 0,
  SORT_MODE_COLUMN
} sort_mode_e;

typedef enum
{
  SORT_DIRECTION_FORWARD = 0,
  SORT_DIRECTION_BACKWARD  
} sort_direction_e;


int               phaseCnt       = 0;                          /* Counts phases that have completed */
int               threadDoneCnt  = 0;                          /* Counts threads that have completed phase */
pthread_mutex_t   doneWriteMutex = PTHREAD_MUTEX_INITIALIZER;  /* Mutex for accessing threadDoneCnt */
pthread_cond_t    phaseDoneCond  = PTHREAD_COND_INITIALIZER;   /* Indicates when threads are done current phase */


int         getSizeOfArray ( FILE *fp );
int         fillArray      ( int ** array, FILE *fp,   int size );
void        printArray     ( int ** array, int size );

void        sortRow        ( int ** array, int row,    int size, sort_direction_e direction );
void        sortLine       ( int ** array, int line,   int size, sort_mode_e mode, sort_direction_e direction );
void        sortColumn     ( int ** array, int column, int size );
void      * sort           ( void * input_thread_data );

sort_mode_e getSortMode    ( int phase );


void signalPhaseDone ( int thread )
{
  pthread_mutex_lock(&doneWriteMutex);
  threadDoneCnt += 1;
  pthread_mutex_unlock(&doneWriteMutex);
}

void sortRow ( int ** array, int row, int size, sort_direction_e direction )
{
  int buff = 0;
  for (int j = 0; j < size; j++)
  {
    for (int k = j+1; k < size; k++)
    {
      if (direction == SORT_DIRECTION_FORWARD)
      {
	if (array[row][j] > array[row][k])
	{
	  buff = array[row][j];
	  array[row][j] = array[row][k];
	  array[row][k] = buff;
	}
      }
      else 
      {
	if (array[row][j] < array[row][k])
	{
	  buff = array[row][j];
	  array[row][j] = array[row][k];
	  array[row][k] = buff;
	}
      }
    }
  }
}

void sortColumn ( int ** array, int column, int size )
{
  int buff = 0;
  for (int i = 0; i < size; i++)
  {
    for (int k = i+1; k < size; k++)
    {
      if (array[i][column] > array[k][column])
      {
	buff = array[i][column];
	array[i][column] = array[k][column];
	array[k][column] = buff;
      }
    }
  }
}


void sortLine ( int ** array, int line, int size, sort_mode_e mode, sort_direction_e direction )
{
  if (mode == SORT_MODE_ROW)
  {
    sortRow (array, line, size, direction);
  }
  else
  {
    sortColumn (array, line, size);
  }
}

void * sort ( void * input_thread_data )
{
  thread_data_t * thread_data = (thread_data_t *)(input_thread_data);
  int ** arr  = thread_data->array;
  int    n    = thread_data->size; 
  sort_mode_e sort_mode = SORT_MODE_ROW;


  /* Set sort direction for rows */
  sort_direction_e sort_direction = SORT_DIRECTION_FORWARD;
  if ((thread_data->thread_id % 2) != 0)
  {
    sort_direction = SORT_DIRECTION_BACKWARD;
  }
  
  while(phaseCnt < (int) (log2 ((float) (n*n)))+2) 
  {
    /* Set sorting mode - column or row */
    sort_mode = getSortMode(phaseCnt);
    /* Sort line - column or row */
    sortLine(arr, thread_data->thread_id, n, sort_mode, sort_direction);
    /* Increment thread completion count */
    signalPhaseDone(thread_data->thread_id); 

    /* Take mutex */
    pthread_mutex_lock(&doneWriteMutex);
    if (threadDoneCnt < n)
    {
      pthread_cond_wait(&phaseDoneCond, &doneWriteMutex);
    }
    else
    {
      threadDoneCnt = 0; 
      printArray(arr, n);
      phaseCnt++;
      pthread_cond_broadcast(&phaseDoneCond);
    }
    /* Release mutex */
    pthread_mutex_unlock(&doneWriteMutex);
   }
  pthread_exit(NULL);
}

void printArray ( int ** array, int size )
{
  printf("\n");
  for (int i = 0; i < size; i++)
  {
    for (int j = 0; j < size; j++)
    {
      printf( "%d ", array[i][j] );
    }
    printf("\n");
  }
  printf("\n");
}

int fillArray ( int ** array, FILE *fp,  int size )
{
  for (int i = 0; i < size; i++)
  {
    array[i] = (int *) malloc(size*size*sizeof(int));
  }
  int buff; 
  for (int i = 0; i < size; i++)
  {
    for (int j = 0; j < size; j++)
    {
      fscanf(fp, "%d", &buff);
      array[i][j] = (int) (buff);
    }
  }
  return 1;
}

sort_mode_e getSortMode ( int phase )
{
  if (phase % 2 == 0)
    return SORT_MODE_ROW;
  return SORT_MODE_COLUMN;
}

int getSizeOfArray ( FILE *fp )
{
  int n;
  fscanf(fp, "%d", &n);
  return n;
}

int main ( void )
{
  FILE *fp = fopen("input.txt", "r");
  
  int n;
  n = getSizeOfArray(fp);
  int * arr[n];

  fillArray(arr, fp, n);
  printArray(arr, n);

  pthread_mutex_init(&doneWriteMutex, NULL);
  pthread_cond_init (&phaseDoneCond, NULL);
  
  pthread_t * threads = (pthread_t *) malloc (sizeof(pthread_t) * n);
  thread_data_t * thread_data = (thread_data_t *) malloc (sizeof(thread_data_t) * n);

  int ret;
  for (int i = 0; i < n; i++)
  {
    thread_data[i].array     = arr;
    thread_data[i].thread_id = i;
    thread_data[i].size      = n;
    ret = pthread_create(&threads[i], NULL, sort, (void *) &thread_data[i]);
    if (ret)
    {
      printf("Error creating thread\n");
      exit(-1);
    }
  }
  pthread_mutex_destroy(&doneWriteMutex);
  pthread_cond_destroy(&phaseDoneCond);
  pthread_exit(NULL); 
}
