#include <stdlib.h>

typedef struct node_struct  queue_node_t;
typedef struct queue_struct queue_t;
typedef int8_t data_t;
void init_queue (queue_t * queue);
struct node 
{
  data_t         data;
  queue_node_t * nextNode;
};

struct queue_struct 
{
  queue_node_t  * head;
  int8_t        ( * push ) (queue_t * queue, data_t   data);
  int8_t        ( * pop )  (queue_t * queue, data_t * data);
};
