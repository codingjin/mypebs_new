#define _GNU_SOURCE
#include "pebs.h"

// DEFINITION
CPU_TID cputid[TIDNUM];
__u64 event1, event2;

// exclusive cpus for PEBS threads
int samplecpu_map[TIDNUM] = {86, 87};

struct perf_event_mmap_page *perf_page[TIDNUM][NPBUFTYPES];
int pfd[TIDNUM][NPBUFTYPES];
char filenames[TIDNUM][64];
FILE *fps[TIDNUM];
pthread_t sample_threads[TIDNUM];

void init()
{
	// cputid should be already initialized inside main() ahead
	unsigned long ts = time(NULL);
	for (int i=0;i<TIDNUM;++i) {
		for (int j=0;j<NPBUFTYPES;++j) {
			perf_page[i][j] = NULL;
			pfd[i][j] = -1;
		}

		snprintf(filenames[i], sizeof(filenames[i]), "profiling_%lu_%d",
				ts, i);
		fps[i] = fopen(filenames[i], "w");
		if (!fps[i]) {
			fprintf(stderr, "fopen file[%s] error!\n", filenames[i]);
			assert(fps[i] != NULL);
		}
	}

	int ret = pfm_initialize();
	if (ret != PFM_SUCCESS) {
		fprintf(stderr, "Cannot initialize library: %s\n", 
			pfm_strerror(ret));
		assert(ret == PFM_SUCCESS);
	}

	event1 = _get_read_attr();
	event2 = _get_write_attr();
	perf_setup();

}

void perf_setup()
{
	// arrt1 - READ; attr2 - WRITE
	struct perf_event_attr attr1, attr2;
	for (int i=0;i<TIDNUM;++i) {

		memset(&attr1, 0, sizeof(struct perf_event_attr));
		memset(&attr2, 0, sizeof(struct perf_event_attr));

		attr1.type = PERF_TYPE_RAW;
		attr2.type = PERF_TYPE_RAW;

		attr1.size = sizeof(struct perf_event_attr);
		attr2.size = sizeof(struct perf_event_attr);

		attr1.config = event1;
		attr1.config1 = 0;
		attr2.config = event2;
		attr2.config1 = 0;

		attr1.sample_period = SAMPLE_PERIOD;
		attr2.sample_period = SAMPLE_PERIOD;

		attr1.sample_type = PERF_SAMPLE_IP | PERF_SAMPLE_TID | PERF_SAMPLE_ADDR | PERF_SAMPLE_TIME;
		attr2.sample_type = PERF_SAMPLE_IP | PERF_SAMPLE_TID | PERF_SAMPLE_ADDR | PERF_SAMPLE_TIME;

		attr1.disabled = 0;
		attr2.disabled = 0;

		attr1.exclude_kernel = 1;
		attr2.exclude_kernel = 1;

		attr1.exclude_hv = 1;
		attr2.exclude_hv = 1;
		
		attr1.exclude_idle = 1;
		attr2.exclude_idle = 1;

		attr1.exclude_callchain_kernel = 1;
		attr2.exclude_callchain_kernel = 1;

		attr1.exclude_callchain_user = 1;
		attr2.exclude_callchain_user = 1;

		attr1.precise_ip = 1;
		attr2.precise_ip = 1;

		// READ
		pfd[i][READ] = _perf_event_open(&attr1, cputid[i].tid/*-1*/, cputid[i].cpuid, -1, 0);
		if (pfd[i][READ] == -1) {
			if (errno == ESRCH) {
				//fprintf(stderr, "No such process(nid=%d)\n", cputid[i].tid);
				assert(pfd[i][READ] != -1);
			}
			assert(pfd[i][READ] != -1);
		}
		perf_page[i][READ] = _get_perf_page(pfd[i][READ]);
		assert(perf_page[i][READ] != NULL);

		// WRITE
		pfd[i][WRITE] = _perf_event_open(&attr2, cputid[i].tid/*-1*/, cputid[i].cpuid, -1, 0);
		if (pfd[i][WRITE] == -1) {
			if (errno == ESRCH) {
				//fprintf(stderr, "No such process(tid=%d)\n", cputid[i].tid);
				assert(pfd[i][WRITE] != -1);
			}
			assert(pfd[i][WRITE] != -1);
		}
		perf_page[i][WRITE] = _get_perf_page(pfd[i][WRITE]);
		assert(perf_page[i][WRITE] != NULL);

	} // end of setup events for each TID

}

void *sample_thread(void *nump)
{
	int index = *(int*)nump;
	assert(index>=0 && index<TIDNUM);

	int cancel_state = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	if (cancel_state) {
		fprintf(stderr, "thread cancel_state setting failed!\n");
	}
	assert(cancel_state == 0);

	int cancel_type = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	if (cancel_type) {
		fprintf(stderr, "thread cancel_type setting failed!\n");
	}
	assert(cancel_type == 0);

	// set affinity
	int cpuid = samplecpu_map[index];
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(cpuid, &cpuset);

	pthread_t thread = pthread_self();

	int affinity_ret = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
	if (affinity_ret)	fprintf(stderr, "pthread_setaffinity_np[%d] failed!\n", index);
	assert(affinity_ret==0);

	while (true) {
	
		for (int i=0;i<NPBUFTYPES;++i) {
			struct perf_event_mmap_page *p = perf_page[index][i];
			char *pbuf = (char *)p + p->data_offset;
			__sync_synchronize();

			if (p->data_head == p->data_tail)	continue;

			struct perf_event_header *ph = 
				reinterpret_cast<struct perf_event_header*>(pbuf + (p->data_tail % p->data_size));
			assert(ph != NULL);
			struct perf_sample *ps;
			switch (ph->type) {
				case PERF_RECORD_SAMPLE:
					ps = (struct perf_sample*)ph; assert(ps != NULL);
					if (ps->addr && ps->time && cputid[index].tid==ps->tid) {
						fprintf(fps[index], "%llu %llu\n", ps->addr >> 12, ps->time);
						//fprintf(fps[index], "buftype=%d %llu %llu\n", i, ps->addr >> 12, ps->time);
						/*fprintf(fps[index], "pid=%u tid=%u %llu %llu\n",
							ps->pid, ps->tid, ps->addr>>12, ps->time);*/
					}
					break;

				case PERF_RECORD_THROTTLE:
				case PERF_RECORD_UNTHROTTLE:
					break;
				default:
					fprintf(stderr, "Unknown perf_sample type %u\n", ph->type);
					break;
			} // got the perf sample
			p->data_tail += ph->size;
		} // end of loop NPBUFTYPES

	} // Repeated Sampling
	return NULL;
}

long _perf_event_open(struct perf_event_attr *hw_event, pid_t pid, int cpu, int group_fd, unsigned long flags)
{
	int ret = syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
	return ret;
}

__u64 _get_read_attr()
{
	struct perf_event_attr attr;
	memset(&attr, 0, sizeof(attr));
	int ret = pfm_get_perf_event_encoding("MEM_LOAD_RETIRED.L3_MISS", 
										PFM_PLMH, &attr, NULL, NULL);
	if (ret != PFM_SUCCESS) {
		fprintf(stderr, "Cannot get encoding %s\n", pfm_strerror(ret));
		assert(ret == PFM_SUCCESS);
	}

	return attr.config;
}

__u64 _get_write_attr()
{
	struct perf_event_attr attr;
	memset(&attr, 0, sizeof(attr));
	int ret = pfm_get_perf_event_encoding("MEM_INST_RETIRED.ALL_STORES",
										PFM_PLMH, &attr, NULL, NULL);
	if (ret != PFM_SUCCESS) {
		fprintf(stderr, "Cannot get encoding %s\n", pfm_strerror(ret));
		assert(ret == PFM_SUCCESS);
	}

	return attr.config;
}

struct perf_event_mmap_page *_get_perf_page(int pfd)
{
	size_t mmap_size = sysconf(_SC_PAGESIZE) * PERF_PAGES;
	struct perf_event_mmap_page *p =
		reinterpret_cast<struct perf_event_mmap_page *>(mmap(NULL, mmap_size, PROT_READ | PROT_WRITE, 
																MAP_SHARED, pfd, 0));

	if (p == MAP_FAILED) {
		fprintf(stderr, "mmap for pfd(%d) failed!\n", pfd);
		assert(p != MAP_FAILED);
	}
	return p;
}

void INThandler(int sig)
{
	signal(sig, SIG_IGN);

	for (int i=0;i<TIDNUM;++i) {
		int ret_cancel = pthread_cancel(sample_threads[i]);
		if (ret_cancel)
			fprintf(stderr, "pthread_cancel failed!\n");
		assert(ret_cancel==0);
	}
	
	// Do cleaning
	for (int i=0;i<TIDNUM;++i) {
		for (int j=0;j<NPBUFTYPES;++j) {
			if (perf_page[i][j]) {
				munmap(perf_page[i][j], sysconf(_SC_PAGESIZE)*PERF_PAGES);
				perf_page[i][j] = NULL;
			}
			if (pfd[i][j] != -1) {
				ioctl(pfd[i][j], PERF_EVENT_IOC_DISABLE, 0);
				close(pfd[i][j]);
				pfd[i][j] = -1;
			}
		}
		fclose(fps[i]);
		fps[i] = NULL;
	}
}


