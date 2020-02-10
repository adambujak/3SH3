#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <unistd.h>
#include  <sys/types.h>
#include  "queue.h"

void printSum ( queue_t * queue )
{
  int total = 0;
  int8_t tempData;
  while ( queue->pop(queue, &tempData) != -1 )
  {
    total += tempData;
  }
  printf("Total: %d\n", total);
}

int main(void)
{

  int       total = 0;
  int       input_fd[2], sum_fd[2], nbytes;
  pid_t     childpid;
  int8_t    parent_input = 0;
  int8_t    user_input = 0;
  int       child_input = 0;
  pipe(sum_fd);
  pipe(input_fd);
  if(( childpid = fork()) ==  -1)
  {
    perror("fork");
    exit (0);
  }
  if (childpid  == 0) 
  {
    close(input_fd[0]);
    while ( user_input != -1 ) 
    {
      printf("Enter an integer: ");
      scanf("%d", &user_input);
      write(input_fd[1], &user_input , 1);
    }
    close(sum_fd[1]);
    
    nbytes = read(sum_fd[0],  &child_input, sizeof(child_input));
    printf("Total: %d\n", child_input);
  }
  else
  {
    close (input_fd[1]);
    while ( parent_input != -1 ) 
    {
      total += parent_input;
      nbytes = read(input_fd[0],  &parent_input, sizeof(parent_input));
      printf("Received int: %d\n", parent_input);
    }
    close (sum_fd[0]);
    write (sum_fd[1], &total, 1);
  }
  return  0;
}

