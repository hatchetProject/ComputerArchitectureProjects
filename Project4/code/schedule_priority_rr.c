/*
  Implementing the Priority + Round Robin method

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"
#include "cpu.h"
#include "schedulers.h"

#define MAXIMUM 0

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

void rr_schedule(struct node **head, int num){
  struct node *p;
  int j;
  p = *head;
  while (1){
    if (num == 0) break;
    for (j = 0; j < num; ++j){
      if (p->task->burst <= QUANTUM){
        run(p->task, p->task->burst);
        delete(head, p->task);
        delete(&rr_queue, p->task);
        num -= 1;
      }
      else{
        run(p->task, QUANTUM);
        p->task->burst -= QUANTUM;
      }
      p = p->next;
      //printf("%s\n", "!!!");
    }
    p = *head;
  }
}


void schedule(){
  // Add a header to the fcfs_queue
  // The head does not contain needed values
  struct node *tmp = malloc(sizeof(struct node));
  insert(&rr_queue, tmp->task);

  struct node *head = rr_queue;
  reverse();
  head = head->next;
  rr_queue = head;
  int single = 1;

  while (1){
    if (!head) break;
    int maximum = MAXIMUM, num_count = 1;
    struct node *tmp_rr;

    while (head){
      if (head->task->priority == maximum){
        single = 0;
        Task *newTask = malloc(sizeof(Task));
        newTask->name = head->task->name;
        newTask->priority = head->task->priority;
        newTask->burst = head->task->burst;
        insert(&tmp_rr, newTask);
        num_count += 1;
      }
      else if (head->task->priority > maximum){
        single = 1;
        maximum = head->task->priority;
        free(tmp_rr);
        Task *newTask = malloc(sizeof(Task));
        newTask->name = head->task->name;
        newTask->priority = head->task->priority;
        newTask->burst = head->task->burst;
        insert(&tmp_rr, newTask);
      }
      head = head->next;
    }
    if (single){
      run(tmp_rr->task, tmp_rr->task->burst);
      delete(&rr_queue, tmp_rr->task);
    }
    else{
      rr_schedule(&tmp_rr, num_count);
    }

    head = rr_queue;
  }
}
