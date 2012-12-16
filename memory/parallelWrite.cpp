#include <cstdio>
#include <cstring>
#include <pthread.h>
#include <cstdlib>
#include <sys/time.h>

using namespace std;

const int Length = 1024 * 1024 * 1024 / 4;  	//	1G

void* workerThread(void* array) {
	int* a = (int*)(array);
	for (int i = 0; i < Length; i++)
		a[i] = i;
}

int main(int argc, char** argv) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s [# Threads]\n", argv[0]);
		return 1;
	}
	int numThread = atoi(argv[1]);
	fprintf(stdout, "# Threads: %d !\n", numThread);
	int* s[100];
	for (int i = 0; i < numThread; i++) {
		s[i] = new int[Length];
		memset(s[i], 0, sizeof(int) * Length);
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
    printf("%.4f MB/s\n", Length * sizeof(int) * numThread / 1024 / 1024 / elapsed);

	for (int i = 0; i < numThread; i++)
		delete [] s[i];

	return 0;
}

