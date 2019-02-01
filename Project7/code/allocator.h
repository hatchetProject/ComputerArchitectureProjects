#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#define MAX_NUMBER 100
#define MAX_MEMORY_SPACE 1048576 // Maximum of 256M memory is guaranteed


void request_memory(int contiguous_memory[], int request_size, int process_id, int method);
void release_memory(int contiguous_memory[], int process_id);
void compact();
void status_report();

struct hole{
    int start;
    int end;
    int size;
};

struct process{
    int start;
    int end;
    int size;
    int process_id;
};

struct hole holes[MAX_NUMBER];
int hole_num;
struct process processes[MAX_NUMBER];
int process_num;
int memory_size;
int contiguous_memory[MAX_MEMORY_SPACE];

#endif
