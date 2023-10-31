#include <iostream>
#include <immintrin.h>
#include <chrono>
#include "lodepng.h"

void frameDifferenceSerial(const std::vector<unsigned char> &frameA, const std::vector<unsigned char> &frameB, std::vector<unsigned char> &difference)
{
    for (size_t i = 0; i < frameA.size(); ++i)
    {
        difference[i] = std::abs(frameB[i] - frameA[i]);
    }
}

void frameDifferenceSIMD(const std::vector<unsigned char> &frameA, const std::vector<unsigned char> &frameB, std::vector<unsigned char> &difference)
{
    size_t frameSize = frameA.size();
    size_t numChunks = frameSize / 32; // AVX registers hold 32 bytes (256 bits)

    __m256i *a = (__m256i *)frameA.data();
    __m256i *b = (__m256i *)frameB.data();
    __m256i *result = (__m256i *)difference.data();

    for (size_t i = 0; i < numChunks; ++i)
    {
        __m256i avxFrameA = _mm256_loadu_si256(a);
        __m256i avxFrameB = _mm256_loadu_si256(b);
        __m256i avxDiff = _mm256_abs_epi8(_mm256_sub_epi8(avxFrameB, avxFrameA));
        _mm256_storeu_si256(result, avxDiff);
        a++;
        b++;
        result++;
    }

    for (size_t i = numChunks * 32; i < frameSize; ++i)
    {
        difference[i] = std::abs(frameB[i] - frameA[i]);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " <input_image1.png> <input_image2.png>" << std::endl;
        return 1;
    }

    std::vector<unsigned char> frameA, frameB;
    unsigned width, height;

    if (lodepng::decode(frameA, width, height, argv[1]) != 0)
    {
        std::cerr << "Error loading image 1." << std::endl;
        return 1;
    }

    if (lodepng::decode(frameB, width, height, argv[2]) != 0)
    {
        std::cerr << "Error loading image 2." << std::endl;
        return 1;
    }

    std::vector<unsigned char> differenceSerial(frameA.size());
    std::vector<unsigned char> differenceSIMD(frameA.size());

    auto startSerial = std::chrono::high_resolution_clock::now();
    frameDifferenceSerial(frameA, frameB, differenceSerial);
    auto endSerial = std::chrono::high_resolution_clock::now();

    auto startSIMD = std::chrono::high_resolution_clock::now();
    frameDifferenceSIMD(frameA, frameB, differenceSIMD);
    auto endSIMD = std::chrono::high_resolution_clock::now();

    // Compare results (omitted for brevity)

    // Calculate and print acceleration factor
    auto elapsedTimeSerial = std::chrono::duration_cast<std::chrono::microseconds>(endSerial - startSerial).count();
    auto elapsedTimeSIMD = std::chrono::duration_cast<std::chrono::microseconds>(endSIMD - startSIMD).count();

    std::cout << "Time taken (Serial): " << elapsedTimeSerial << " microseconds" << std::endl;
    std::cout << "Time taken (SIMD): " << elapsedTimeSIMD << " microseconds" << std::endl;

    double accelerationFactor = static_cast<double>(elapsedTimeSerial) / elapsedTimeSIMD;
    std::cout << "Acceleration Factor: " << accelerationFactor << std::endl;

    // Save the difference image as a PNG (omitted for brevity)

    return 0;
}
