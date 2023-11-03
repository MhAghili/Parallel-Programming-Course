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

int main(int argc, char *argv[])
{

    vector<unsigned char> image1, image2;
    unsigned width, height;

    if (lodepng::decode(image1, width, height, argv[1]) != 0)
    {
        cerr << "Error loading image 1." << endl;
        return 1;
    }

    if (lodepng::decode(image2, width, height, argv[2]) != 0)
    {
        cerr << "Error loading image 2." << endl;
        return 1;
    }

    vector<unsigned char> resultSerial(image1.size());

    float alpha = 0.25;

    auto startSerial = chrono::high_resolution_clock::now();
    addImagesWithTransparencySerial(image1, image2, alpha, resultSerial);
    auto endSerial = chrono::high_resolution_clock::now();

    auto timeSerial = chrono::duration_cast<chrono::microseconds>(endSerial - startSerial).count();

    cout << "Time : " << timeSerial << " microseconds" << endl;

    // outputing result:
    lodepng::encode("result.png", resultSerial, width, height);

    return 0;
}
