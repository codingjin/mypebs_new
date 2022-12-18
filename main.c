#include "pebs.h"
#include <sys/resource.h>

int main(int argc, char **argv) {

	if (argc != TIDNUM+1) {
		fprintf(stderr, "Usage: ./pebs tid0 tid1 ...\n The number of tid is %d\n", TIDNUM);
	}
	assert(argc == TIDNUM+1);

	// exclusive cpus for vCPUs
	//int cpuid_map[TIDNUM] = {35, 38, 82, 85};
	//int cpuid_map[TIDNUM] = {35, 82};
	int cpuid_map[TIDNUM] = {81, 82};
	int tids[TIDNUM];
	for (int i=1;i<=TIDNUM;++i) {
		tids[i-1] = atoi(argv[i]);
		assert(tids[i-1] > 0);
		assert(cpuid_map[i-1] > 0);

		cputid[i-1].cpuid = cpuid_map[i-1];
		cputid[i-1].tid = tids[i-1];
	}

	init();

	signal(SIGINT, INThandler);
	
	//int index_arr[] = {0, 1, 2, 3};
	int index_arr[] = {0, 1};
	// lauch the sampling threads
	for (int i=0;i<TIDNUM;++i) {
		int index = i;
		int ret = pthread_create(&sample_threads[index], NULL, sample_thread, (void*)&index_arr[index]);
		if (ret)
			fprintf(stderr, "pthread_create failed!\n");
		assert(ret == 0);
	}

	// Wait for sampling threads finish
	for (int i=0;i<TIDNUM;++i) {

		void *ret_thread;
		int index = i;
		int join_ret = pthread_join(sample_threads[index], &ret_thread);
		if (join_ret)
			fprintf(stderr, "pthread_join failed!\n");
		assert(join_ret==0);
		if (ret_thread != PTHREAD_CANCELED)
			fprintf(stderr, "pthread_cancel failed!\n");
		assert(ret_thread == PTHREAD_CANCELED);

	}
	
	return 0;
}

