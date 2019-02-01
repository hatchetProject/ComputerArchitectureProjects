#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "allocator.h"


void welcome(){
  printf("\e[35m>>>Welcome! \e[0m\n");
  printf("\e[35m>>>This is a simple memory allocator without actual data.\e[0m\n");
}


int main(int argc, char const *argv[]){
  if (argc > 2){
    fprintf(stderr, "%s\n", "Only 1 parameter is needed");
    return -1;
  }
  if (atoi(argv[1]) <= 0){
    fprintf(stderr, "%s\n", "Initial amount of memory allocated(bytes) should be greater than 0");
    return -1;
  }


  memory_size = atoi(argv[1]);
  char input[4];
  welcome();
  int should_run = 1;
  struct hole init;
  init.start = 0;
  init.end = memory_size; // one element after the end of the hole
  init.size = memory_size;
  holes[0] = init;
  hole_num = 1;
  process_num = 0;
  memset(contiguous_memory, 0, memory_size);
  while (should_run){
    printf("%s", "allocator>");
    scanf("%s", input);
    if (strncmp(input, "X", 1) == 0) {
      printf("%s\n", "Process terminated.");
      break;
    }
    else if (strncmp(input, "RQ", 2) == 0){
      char pro[2], mode[1];
      int mem, method = -1;
      scanf("%s", pro);
      scanf("%d", &mem);
      scanf("%s", mode);
      int process_id = atoi(&pro[1]) + 1;
      if (strncmp(mode, "F", 1) == 0) method = 0;
      if (strncmp(mode, "B", 1) == 0) method = 1;
      if (strncmp(mode, "W", 1) == 0) method = 2;

      request_memory(contiguous_memory, mem, process_id, method);
    }
    else if (strncmp(input, "RL", 2) == 0){
      char pro[2];
      scanf("%s", pro);
      int process_id = atoi(&pro[1]);
      release_memory(contiguous_memory, process_id);
    }
    else if (strncmp(input, "C", 1) == 0){
      compact();
    }
    else if (strncmp(input, "STAT", 4) == 0){
      status_report();
    }
  }

  return 0;
}
