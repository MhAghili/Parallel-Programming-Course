#include <omp.h> //only using for timing function
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 
#include <semaphore.h>
#include <limits.h>
#include <math.h>

#define ARRAY_SIZE 1048576  // 2^20
#define NUM_THREADS 4

typedef struct {
    int* array;
    int size;
    double result;
} ThreadData; 

void* calculate_mean_thread(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    double sum = 0.0;

    for (int i = 0; i < data->size; i++) {
        sum += data->array[i];
    }

    data->result = sum / data->size;

    pthread_exit(NULL);
}

void* calculate_std_dev_thread(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    double sum_squared_diff = 0.0;

    for (int i = 0; i < data->size; i++) {
        double diff = data->array[i] - data->result;
        sum_squared_diff += diff * diff;
    }

    data->result = sqrt(sum_squared_diff / data->size);

    pthread_exit(NULL);
}

double calculate_mean_parallel(int array[], int size) {
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];

    int chunk_size = size / NUM_THREADS;
    int remainder = size % NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].array = &array[i * chunk_size];
        thread_data[i].size = (i == NUM_THREADS - 1) ? chunk_size + remainder : chunk_size;

        pthread_create(&threads[i], NULL, calculate_mean_thread, (void*)&thread_data[i]);
    }

    double sum = 0.0;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
        sum += thread_data[i].result;
    }

    return sum / NUM_THREADS;
}

double calculate_standard_deviation_parallel(int array[], int size, double mean) {
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];

    int chunk_size = size / NUM_THREADS;
    int remainder = size % NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].array = &array[i * chunk_size];
        thread_data[i].size = (i == NUM_THREADS - 1) ? chunk_size + remainder : chunk_size;

        pthread_create(&threads[i], NULL, calculate_std_dev_thread, (void*)&thread_data[i]);
    }

    double sum_squared_diff = 0.0;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
        sum_squared_diff += thread_data[i].result;
    }

    return sqrt(sum_squared_diff / NUM_THREADS);
}

double calculate_mean_serial(int array[], int size) {
    double sum = 0.0;

    for (int i = 0; i < size; i++) {
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

    // Serial version
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
