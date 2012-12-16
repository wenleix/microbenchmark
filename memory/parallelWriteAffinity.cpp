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
	if (argc < 2) {
		fprintf(stderr, "Usage: %s [# Threads] [Affinity]\n", argv[0]);
		return 1;
	}
	int numThread = atoi(argv[1]);
	fprintf(stdout, "# Threads: %d !\n", numThread);
	if (argc != 2 + numThread) {
		fprintf(stderr, "Affinity list not enough!\n");
		return 1;
	}

	int affinity[100];
	int* s[100];
	fprintf(stdout, "Affinity: ");
	for (int i = 0; i < numThread; i++) {
		s[i] = new int[Length];
		memset(s[i], 0, sizeof(int) * Length);
		affinity[i] = atoi(argv[2 + i]);
		fprintf(stdout, "%d ", affinity[i]);
	}
	fprintf(stdout, "\n");

    timeval start, end;
    gettimeofday(&start, NULL);

	pthread_t p[100];
	for (int i = 0; i < numThread; i++) {
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		cpu_set_t cpu_set;
		CPU_ZERO(&cpu_set);

		CPU_SET(affinity[i], &cpu_set);
		if (pthread_attr_setaffinity_np(&attr, sizeof(cpu_set), &cpu_set) != 0) {
			fprintf(stderr, "Set Affinity Failed!\n");
			return 1;
		}
		pthread_create(p + i, &attr, workerThread, (void*)(s[i]));
	}
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

