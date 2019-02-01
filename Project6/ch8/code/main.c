#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "banker.h"


int readMaximum(){
  FILE *file;
  file = fopen("maximum.txt", "r");
  if (file == NULL){
    printf("%s\n", "File does not exist");
    fclose(file);
    return -1;
  }
  else{
    int i, j;
    for (i = 0; i < NUMBER_OF_CUSTOMERS; ++i){
      for (j = 0; j < NUMBER_OF_RESOURCES; ++j){
        fscanf(file, "%d", &maximum[i][j]);
      }
    }
    fclose(file);
    return 0;
  }
}

void init(){
  int i, j;
  for (i = 0; i < NUMBER_OF_CUSTOMERS; ++i){
    for (j = 0; j < NUMBER_OF_RESOURCES; ++j){
      allocation[i][j] = 0;
      need[i][j] = maximum[i][j];
      //printf("%d\n", maximum[i][j]);
    }
  }
}


int main(int argc, char const *argv[]){
  if (argc < 5){
    fprintf(stderr, "%s\n", "Need at least 4 paramters");
    return -1;
  }
  if (atoi(argv[1]) < 0 || atoi(argv[2]) < 0 || atoi(argv[3]) < 0 || atoi(argv[4]) < 0){
    fprintf(stderr, "%s\n", "Available resource amount must not be less than 0");
    return -1;
  }
  int i;
  for (i = 1; i < 5; ++i){
    available[i-1] = atoi(argv[i]);
  }

  char input[2];
  int re[10];
  if (readMaximum() == -1){
    return 0;
  }

  init();

  int customer_num;
  while (scanf("%s", input)){
    if (strcmp(input, "*") == 0) {
      printf("%s\n", "Process terminated");
      break;
    }
    else if (strncmp(input, "RQ", 2) == 0){
      int j;
      scanf("%d", &customer_num);
      for (j = 0; j < 4; ++j){
        scanf("%d", &re[j]);
      }
      if (request_resources(customer_num, re) == -1){
        printf("%s\n", "Request unsuccessful! This operation is ignored.");
      }
      else{
        printf("%s\n", "Request successful!");
      }
    }
    else if (strncmp(input, "RL", 2) == 0){
      int j;
      scanf("%d", &customer_num);
      for (j = 0; j < 4; ++j){
        scanf("%d", &re[j]);
      }
      release_resources(customer_num, re);
    }
  }

  return 0;
}
