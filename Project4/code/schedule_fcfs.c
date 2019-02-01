/*
  Implementing the FCFS method

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"
#include "cpu.h"
#include "schedulers.h"


struct node *fcfs_queue;

void add(char *name, int priority, int burst){
  Task *newTask = malloc(sizeof(Task));
  newTask->name = name;
  newTask->priority = priority;
  newTask->burst = burst;
  insert(&fcfs_queue, newTask);
}

void reverse(){
  // Reversing the fcfs_queue
  // We reverse with a head
  struct node *p = fcfs_queue->next, *q;
  fcfs_queue->next = NULL;
  while (p){
    q = p;
    p = p->next;
    q->next = fcfs_queue->next;
    fcfs_queue->next = q;
  }
}


void schedule(){
  // Add a header to the fcfs_queue
  // The head does not contain needed values
  struct node *tmp = malloc(sizeof(struct node));
  insert(&fcfs_queue, tmp->task);

  int time_count = 0;
  struct node *head = fcfs_queue;
  reverse();
  head = head->next;
  while (head){
    time_count = head->task->burst;
    run(head->task, time_count);
    delete(&fcfs_queue, head->task);
    head = head->next;
  }
}
