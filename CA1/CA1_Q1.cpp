#include <iostream>
#include <vector>

void cpuid(int code, std::vector<int> &info)
{
    asm volatile("cpuid"
                 : "=a"(info[0]), "=b"(info[1]), "=c"(info[2]), "=d"(info[3])
                 : "a"(code));
}

int main()
{
    std::vector<int> info(4);

    // A - Report processor type, physical and logical cores,
    cpuid(0, info);
    int maxId = info[0];
    char vendor[13];
    *reinterpret_cast<int *>(vendor) = info[1];
    *reinterpret_cast<int *>(vendor + 4) = info[3];
    *reinterpret_cast<int *>(vendor + 8) = info[2];
    vendor[12] = '\0';
    std::cout << "A-" << '\n';
    std::cout << "Processor Model: " << vendor << std::endl;

    cpuid(1, info);

    int cores = (info[1] >> 16) & 0xff;
    int threads = (info[1] >> 24) & 0xff;
    bool ht = threads > 1;

    std::cout << "Physical Cores: " << cores << " Logical Cores: " << threads << std::endl;
    if (ht)
        std::cout << " HyperThreading supported";
    std::cout << "\n------------------\n"
              << std::endl;

    std::cout << "B-" << '\n';
    std::cout << "Cache Information:" << std::endl;
    cpuid(0x2, info); // Query cache IDs
    for (int i = 0; i < 4; ++i)
    {
        int cacheId = (info[i] & 0xff);
        if (cacheId == 0)
            continue; // Cache ID 0 is reserved

        cpuid(4, info); // Query cache information using cache ID
        int cacheType = (info[0] & 0x1f);
        int cacheSizeKB = (info[1] >> 22) + 1;
        std::cout << "Cache " << cacheId << " Type: " << cacheType << " Size: " << cacheSizeKB << " KB" << std::endl;
    }
    std::cout << "\n------------------\n"
              << std::endl;

    // C - Report SIMD architectures supported
    std::cout << "C-" << '\n';
    cpuid(1, info);
    bool sse = info[3] & (1 << 25);
    bool avx = info[2] & (1 << 28);
    bool avx2 = info[2] & (1 << 5);

    std::cout << "SIMD Architectures Supported:" << std::endl;
    if (sse)
        std::cout << "SSE" << std::endl;
    if (avx)
        std::cout << "AVX" << std::endl;
    if (avx2)
        std::cout << "AVX2" << std::endl;

    return 0;
}