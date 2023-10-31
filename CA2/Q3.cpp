#include <iostream>
#include <immintrin.h>
#include <chrono>

using namespace std;

void frameDifferenceSerial(const unsigned char *frameA, const unsigned char *frameB, unsigned char *difference, int frameSize)
{
    for (int i = 0; i < frameSize; ++i)
    {
        difference[i] = abs(frameB[i] - frameA[i]);
    }
}

void frameDifferenceSIMD(const unsigned char *frameA, const unsigned char *frameB, unsigned char *difference, int frameSize)
{
    int numChunks = frameSize / 32;
    int remainder = frameSize % 32;

    __m256i *a = (__m256i *)frameA;
    __m256i *b = (__m256i *)frameB;
    __m256i *result = (__m256i *)difference;

    for (int i = 0; i < numChunks; ++i)
    {
        __m256i avxFrameA = _mm256_loadu_si256(a);
        __m256i avxFrameB = _mm256_loadu_si256(b);
        __m256i avxDiff = _mm256_abs_epi8(_mm256_sub_epi8(avxFrameB, avxFrameA));
        _mm256_storeu_si256(result, avxDiff);
        a++;
        b++;
        result++;
    }

    for (int i = 0; i < remainder; ++i)
    {
        difference[numChunks * 32 + i] = abs(frameB[numChunks * 32 + i] - frameA[numChunks * 32 + i]);
    }
}

int main()
{
    const int frameSize = 3840 * 2160; // 4K
    unsigned char *frameA = new unsigned char[frameSize];
    unsigned char *frameB = new unsigned char[frameSize];
    unsigned char *differenceSerial = new unsigned char[frameSize];
    unsigned char *differenceSIMD = new unsigned char[frameSize];

    for (int i = 0; i < frameSize; ++i)
    {
        frameA[i] = i % 256;
        frameB[i] = (i + 1) % 256;
    }

    auto startSerial = chrono::high_resolution_clock::now();
    frameDifferenceSerial(frameA, frameB, differenceSerial, frameSize);
    auto endSerial = chrono::high_resolution_clock::now();

    auto startSIMD = chrono::high_resolution_clock::now();
    frameDifferenceSIMD(frameA, frameB, differenceSIMD, frameSize);
    auto endSIMD = chrono::high_resolution_clock::now();

    auto timeSerial = chrono::duration_cast<chrono::microseconds>(endSerial - startSerial).count();
    auto timeSIMD = chrono::duration_cast<chrono::microseconds>(endSIMD - startSIMD).count();

    cout << "Time (Serial): " << timeSerial << " microseconds" << endl;
    cout << "Time (SIMD): " << timeSIMD << " microseconds" << endl;

    double speedUp = static_cast<double>(timeSIMD) / timeSerial;
    cout << "Acceleration " << 1 / speedUp << endl;

    delete[] frameA;
    delete[] frameB;
    delete[] differenceSerial;
    delete[] differenceSIMD;

    return 0;
}
