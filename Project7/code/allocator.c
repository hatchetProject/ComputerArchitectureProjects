#include <stdio.h>
#include <stdlib.h>
#include "allocator.h"

#define maximum 10000000

void sort_holes(){
  struct hole tmp_hole;
  int i, j;
  for (i = 0; i < hole_num - 1; ++i){
    for (j = 0; j < hole_num - i - 1; ++j){
      if (holes[j].start > holes[j+1].start){
        tmp_hole = holes[j];
        holes[j] = holes[j+1];
        holes[j+1] = tmp_hole;
      }
    }
  }
}


void request_memory(int contiguous_memory[], int request_size, int process_id, int method){
  // method == 0: First-fit; method == 1: Best-fit; method == 2: Worst-Fit
  int i, j;
  if (method == 0){
    int flag = 0;
    for (i = 0; i < hole_num; ++i){
      if (request_size <= holes[i].size){
        flag = 1;
        struct hole tmp_hole = holes[i];

        struct process new_process;
        new_process.start = tmp_hole.start;
        new_process.end = new_process.start + request_size;
        new_process.size = request_size;
        new_process.process_id = process_id - 1;
        processes[process_num++] = new_process;

        for (j = 0; j < request_size; ++j){
          contiguous_memory[j + tmp_hole.start] = process_id;
        }

        if (request_size == tmp_hole.size){
          for (j = i; j < hole_num - 1; ++j) holes[j] = holes[j+1];
          hole_num -= 1;
        }
        else{
          tmp_hole.start = tmp_hole.start + request_size;
          tmp_hole.size = tmp_hole.size - request_size;
          holes[i] = tmp_hole;
        }
        printf("%s\n", "Allocation Successful!!");
        break;
      }
    }
    if (flag == 0){
      printf("%s\n", "Request size too big, no space in memory! Compacting or releasing memory are recommended.");
    }
  }
  else if (method == 1){
    int max_index = -1, max_size = maximum, flag = 0;
    for (i = 0; i < hole_num; ++i){
      if (holes[i].size < max_size && holes[i].size >= request_size){
        flag = 1;
        max_size = holes[i].size;
        max_index = i;
      }
    }
    if (flag == 0){
      printf("%s\n", "Request size too big, no space in memory! Compacting or releasing memory are recommended.");
      return;
    }

    struct process new_process;
    struct hole tmp_hole = holes[max_index];
    new_process.start = tmp_hole.start;
    new_process.end = new_process.start + request_size;
    new_process.size = request_size;
    new_process.process_id = process_id - 1;
    processes[process_num++] = new_process;


    for (j = 0; j < request_size; ++j)
      contiguous_memory[j + tmp_hole.start] = process_id;

    if (request_size == tmp_hole.size){
      for (j = i; j < hole_num - 1; ++j) holes[j] = holes[j+1];
      hole_num -= 1;
    }
    else{
      tmp_hole.start = tmp_hole.start + request_size;
      tmp_hole.size = tmp_hole.size - request_size;
      holes[max_index] = tmp_hole;
    }
    printf("%s\n", "Allocation Successful!!");
  }
  else{
    int max_index = -1, max_size = 0;
    for (i = 0; i < hole_num; ++i){
      if (holes[i].size > max_size){
        max_size = holes[i].size;
        max_index = i;
      }
    }
    if (max_size < request_size){
      printf("%s\n", "Request size too big, no space in memory! Compacting or releasing memory are recommended.");
      return;
    }

    struct process new_process;
    struct hole tmp_hole = holes[max_index];
    new_process.start = tmp_hole.start;
    new_process.end = new_process.start + request_size;
    new_process.size = request_size;
    new_process.process_id = process_id - 1;
    processes[process_num++] = new_process;

    for (j = 0; j < request_size; ++j)
      contiguous_memory[j + tmp_hole.start] = process_id;
    if (request_size == tmp_hole.size){
      for (j = i; j < hole_num - 1; ++j) holes[j] = holes[j+1];
      hole_num -= 1;
    }
    else{
      tmp_hole.start = tmp_hole.start + request_size;
      tmp_hole.size = tmp_hole.size - request_size;
      holes[max_index] = tmp_hole;
    }
    printf("%s\n", "Allocation Successful!!");
  }
  sort_holes();
}


void release_memory(int contiguous_memory[], int process_id){
  int i, j, id = -1;
  for (i = 0; i < process_num; ++i){
    if (processes[i].process_id == process_id){
      id = i;
      break;
    }
  }

  if (id == -1) {
    printf("%s\n", "No such process exist!");
    return;
  }
  struct process tmp_process = processes[id];
  for (i = 0; i < tmp_process.size; ++i){
    contiguous_memory[i + tmp_process.start] = 0;
  }

  for (j = id; j < process_num-1; ++j){
    processes[j] = processes[j+1];
  }
  process_num = process_num - 1;
  check_release(tmp_process.start, tmp_process.end, tmp_process.size);

  printf("%s\n", "Release Successful!");
  sort_holes();
}


int check_release(int start, int end, int size){
  // flag number 1 if the total number of holes does not change;
  // flag number 2 if the total number of holes decreases;
  // flag number 0 if the total number of holes increases.
  int i, j, flag = 0;
  int id1 = -1, id2 = -1;
  for (i = 0; i < hole_num; ++i){
    if (holes[i].end == start){
      flag += 1;
      holes[i].end = end;
      holes[i].size += size;
      id1 = i;
    }
    if (holes[i].start == end){
      flag += 1;
      holes[i].start = start;
      holes[i].size += size;
      id2 = i;
    }
  }

  if (flag == 1){
    if (id2 == -1){
      for (j = id1; j < hole_num-1; ++j) holes[j] = holes[j+1];
    }
    else{
      for (j = id2; j < hole_num-1; ++j) holes[j] = holes[j+1];
    }
  }

  if (flag == 2){
    struct hole new_hole;
    new_hole.start = holes[id1].start;
    new_hole.end = holes[id2].end;
    new_hole.size = holes[id1].size + holes[id2].size - size;
    for (j = id1; j < hole_num-1; ++j) holes[j] = holes[j+1];
    hole_num -= 1;
    for (j = id2; j < hole_num-1; ++j) holes[j] = holes[j+1];
    hole_num -= 1;
    holes[hole_num++] = new_hole;
  }

  if (flag == 0){
    struct hole new_hole;
    new_hole.start = start;
    new_hole.end = end;
    new_hole.size = size;
    holes[hole_num++] = new_hole;
  }
}


void status_report(){
  int i;
  printf("%s\n", "Allocated regions:");
  for (i = 0; i < process_num; ++i){
    printf("Addresses [%d:%d] Process P%d\n", processes[i].start, processes[i].end-1, processes[i].process_id);
  }
  printf("%s\n", "Unused regions:");
  for (i = 0; i < hole_num; ++i){
    printf("Addresses [%d:%d] Unused\n", holes[i].start, holes[i].end-1);
  }
}


void compact(){
  int total_hole_size = 0, i;
  for (i = 0; i< hole_num; ++i){
    total_hole_size += holes[i].size;
  }
  int recent_start = 0;
  for (i = 0; i < process_num; ++i){
    struct process tmp_process = processes[i];
    tmp_process.start = recent_start;
    tmp_process.end = recent_start + tmp_process.size;
    recent_start = tmp_process.end;
    processes[i] = tmp_process;
  }
  if (total_hole_size == 0) hole_num = 0;
  else hole_num = 1;

  struct hole tmp_hole;
  tmp_hole.start = processes[process_num-1].end;
  tmp_hole.end = memory_size;
  tmp_hole.size = total_hole_size;
  holes[0] = tmp_hole;
  printf("%s\n", "Compacting Finished!");
}
