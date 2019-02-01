/**
 * Fork/join parallelism in Java
 *
 * Figure 4.18
 *
 * @author Gagne, Galvin, Silberschatz
 * Operating System Concepts  - Tenth Edition
 * Copyright John Wiley & Sons - 2018
 */

import java.util.concurrent.*;

public class QuickSort extends RecursiveAction
{
    static final int SIZE = 10;
    static final int THRESHOLD = 2;

    private int begin;
    private int end;
    private int[] array;

    public QuickSort(int begin, int end, int[] array) {
        this.begin = begin;
        this.end = end;
        this.array = array;
    }

    protected void compute() {
        if (end - begin < THRESHOLD) {
            // Simple but complex sort stage
            Integer i, j, tmp;
            for (i = 0; i < end - begin; ++i){
              for (j = 0; j < end - begin - i; ++j){
                if (array[j+begin] > array[j+begin+1]){
                  tmp = array[j+begin];
                  array[j+begin] = array[j+begin+1];
                  array[j+begin+1] = tmp;
                }
              }
            }
        }
        else {
            // Quicksort stage
            Integer i, j, tmp;
            i = begin;
            j = end;
            tmp = array[begin];
            while (i < j){
              while (array[j] >= tmp && j > i) j--;
              if (j > i){
                array[i] = array[j];
                i++;
                while (array[i] <= tmp && i < j) i++;
                if (i < j){
                  array[j] = array[i];
                  j--;
                }
              }
            }
            array[i] = tmp;

            QuickSort leftTask = new QuickSort(begin, i-1, array);
            QuickSort rightTask = new QuickSort(j+1, end, array);

            leftTask.fork();
            rightTask.fork();

            rightTask.join();
            leftTask.join();
        }
    }

	public static void main(String[] args) {
		ForkJoinPool pool = new ForkJoinPool();
		int[] array = new int[SIZE];

		// create SIZE random integers between 0 and 9
		java.util.Random rand = new java.util.Random();

		for (int i = 0; i < SIZE; i++) {
			array[i] = rand.nextInt(10);
		}

		// use fork-join parallelism to sum the array
		QuickSort task = new QuickSort(0, SIZE-1, array);

		pool.invoke(task);
    for (int i = 0; i < SIZE; ++i)
      System.out.println(array[i]);
	}
}
