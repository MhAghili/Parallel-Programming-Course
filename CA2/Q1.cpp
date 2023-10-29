#include <iostream>
#include <immintrin.h>
#include <chrono>

using namespace std;

void findMinElementSIMD(float *data, int arraySize, int &minIndex, float &minValue)
{
    minIndex = 0;
    minValue = data[0];

    __m256 minValues = _mm256_set1_ps(minValue);
    __m256i minIndices = _mm256_set1_epi32(0);

    for (int i = 0; i < arraySize; i += 8)
    {
        // Load 8
        __m256 values = _mm256_loadu_ps(&data[i]);

        // Compare them  with curr minimum
        __m256 cmpMask = _mm256_cmp_ps(values, minValues, _CMP_LT_OQ);

        // Update minimum
        minValues = _mm256_blendv_ps(minValues, values, cmpMask);
        minIndices = _mm256_blendv_epi8(minIndices, _mm256_set1_epi32(i), _mm256_castps_si256(cmpMask));
    }

    // Find the minimum value and its index among the 8 values
    __m128 minValues128 = _mm_min_ps(_mm256_castps256_ps128(minValues), _mm256_extractf128_ps(minValues, 1));
    __m128i minIndices128 = _mm_min_epi32(_mm256_castsi256_si128(minIndices), _mm256_extractf128_si256(minIndices, 1));

    // Temporary arrays to store the results
    int minIndicesArray[4];
    float minValuesArray[4];

    // Store the results into arrays
    _mm_storeu_si128((__m128i *)minIndicesArray, minIndices128);
    _mm_storeu_ps(minValuesArray, minValues128);

    // Extract the scalar values from the arrays
    minIndex = minIndicesArray[0];
    minValue = minValuesArray[0];
}

void findMinElementSerial(float *data, int arraySize, int &minIndex, float &minValue)
{
    minIndex = 0;
    minValue = data[0];

    for (int i = 1; i < arraySize; i++)
    {
        if (data[i] < minValue)
        {
            minValue = data[i];
            minIndex = i;
        }
    }
}

int main()
{
    const int arraySize = 1 << 20;
    float *data = new float[arraySize];

    for (int i = 0; i < arraySize; i++)
    {
        data[i] = static_cast<float>(rand()) / RAND_MAX;
    }

    int minIndex;
    float minValue;

    auto startSIMD = std::chrono::high_resolution_clock::now();
    findMinElementSIMD(data, arraySize, minIndex, minValue);
    auto endSIMD = std::chrono::high_resolution_clock::now();

    auto startSerial = std::chrono::high_resolution_clock::now();
    findMinElementSerial(data, arraySize, minIndex, minValue);
    auto endSerial = std::chrono::high_resolution_clock::now();

    cout << "Minimum Value : " << minValue << endl;
    cout << "Index: " << minIndex << endl;

    double SMIDTime = std::chrono::duration<double>(endSIMD - startSIMD).count();
    double SerialTime = std::chrono::duration<double>(endSerial - startSerial).count();
    double Speedup = SMIDTime / SerialTime;
    cout << "Acceleration: " << 1 / Speedup << endl;

    delete[] data;

    return 0;
}
