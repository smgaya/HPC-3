/** 
 * Quicksort implementation for practical course
 **/

#include "timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define THRESHOLD 10000

void print_list(double *data, int length){
	int i;
	for(i = 0; i < length; i++)
		printf("%e\t", data[i]);
	printf("\n");
}

void quicksort(double *data, int length){
	if (length <= 1) return;
	//print_list(data, length);

	double pivot = data[0];
	double temp;
	int left = 1;
	int right = length - 1;

	do {
		while(left < (length - 1) && data[left] <= pivot) left++;
		while(right > 0 && data[right] >= pivot) right--;
		
		/* swap elements */
		if(left < right){
			temp = data[left];
			data[left] = data[right];
			data[right] = temp;
		}

	} while(left < right);
	
	if(data[right] < pivot){
		data[0] = data[right];
		data[right] = pivot;
	}


	#pragma omp task final(right < THRESHOLD)
	quicksort(data, right);
	
	#pragma omp task final((length - left) < THRESHOLD)
	quicksort(&(data[left]), length - left);
}

int check(double *data, int length){
	int i;
	for(i = 1; i < length; i++)
		if(data[i] < data[i-1]) return 1;
	return 0;
}

int main(int argc, char **argv)
{
	int length;
	double *data;

	int mem_size;

	int i, j, k, n_threads;
	n_threads=2;
	length = 100000000;
	if(argc > 2){
		length = atoi(argv[1]);
		n_threads = atoi(argv[2]);
	}
	omp_set_num_threads(n_threads);

	data = (double*)malloc(length * sizeof(double));
	if(0 == data){
		printf("memory allocation failed");
		return 0;
	}

	/* initialisation */
	srand(0);
	for (i = 0; i < length; i++){
		data[i] = (double)rand() / (double)RAND_MAX;
	}
	
	time_marker_t time = get_time();
	
	//print_list(data, length);
	#pragma omp parallel num_threads(n_threads)
	{
		#pragma omp single nowait
		quicksort(data, length);
	}
	printf("Size of dataset: %d, elapsed time[s] %e \n", length, get_ToD_diff_time(time));

	/*print_list(data, length);*/
	if(check(data, length) != 0)
		printf("ERROR\n");
	
	return(0);
}

