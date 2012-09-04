// Test memory throughput.

#include <cstdio>
#include <sys/time.h>

using namespace std;

const int Size = 100000000;
int s[Size];

int main() {
    timeval start, end;
    gettimeofday(&start, NULL);
    for (int iter = 0; iter < 10; iter++) {
        for (int i = 0; i < Size; i++)
            s[i] = 1;
    }
    gettimeofday(&end, NULL);
    double elapsed = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    elapsed /= 1000000;
    printf("%.10f sec\n", elapsed);
    printf("%.2f MB/s\n", Size * sizeof(int) / 1024 / 1024 / elapsed * 10);
    return 0;
}


