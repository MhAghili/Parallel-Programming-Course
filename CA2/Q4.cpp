#include <iostream>
#include <immintrin.h>
#include <chrono>
#include "lodepng-master/lodepng.h"

using namespace std;

void addImagesWithTransparencySerial(const vector<unsigned char> &image1, const vector<unsigned char> &image2, float alpha, vector<unsigned char> &result)
{
    for (size_t i = 0; i < image1.size(); ++i)
    {
        // formole dakhel soorat poroje mesle inke eshtebahe va in dorosteshe:
        result[i] = static_cast<unsigned char>(image1[i] * (1.0 - alpha) + image2[i] * alpha);
    }
}

void addImagesWithTransparencySIMD(const vector<unsigned char> &image1, const vector<unsigned char> &image2, float alpha, vector<unsigned char> &result)
{
    size_t imageSize = image1.size();
    size_t numChunks = imageSize / 32;

    __m256 alphaVec = _mm256_set1_ps(alpha);
    __m256 betaVec = _mm256_set1_ps(1.0 - alpha);

    __m256i *a = (__m256i *)image1.data();
    __m256i *b = (__m256i *)image2.data();
    __m256i *res = (__m256i *)result.data();

    for (size_t i = 0; i < numChunks; ++i)
    {
        __m256i avxImage1 = _mm256_cvtepu8_epi16(_mm256_loadu_si256(a));
        __m256i avxImage2 = _mm256_cvtepu8_epi16(_mm256_loadu_si256(b));

        __m256i avxResult = _mm256_cvttps_epi32(_mm256_fmadd_ps(betaVec, _mm256_cvtepi32_ps(avxImage1), _mm256_fmadd_ps(alphaVec, _mm256_cvtepi32_ps(avxImage2), _mm256_setzero_ps())));

        _mm256_storeu_si256(res, avxResult);

        a++;
        b++;
        res++;
    }

    for (size_t i = numChunks * 32; i < imageSize; ++i)
    {
        result[i] = static_cast<unsigned char>(image1[i] * (1.0 - alpha) + image2[i] * alpha);
    }
}

int main(int argc, char *argv[])
{

    vector<unsigned char> image1, image2;
    unsigned width, height;

    int load1 = lodepng::decode(image1, width, height, argv[1]);

    int load2 = lodepng::decode(image2, width, height, argv[2]);

    if (load1 != 0)
    {
        cerr << "Error loading image 1." << endl;
        return 1;
    }

    if (load2 != 0)
    {
        cerr << "Error loading image 2." << endl;
        return 1;
    }

    vector<unsigned char> resultSerial(image1.size());
    vector<unsigned char> resultSIMD(image1.size());

    float alpha = 0.25;

    auto startSerial = chrono::high_resolution_clock::now();
    addImagesWithTransparencySerial(image1, image2, alpha, resultSerial);
    auto endSerial = chrono::high_resolution_clock::now();

    auto startSIMD = chrono::high_resolution_clock::now();
    addImagesWithTransparencySIMD(image1, image2, alpha, resultSIMD);
    auto endSIMD = chrono::high_resolution_clock::now();

    auto timeSerial = chrono::duration_cast<chrono::microseconds>(endSerial - startSerial).count();
    auto timeSIMD = chrono::duration_cast<chrono::microseconds>(endSIMD - startSIMD).count();

    cout << "Time (Serial): " << timeSerial << " microseconds" << endl;
    cout << "Time (SIMD): " << timeSIMD << " microseconds" << endl;

    double accelerationFactor = static_cast<double>(timeSIMD) / timeSerial;
    cout << "Acceleration Factor: " << accelerationFactor << endl;

    // outputing result:
    lodepng::encode("result.png", resultSIMD, width, height);

    return 0;
}
