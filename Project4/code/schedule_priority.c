/*
  Implementing the Priority Scheduling method

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"
#include "cpu.h"
#include "schedulers.h"

#define MAXIMUM 0

struct node *priority_queue;

void add(char *name, int priority, int burst){
  Task *newTask = malloc(sizeof(Task));
  newTask->name = name;
  newTask->priority = priority;
  newTask->burst = burst;
  insert(&priority_queue, newTask);
}



void schedule(){
  int time_count = 0;
  struct node *head = priority_queue;

  while (head){
    struct node *tmp = head;
    int maximum = MAXIMUM, time_cost;
    Task *maxTask;
    while (tmp){
      if (tmp->task->priority >= maximum){
        maximum = tmp->task->priority;
        maxTask = tmp->task;
        time_cost = tmp->task->burst;
      }
      tmp = tmp->next;
    }
    time_count = time_cost;
    run(maxTask, time_count);
    delete(&priority_queue, maxTask);
    head = priority_queue;
  }
}
