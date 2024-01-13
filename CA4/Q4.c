#include <omp.h> //only using for timing function
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include "lodepng.h"

#define WIDTH 1024
#define HEIGHT 1024
#define TRANSPARENCY 0.25
#define NUM_THREADS 4

typedef struct {
    uint8_t red, green, blue, alpha;
} Pixel;

typedef struct {
    Pixel* img1;
    Pixel* img2;
    Pixel* result;
    int size;
    int thread_id;
} ThreadData;

void* addImagesWithTransparencyThread(void* arg);
void addImagesWithTransparencySerial(Pixel* img1, Pixel* img2, Pixel* result, int size);
void addImagesWithTransparencyParallel(Pixel* img1, Pixel* img2, Pixel* result, int size);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <image1.png> <image2.png>\n", argv[0]);
        return 1;
    }

    // Read images
    Pixel* image1;
    Pixel* image2;
    int width1, height1;
    int width2, height2;

    if (lodepng_decode32_file((unsigned char**)&image1, &width1, &height1, argv[1]) != 0 ||
        lodepng_decode32_file((unsigned char**)&image2, &width2, &height2, argv[2]) != 0) {
        printf("Error: Failed to load PNG files.\n");
        return 1;
    }

    if (width1 != width2 || height1 != height2) {
        printf("Error: Images have different dimensions.\n");
        free(image1);
        free(image2);
        return 1;
    }

    Pixel* result_serial = (Pixel*)malloc(width1 * height1 * sizeof(Pixel));
    Pixel* result_parallel = (Pixel*)malloc(width1 * height1 * sizeof(Pixel));

    // Serial version
    double start_time_serial = omp_get_wtime();
    addImagesWithTransparencySerial(image1, image2, result_serial, width1 * height1);
    double end_time_serial = omp_get_wtime();
    printf("Serial Execution time: %f seconds\n", end_time_serial - start_time_serial);

    // Parallel version
    double start_time_parallel = omp_get_wtime();
    addImagesWithTransparencyParallel(image1, image2, result_parallel, width1 * height1);
    double end_time_parallel = omp_get_wtime();
    printf("Parallel Execution time: %f seconds\n", end_time_parallel - start_time_parallel);

    double speedup = (end_time_serial - start_time_serial) / (end_time_parallel - start_time_parallel);
    printf("Speedup: %f\n", speedup);

    // Save results
    if (lodepng_encode32_file("output_serial.png", (const unsigned char*)result_serial, width1, height1) != 0 ||
        lodepng_encode32_file("output_parallel.png", (const unsigned char*)result_parallel, width1, height1) != 0) {
        printf("Error: Failed to save PNG files.\n");
    }

    free(image1);
    free(image2);
    free(result_serial);
    free(result_parallel);

    return 0;
}

void* addImagesWithTransparencyThread(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    int start = (data->thread_id * data->size) / NUM_THREADS;
    int end = ((data->thread_id + 1) * data->size) / NUM_THREADS;

    for (int i = start; i < end; ++i) {
        data->result[i].red = data->img1[i].red + (uint8_t)(data->img2[i].red * TRANSPARENCY);
        data->result[i].green = data->img1[i].green + (uint8_t)(data->img2[i].green * TRANSPARENCY);
        data->result[i].blue = data->img1[i].blue + (uint8_t)(data->img2[i].blue * TRANSPARENCY);
        data->result[i].alpha = data->img1[i].alpha;
    }

    pthread_exit(NULL);
}

void addImagesWithTransparencySerial(Pixel* img1, Pixel* img2, Pixel* result, int size) {
    for (int i = 0; i < size; ++i) {
        result[i].red = img1[i].red + (uint8_t)(img2[i].red * TRANSPARENCY);
        result[i].green = img1[i].green + (uint8_t)(img2[i].green * TRANSPARENCY);
        result[i].blue = img1[i].blue + (uint8_t)(img2[i].blue * TRANSPARENCY);
        result[i].alpha = img1[i].alpha;
    }
}

void addImagesWithTransparencyParallel(Pixel* img1, Pixel* img2, Pixel* result, int size) {
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_data[i].img1 = img1;
        thread_data[i].img2 = img2;
        thread_data[i].result = result;
        thread_data[i].size = size;
        thread_data[i].thread_id = i;

        pthread_create(&threads[i], NULL, addImagesWithTransparencyThread, (void*)&thread_data[i]);
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }
}
