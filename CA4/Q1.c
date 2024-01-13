#include <omp.h>
 
#include <stdio.h>
#include <stdlib.h>
#include <limits.h> 

#define ARRAY_SIZE 1048576  //2^20


int main() {
    int array[ARRAY_SIZE];


    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = rand();
    }


    int min_serial = INT_MAX;
    int index_serial;

    double start_time_serial = omp_get_wtime();

    // serial 
    for (int i = 0; i < ARRAY_SIZE; i++) {
        if (array[i] < min_serial) {
            min_serial = array[i];
            index_serial = i;
        }
    }

    double end_time_serial = omp_get_wtime();
    double serial_time = end_time_serial - start_time_serial;

    // Parallel 
    int min_parallel = INT_MAX;
    int index_parallel;

    double start_time_parallel = omp_get_wtime();
    
    int i;
    #pragma omp parallel for private(i)
    for (int i = 0; i < ARRAY_SIZE; i++) {
        {
            if (array[i] < min_parallel) {
                min_parallel = array[i];
                index_parallel = i;
            }
        }
    }

    double end_time_parallel = omp_get_wtime();
    double parallel_time = end_time_parallel - start_time_parallel;


    printf("Serial Version:   Minimum = %d, Index = %d, Time = %f seconds\n", min_serial, index_serial, serial_time);
    printf("Parallel Version: Minimum = %d, Index = %d, Time = %f seconds\n", min_parallel, index_parallel, parallel_time);

   
    double speedUp = serial_time / parallel_time;
    printf("speedUp: %f\n", speedUp);

    return 0;
}