#include <stdlib.h>
#include <signal.h>

void signal_handler ( int sig )
{
  if (sig == SIGINT)
  {
    printf("CTRL+C pressed!\n");
  }
  else if (sig == SIGTSTP) 
  {
    printf("CTRL+ pressed!\n");
  }
  else if (sig == SIGALRM)
  {
    printf("Alarm\n");
  }
}
int main ( void ) 
{
    if (signal(SIGALRM, signal_handler) == SIG_ERR) 
    {
        printf("failed to register alarm handler.\n");
        exit(1);
    }
    if (signal(SIGTSTP, signal_handler) == SIG_ERR) 
    {
        printf("failed to register stop handler.\n");
        exit(1);
    }
    if (signal(SIGINT, signal_handler) == SIG_ERR) 
    {
        printf("failed to register interrupt handler.\n");
        exit(1);
    }
    while (1)
    {
        sleep(2);
        raise(SIGALRM);
    }
}

