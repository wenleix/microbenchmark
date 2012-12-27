#include <cstdio>
#include <cstring>
#include <pthread.h>
#include <cstdlib>
#include <algorithm>
#include <cassert>
#include <sys/time.h>

using namespace std;

//	Cache line is the scheduling unit in random access blocks.
struct CacheLine {
	float data[16];
};

const int Length = 1024 * 1024 * 1024 / sizeof(CacheLine);  	//	1G
const int InnerLoop = 20;
int accessSeq[Length];
int blockSize;

void* workerThread(void* array) {
	CacheLine* a = (CacheLine*)(array);
	for (int offset = 0; offset < Length; offset += blockSize) {
		//	Work on the block
		int block = min(blockSize, Length - offset);
		for (int i = 0; i < InnerLoop; i++) {
			int* seq = accessSeq + offset;
			for (int j = 0; j < block; j++) {
				int idx = *seq;
				float* b = a[idx].data;
				for (int k = 0; k < 16; k++)
					b[k] = b[k] * 2 + k;
				seq++;
			}

			float sum = 0.0;
			seq = accessSeq + offset;	//	Reset seq pointer.
			for (int j = 0; j < block; j++) {
				int idx = *seq;
				float* b = a[idx].data;
				for (int k = 0; k < 16; k++)
					sum += b[k];
				seq++;
			}

			sum /= blockSize * 16;
			seq = accessSeq + offset;	//	Reset seq pointer.
			for (int j = 0; j < block; j++) {
				int idx = *seq;
				float* b = a[idx].data;
				for (int k = 0; k < 16; k++)
					b[k] -= sum;
				seq++;
			}
		}
	}
}

int main(int argc, char** argv) {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s [# Threads] [BlockSize(KB)]\n", argv[0]);
		return 1;
	}
	int numThread = atoi(argv[1]);
	blockSize = atoi(argv[2]) * 1024 / sizeof(CacheLine);
	for (int i = 0; i < Length; i++) 
		accessSeq[i] = i;
	random_shuffle(accessSeq, accessSeq + Length);

	fprintf(stdout, "# Threads: %d, BlockSize %d KB !\n", numThread, atoi(argv[2]));
	CacheLine* s[100];
	for (int i = 0; i < numThread; i++) {
		void* ptr;
		assert(posix_memalign(&ptr, 64, sizeof(CacheLine) * Length) == 0);
		s[i] = (CacheLine*)(ptr);
		assert(size_t(s[i]) % 64 == 0);
		memset(s[i], 0, sizeof(CacheLine) * Length);
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
		free(s[i]);

	return 0;
}

