#include <stdio.h>
#include "buffer.h"

static buffer_item buffer[BUFFER_SIZE];
static int counter, in , out;

int insert_item(buffer_item item){
  if (counter == BUFFER_SIZE){
    return -1;
  }
  else{
    buffer[in] = item;
    in = (in + 1) % BUFFER_SIZE;
    counter++;
    return 0;
  }
}

int remove_item(buffer_item *item){
  if (counter == 0){
    return -1;
  }
  else{
    *item = buffer[out];
    buffer[out] = 0;
    out = (out + 1) % BUFFER_SIZE;
    counter--;
    return 0;
  }
}

void printBuffer(){
  int k = 0;
  while (k < BUFFER_SIZE){
    printf("%2d\n", buffer[k++]);
  }
}
