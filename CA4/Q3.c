#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

#define FRAME_SIZE 10000

void calculate_absolute_difference(int frame1[], int frame2[], int result[], int size) {
    int i;
    #pragma omp parallel for private(i)
    for (int i = 0; i < size; i++) {
        result[i] = abs(frame1[i] - frame2[i]);
    }
}

int main() {

    int frame1[FRAME_SIZE];
    int frame2[FRAME_SIZE];
    int result[FRAME_SIZE];

    for (int i = 0; i < FRAME_SIZE; i++) {
        frame1[i] = rand();
        frame2[i] = rand();
    }

    double start_time_serial = omp_get_wtime();

    for (int i = 0; i < FRAME_SIZE; i++) {
        result[i] = abs(frame1[i] - frame2[i]);
    }

    double end_time_serial = omp_get_wtime();
    double elapsed_time_serial = end_time_serial - start_time_serial;

    // Parallel version
    double start_time_parallel = omp_get_wtime();

    calculate_absolute_difference(frame1, frame2, result, FRAME_SIZE);

    double end_time_parallel = omp_get_wtime();
    double elapsed_time_parallel = end_time_parallel - start_time_parallel;

    printf("Serial Version:   Time = %f seconds\n", elapsed_time_serial);
    printf("Parallel Version: Time = %f seconds\n", elapsed_time_parallel);

    double speedup = elapsed_time_serial / elapsed_time_parallel;
    printf("Speedup: %f\n", speedup);

    return 0;
}
