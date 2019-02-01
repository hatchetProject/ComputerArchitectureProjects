/*
  Implementing the SJF method

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"
#include "cpu.h"
#include "schedulers.h"

#define MINIMUM 99999

struct node *sjf_queue;

void add(char *name, int priority, int burst){
  Task *newTask = malloc(sizeof(Task));
  newTask->name = name;
  newTask->priority = priority;
  newTask->burst = burst;
  insert(&sjf_queue, newTask);
}



void schedule(){
  int time_count = 0;
  struct node *head = sjf_queue;

  while (head){
    struct node *tmp = head;
    int minimum = MINIMUM;
    Task *miniTask;
    while (tmp){
      if (tmp->task->burst <= minimum){
        minimum = tmp->task->burst;
        miniTask = tmp->task;
      }
      tmp = tmp->next;
    }
    time_count = minimum;
    run(miniTask, time_count);
    delete(&sjf_queue, miniTask);
    head = sjf_queue;
  }
}
