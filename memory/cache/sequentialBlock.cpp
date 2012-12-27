#include <cstdio>
#include <cstring>
#include <pthread.h>
#include <cstdlib>
#include <sys/time.h>
#include <algorithm>

using namespace std;

const int Length = 1024 * 1024 * 1024 / 4;  	//	1G
const int InnerLoop = 20;
int blockSize;

void* workerThread(void* array) {
	float* a = (float*)(array);
	for (int offset = 0; offset < Length; offset += blockSize) {
		float* b = a + offset;
		//	Work on the block
		int block = min(blockSize, Length - offset);
		for (int i = 0; i < InnerLoop; i++) {
			for (int j = 0; j < block; j++)
				b[j] = b[j] * 2 + j;
			float sum = 0.0;
			for (int j = 0; j < block; j++)
				sum += b[j];
			sum /= blockSize;
			for (int j = 0; j < block; j++)
				b[j] -= sum;
		}
	}
}

int main(int argc, char** argv) {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s [# Threads] [BlockSize(KB)]\n", argv[0]);
		return 1;
	}
	int numThread = atoi(argv[1]);
	blockSize = atoi(argv[2]) * 1024 / 4;
	//fprintf(stderr, "blockSize %d\n", blockSize);

	fprintf(stdout, "# Threads: %d, BlockSize %d KB !\n", numThread, atoi(argv[2]));
	float* s[100];
	for (int i = 0; i < numThread; i++) {
		s[i] = new float[Length];
		memset(s[i], 0, sizeof(float) * Length);
	}

    timeval start, end;
    gettimeofday(&start, NULL);

	pthread_t p[100];
	for (int i = 0; i < numThread; i++) 
		pthread_create(p + i, NULL, workerThread, (void*)(s[i]));
	for (int i = 0; i < numThread; i++)
		pthread_join(p[i], NULL);

    gettimeofday(&end, NULL);
    
	double elapsed = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    elapsed /= 1000000;
    printf("%.2f sec\n", elapsed);

	for (int i = 0; i < numThread; i++)
		delete [] s[i];

	return 0;
}

