#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define WIDTH 1024
#define HEIGHT 1024
#define MAX_COLOR_VALUE 255
#define TRANSPARENCY 0.25

typedef struct {
    unsigned char red, green, blue;
} Pixel;

void addImagesWithTransparencySerial(Pixel* img1, Pixel* img2, Pixel* result, int size);
void savePPM(const char* filename, Pixel* image, int width, int height) ;
void addImagesWithTransparencyParallel(Pixel* img1, Pixel* img2, Pixel* result, int size);

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: %s <image1.ppm> <image2.ppm> <output.ppm>\n", argv[0]);
        return 1;
    }

    FILE* file1 = fopen(argv[1], "rb");
    FILE* file2 = fopen(argv[2], "rb");

    if (!file1 || !file2) {
        printf("Error opening input files.\n");
        return 1;
    }

    Pixel* image1 = (Pixel*)malloc(WIDTH * HEIGHT * sizeof(Pixel));
    Pixel* image2 = (Pixel*)malloc(WIDTH * HEIGHT * sizeof(Pixel));
    Pixel* result_serial = (Pixel*)malloc(WIDTH * HEIGHT * sizeof(Pixel));
    Pixel* result_parallel = (Pixel*)malloc(WIDTH * HEIGHT * sizeof(Pixel));

    fread(image1, sizeof(Pixel), WIDTH * HEIGHT, file1);
    fread(image2, sizeof(Pixel), WIDTH * HEIGHT, file2);

    fclose(file1);
    fclose(file2);

    double start_time_serial = omp_get_wtime();
    addImagesWithTransparencySerial(image1, image2, result_serial, WIDTH * HEIGHT);
    double end_time_serial = omp_get_wtime();
    printf("Serial Execution time: %f seconds\n", end_time_serial - start_time_serial);

    double start_time_parallel = omp_get_wtime();
    addImagesWithTransparencyParallel(image1, image2, result_parallel, WIDTH * HEIGHT);
    double end_time_parallel = omp_get_wtime();
    printf("Parallel Execution time: %f seconds\n", end_time_parallel - start_time_parallel);

    double speedup = (end_time_serial - start_time_serial) / (end_time_parallel - start_time_parallel);
    printf("Speedup: %f\n", speedup);

    savePPM(argv[3], result_parallel, WIDTH, HEIGHT);
    savePPM(argv[3], result_serial, WIDTH, HEIGHT);

    free(image1);
    free(image2);
    free(result_serial);
    free(result_parallel);

}

void addImagesWithTransparencySerial(Pixel* img1, Pixel* img2, Pixel* result, int size) {
    for (int i = 0; i < size; ++i) {
        result[i].red = img1[i].red + (unsigned char)(img2[i].red * TRANSPARENCY);
        result[i].green = img1[i].green + (unsigned char)(img2[i].green * TRANSPARENCY);
        result[i].blue = img1[i].blue + (unsigned char)(img2[i].blue * TRANSPARENCY);
    }
}

void addImagesWithTransparencyParallel(Pixel* img1, Pixel* img2, Pixel* result, int size) {
    #pragma omp parallel for
    for (int i = 0; i < size; ++i) {
        result[i].red = img1[i].red + (unsigned char)(img2[i].red * TRANSPARENCY);
        result[i].green = img1[i].green + (unsigned char)(img2[i].green * TRANSPARENCY);
        result[i].blue = img1[i].blue + (unsigned char)(img2[i].blue * TRANSPARENCY);
    }
}

void savePPM(const char* filename, Pixel* image, int width, int height) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("Error opening file for writing.\n");
        exit(1);
    }

    fprintf(file, "P6\n%d %d\n%d\n", width, height, MAX_COLOR_VALUE);
    fwrite(image, sizeof(Pixel), width * height, file);

    fclose(file);
}