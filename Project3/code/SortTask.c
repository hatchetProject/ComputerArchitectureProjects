/**
 * A pthread program illustrating how to
 * create a simple thread and some of the pthread API
 * This program implements the summation function where
 * the summation operation is run as a separate thread.
 *
 * Most Unix/Linux/OS X users
 * gcc thrd.c -lpthread
 *
 * Figure 4.11
 *
 * @author Gagne, Galvin, Silberschatz
 * Operating System Concepts  - Tenth Edition
 * Copyright John Wiley & Sons - 2018
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int array[] = {2, 5, 1, 4, 3}; /* this data is shared by the thread(s) */
int n = 5;

struct parameters
{
  int low;
  int high;
};

void *runner(void *param); /* the thread */
void *merge(void *param);

int main(int argc, char *argv[])
{
  pthread_t tid1, tid2, tid3; /* the thread identifier */
  pthread_attr_t attr; /* set of attributes for the thread */

  if (argc != 1) {
	   fprintf(stderr,"usage: a.out <integer value>\n");
	    /*exit(1);*/
	     return -1;
  }

  //if (atoi(argv[1]) < 0) {
	//   fprintf(stderr,"Argument %d must be non-negative\n",atoi(argv[1]));
	    /*exit(1);*/
	//     return -1;
  //}

  /* get the default attributes */
  pthread_attr_init(&attr);
  struct parameters p1, p2;
  p1.low = 0;
  p1.high = n/2;
  p2.low = n/2;
  p2.high = n;
  int i;
  /* create the thread */
  pthread_create(&tid1, &attr, runner, (void *)&p1);
  //pthread_create(&tid, &attr, merge, NULL);
  /* now wait for the thread to exit */

  //pthread_attr_init(&attr);
  pthread_create(&tid2, &attr, runner, (void *)&p2);
  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);
  //for (i = 0; i < n; ++i)
  //  printf("%d ", array[i]);



  pthread_attr_init(&attr);
  pthread_create(&tid3, &attr, merge, NULL);
  pthread_join(tid3, NULL);

  for (i = 0; i < n; ++i)
    printf("%d ", array[i]);
  printf("\n");
}

/**
 * The thread will begin control in this function
 */
void *runner(void *param)
{
  struct parameters *p = (struct parameters *)param;
  int lower = p->low, upper = p->high, i, j, tmp;
  for (i = 0; i < upper-lower-1; ++i){
    for (j = 0; j < upper-i-lower-1; ++j){
      if (array[j+lower] > array[j+lower+1]){
        tmp = array[j+lower];
        array[j+lower] = array[j+lower+1];
        array[j+lower+1] = tmp;
      }
    }
  }
	pthread_exit(0);
}

void *merge(void *param)
{
  int len1 = n/2, len2 = n - n/2;
  int i = 0, j = n/2, num = 0;
  int result[n];
  while (i < n/2 && j < n){
    if (array[i] < array[j]){
      result[num++] = array[i++];
    }
    else{
      result[num++] = array[j++];
    }
  }

  while (i < n/2){
    result[num++] = array[i++];
  }
  while (j < n){
    result[num++] = array[j++];
  }
  for (i = 0; i < n; ++i)
    array[i] = result[i];

  pthread_exit(0);
}
