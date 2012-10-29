#include <cstdio>
#include <sys/time.h>
#include <cstring>

using namespace std;

#define STRIDE 16

const int Size = 1024 * 1024 * 1024 / 4;    // 1G
float c[Size];

int main() {
    for (int i = 0; i < Size; i++) {
        c[i] = 0;
    }

    float *cend = c + Size;
    float *cptr = c;

    timeval start, end;
    gettimeofday(&start, NULL);

    while (cptr != cend) {
        *cptr = 1.23f;
        cptr += STRIDE;
    }

    gettimeofday(&end, NULL);
    double elapsed = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    elapsed /= 1000000;
    printf("%.2f sec\n", elapsed);
    printf("%.4f MB/s\n", Size * sizeof(float) / 1024 / 1024 / elapsed);
    return 0;
}


