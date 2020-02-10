#include "queue.h"

static int8_t push ( queue_t * queue, data_t data )
{
  queue_node_t * N = (queue_node_t *) malloc(sizeof(queue_node_t));
  N->data = data;
  N->nextNode = NULL;
  
  if ( queue->head == NULL )
  {
    queue->head = N;
    return 0;
  }

  queue_node_t * temp = queue->head;

  while ( temp->nextNode != NULL )
  {
    temp = temp->nextNode;
  }
  temp->nextNode = N;
  return 0;
}

static int8_t pop ( queue_t * queue, data_t * data )
{
  if ( queue->head == NULL )
  {
    return -1;
  }

  queue_node_t * temp = queue->head;
  *data = temp->data;
  queue->head = queue->head->nextNode;
  free(temp);
  temp = NULL;
  return 0;
}




void init_queue (queue_t * queue)
{
  queue = (queue_t *) malloc(sizeof(queue_t));
  queue->head = NULL;
  queue->pop = &pop;
  queue->push = &push;
}
