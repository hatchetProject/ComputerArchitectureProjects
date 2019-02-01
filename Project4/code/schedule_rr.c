/*
  Implementing the Round Robin method

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"
#include "cpu.h"
#include "schedulers.h"


struct node *rr_queue;

void add(char *name, int priority, int burst){
  Task *newTask = malloc(sizeof(Task));
  newTask->name = name;
  newTask->priority = priority;
  newTask->burst = burst;
  insert(&rr_queue, newTask);
}

void reverse(){
  // Reversing the fcfs_queue
  // We reverse with a head
  struct node *p = rr_queue->next, *q;
  rr_queue->next = NULL;
  while (p){
    q = p;
    p = p->next;
    q->next = rr_queue->next;
    rr_queue->next = q;
  }
}


void schedule(){
  // Add a header to the fcfs_queue
  // The head does not contain needed values
  struct node *tmp = malloc(sizeof(struct node));
  insert(&rr_queue, tmp->task);

  int time_count = 0;
  struct node *head = rr_queue;
  reverse();
  head = head->next;
  rr_queue = head;
  while (1){
    if (!head) break;
    while (head){
      if (head->task->burst <= QUANTUM){
        time_count = head->task->burst;
        delete(&rr_queue, head->task);
      }
      else{
        time_count = QUANTUM;
        head->task->burst -= QUANTUM;
      }
      run(head->task, time_count);
      head = head->next;
    }
    head = rr_queue;
  }
}
