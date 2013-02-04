#include <cstdio>
#include <cstring>
#include <sys/time.h>

using namespace std;

void my_matmul(int n, float* __restrict__ C,
			          const float* __restrict__ A,
			          const float* __restrict__ B) {
	for (int k = 0; k < n; ++k) {
		for (int j = 0; j < n; ++j) {
			float bkj = B[k * n + j];
			for (int i = 0; i < n; ++i) {
				C[k * n + j] += A[j * n + i] * bkj;
			}
		}
	}
}

const int N = 1024;
float A[N * N], B[N * N], C[N * N];

int main() {
	for (int i = 0; i < N * N; i++) {
		A[i] = 1.3 + i;
		B[i] = 9.3 * i - 3.7;
		C[i] = 0.0;
	}

		
    timeval start, end;
    gettimeofday(&start, NULL);

	my_matmul(N, C, A, B);

    gettimeofday(&end, NULL);
	double elapsed = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    elapsed /= 1000000;
    printf("%.2f sec\n", elapsed);
	
	double flops = (double)N * N * N * 2;
	printf("%.2f MFlops/s\n", flops / elapsed / 1000 / 1000.0);
	return 0;
}

