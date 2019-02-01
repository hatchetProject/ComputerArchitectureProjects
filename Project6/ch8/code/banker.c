#include <stdio.h>
#include <stdlib.h>
#include "banker.h"


int request_resources(int customer_num, int request[])
{
  int i;
  for (i = 0; i < NUMBER_OF_RESOURCES; ++i){
    if (need[customer_num][i] < request[i]){
      printf("%s\n", "Unsafe! Need < Request");
      return -1;
    }
    if (available[i] < request[i]) {
      printf("%s\n", "Unsafe! Available < Request");
      return -1;
    }
  }
  for (i = 0; i < NUMBER_OF_RESOURCES; ++i){
    available[i] -= request[i];
    allocation[customer_num][i] += request[i];
    need[customer_num][i] -= request[i];
  }
  return 0;
}


void release_resources(int customer_num, int release[])
{
  int i;
  for (i = 0; i < NUMBER_OF_RESOURCES; ++i){
    if (allocation[customer_num][i] < release[i]){
      printf("%s\n", "Cannot release resources of this amount!");
      printf("Customer %d only has resource amount %d, %d, %d, %d\n",
            customer_num, allocation[customer_num][0], allocation[customer_num][1],
            allocation[customer_num][2], allocation[customer_num][3]);
      return;
    }

  }
  for (i = 0; i < NUMBER_OF_RESOURCES; ++i){
    available[i] += release[i];
    need[customer_num][i] = 0;
    need[customer_num][i] = 0;
  }
  printf("Customer %d has released resource amount %d, %d, %d, %d seperately\n",
          customer_num, release[0], release[1], release[2], release[3]);
}
