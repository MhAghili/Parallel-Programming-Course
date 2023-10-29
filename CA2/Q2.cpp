#include <iostream>
#include <immintrin.h>
#include <cmath>
#include <chrono>

void calculateMeanAndstandardDevisionSIMD(float *data, int arraySize, float &mean, float &standardDevision)
{
    __m256 sum = _mm256_setzero_ps();
    __m256 sumOfSquares = _mm256_setzero_ps();

    for (int i = 0; i < arraySize; i += 8)
    {
        __m256 values = _mm256_loadu_ps(&data[i]);
        sum = _mm256_add_ps(sum, values);

        __m256 squaredValues = _mm256_mul_ps(values, values);

        sumOfSquares = _mm256_add_ps(sumOfSquares, squaredValues);
    }

    // Reduce SIMD variables to get the total sum and sum of squares
    __m128 sum128 = _mm_add_ps(_mm256_castps256_ps128(sum), _mm256_extractf128_ps(sum, 1));
    __m128 sumOfSquares128 = _mm_add_ps(_mm256_castps256_ps128(sumOfSquares), _mm256_extractf128_ps(sumOfSquares, 1));

    // Horizontal additione
    sum128 = _mm_hadd_ps(sum128, sum128);
    sumOfSquares128 = _mm_hadd_ps(sumOfSquares128, sumOfSquares128);

    // Sum the results
    float totalSum, totalSumOfSquares;
    _mm_store_ss(&totalSum, sum128);
    _mm_store_ss(&totalSumOfSquares, sumOfSquares128);

    mean = totalSum / arraySize;
    float variance = (totalSumOfSquares / arraySize) - (mean * mean);
    standardDevision = sqrt(variance);
}

void calculateMeanAndstandardDevisionSerial(float *data, int arraySize, float &mean, float &standardDevision)
{
    float sum = 0.0;
    float sumOfSquares = 0.0;

    for (int i = 0; i < arraySize; i++)
    {
        float value = data[i];
        sum += value;
        sumOfSquares += value * value;
    }

    mean = sum / arraySize;
    float variance = (sumOfSquares / arraySize) - (mean * mean);
    standardDevision = sqrt(variance);
}

int main()
{
    const int arraySize = 1 << 20;
    float *data = new float[arraySize];

    for (int i = 0; i < arraySize; i++)
    {
        data[i] = static_cast<float>(rand()) / RAND_MAX;
    }

    float meanSIMD, standardDevisionSIMD;
    float meanSerial, standardDevisionSerial;

    auto startSIMD = std::chrono::high_resolution_clock::now();
    calculateMeanAndstandardDevisionSIMD(data, arraySize, meanSIMD, standardDevisionSIMD);
    auto endSIMD = std::chrono::high_resolution_clock::now();

    auto startSerial = std::chrono::high_resolution_clock::now();
    calculateMeanAndstandardDevisionSerial(data, arraySize, meanSerial, standardDevisionSerial);
    auto endSerial = std::chrono::high_resolution_clock::now();

    std::cout << "Mean: " << meanSIMD << std::endl;
    std::cout << "Standard Deviation: " << standardDevisionSIMD << std::endl;

    auto SMIDTime = std::chrono::duration_cast<std::chrono::microseconds>(endSIMD - startSIMD).count();
    auto SerialTime = std::chrono::duration_cast<std::chrono::microseconds>(endSerial - startSerial).count();

    std::cout << "Time SIMD: " << SMIDTime << " ms" << std::endl;
    std::cout << "Time Serial " << SerialTime << " ms" << std::endl;

    double speedUp = static_cast<double>(SMIDTime) / SerialTime;

    std::cout << "Acceleration: " << 1 / speedUp << std::endl;

    delete[] data;

    return 0;
}
