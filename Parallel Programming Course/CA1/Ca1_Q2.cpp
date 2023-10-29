#include <emmintrin.h>
#include <stdio.h>
#include <xmmintrin.h>
void print_int_vector(__m128i a, unsigned char type)
{
    int i;
    switch (type)
    {
    case 'u8':
        for (i = 0; i < 16; i++)
        {
            printf("%u ", _mm_extract_epi8(a, i));
        }
        break;
    case 'i8':
        for (i = 0; i < 16; i++)
        {
            printf("%d ", _mm_extract_epi8(a, i));
        }
        break;
    case 'u16':
        for (i = 0; i < 8; i++)
        {
            printf("%u ", _mm_extract_epi16(a, i));
        }
        break;
    case 'i16':
        for (i = 0; i < 8; i++)
        {
            printf("%d ", _mm_extract_epi16(a, i));
        }
        break;
    case 'u32':
        for (i = 0; i < 4; i++)
        {
            printf("%u ", _mm_extract_epi32(a, i));
        }
        break;
    case 'i32':
        for (i = 0; i < 4; i++)
        {
            printf("%d ", _mm_extract_epi32(a, i));
        }
        break;
    case 'u64':
        for (i = 0; i < 2; i++)
        {
            printf("%llu ", (unsigned long long)_mm_extract_epi64(a, i));
        }
        break;
    case 'i64':
        for (i = 0; i < 2; i++)
        {
            printf("%lld ", (long long)_mm_extract_epi64(a, i));
        }
        break;
    default:
        printf("Unsupported type\n");
    }
    printf("\n");
}

void print_spfp_vector(__m128 a)
{
    int i;
    float temp[4];

    _mm_storeu_ps(temp, a);

    for (i = 0; i < 4; i++)
    {
        printf("%f ", temp[i]);
    }

    printf("\n");
}