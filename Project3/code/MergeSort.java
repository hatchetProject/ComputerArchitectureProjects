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

public class MergeSort extends RecursiveAction
{
    static final int SIZE = 10;
    static final int THRESHOLD = 2;

    private int begin;
    private int end;
    private int[] array;

    public MergeSort(int begin, int end, int[] array) {
        this.begin = begin;
        this.end = end;
        this.array = array;
    }

    protected void compute() {
        if (end - begin < THRESHOLD) {
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
            int mid = begin + (end - begin) / 2;

            MergeSort leftTask = new MergeSort(begin, mid, array);
            MergeSort rightTask = new MergeSort(mid + 1, end, array);

            leftTask.fork();
            rightTask.fork();

            rightTask.join();
            leftTask.join();

            int []tmp_array = new int[SIZE];
            int num = 0;
            int i = begin, j = mid+1;
            while (i <= mid && j <= end){
              if (array[i] < array[j]){
                tmp_array[num] = array[i];
                num++;
                i++;
              }
              else{
                tmp_array[num] = array[j];
                num++;
                j++;
              }
            }

            while (i <= mid){
              tmp_array[num] = array[i];
              num++;
              i++;
            }

            while (j <= end){
              tmp_array[num] = array[j];
              num++;
              j++;
            }

            for (int k = 0; k < num; ++k){
              array[begin+k] = tmp_array[k];
            }

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
		MergeSort task = new MergeSort(0, SIZE-1, array);

		pool.invoke(task);

    for (int i = 0; i < SIZE; ++i)
		  System.out.println(array[i]);
	}
}
