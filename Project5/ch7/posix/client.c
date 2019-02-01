/**
 * Example client program that uses thread pool.
 */

#include <stdio.h>
#include <unistd.h>
#include "threadpool.h"

struct data
{
    int a;
    int b;
};

void add(void *param)
{
    struct data *temp;
    temp = (struct data*)param;

    printf("I add two values %d and %d result = %d\n",temp->a, temp->b, temp->a + temp->b);
}

int main(void)
{
    // create some work to do
    struct data work;
    work.a = 5;
    work.b = 10;

    struct data work1;
    work1.a = 10;
    work1.b = 20;
    // initialize the thread pool
    pool_init();
    // submit the work to the queue
    int i;
    for (i = 0; i < 10; ++i)
      pool_submit(&add,&work);
    pool_submit(&add, &work1);
    // may be helpful
    //sleep(3);
    pool_shutdown();

    return 0;
}
