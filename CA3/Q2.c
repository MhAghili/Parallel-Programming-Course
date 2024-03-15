#include <omp.h>
 
#include <stdio.h>
#include <stdlib.h>
#include <limits.h> 
#include <math.h>

#define ARRAY_SIZE 1048576  //2^20

double calculate_mean_parallel(int array[], int size) {
    double sum = 0.0;

    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < size; i++) {
        sum += array[i];
    }

    return sum / size;
}


double calculate_standard_deviation_parallel(int array[], int size, double mean) {
    double sum_squared_diff = 0.0;

    #pragma omp parallel for reduction(+:sum_squared_diff)
    for (int i = 0; i < size; i++) {
        double diff = array[i] - mean;
        sum_squared_diff += diff * diff;
    }

    return sqrt(sum_squared_diff / size);
}

double calculate_mean_serial(int array[], int size) {
    double sum = 0.0;

    for (int i = 0; i < size; i++)
    {
        sum += array[i];
    }

    return sum / size;
}

double calculate_standard_deviation_serial(int array[], int size, double mean) {
    double sum_squared_diff = 0.0;

    for (int i = 0; i < size; i++) {
        double diff = array[i] - mean;
        sum_squared_diff += diff * diff;
    }
    
    return sqrt(sum_squared_diff / size);
}

int main() {
    int array[ARRAY_SIZE];

    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = rand();
    }

    // Seril version
    double start_time_serial = omp_get_wtime();

    double mean_serial = calculate_mean_serial(array, ARRAY_SIZE);
    double std_dev_serial = calculate_standard_deviation_serial(array, ARRAY_SIZE, mean_serial);

    double end_time_serial = omp_get_wtime();
    double elapsed_time_serial = end_time_serial - start_time_serial;

    // Parallel version
    double start_time_parallel = omp_get_wtime();

    double mean_parallel = calculate_mean_parallel(array, ARRAY_SIZE);
    double std_dev_parallel = calculate_standard_deviation_parallel(array, ARRAY_SIZE, mean_parallel);

    double end_time_parallel = omp_get_wtime();
    double elapsed_time_parallel = end_time_parallel - start_time_parallel;

    // Output results
    printf("Serial Version:   Mean = %f, Standard Deviation = %f, Time = %f seconds\n", mean_serial, std_dev_serial, elapsed_time_serial);
    printf("Parallel Version: Mean = %f, Standard Deviation = %f, Time = %f seconds\n", mean_parallel, std_dev_parallel, elapsed_time_parallel);

    // Report speedup
    double speedup = elapsed_time_serial / elapsed_time_parallel;
    printf("Speedup: %f\n", speedup);

    return 0;
}