#ifndef PEBS_H
#define PEBS_H

#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>
#include <asm/unistd.h>
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <sys/mman.h>
#include <sched.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <cstring>
#include <perfmon/pfmlib.h>
#include <perfmon/pfmlib_perf_event.h>
#include <err.h>
#include <signal.h>
#include <cerrno>
#include <cstddef>

#define PERF_PAGES 65
#define SAMPLE_PERIOD 10
#define TIDNUM 2

struct perf_sample {
	struct perf_event_header header;
	__u64 ip; /* if PERF_SAMPLE_IP */
	__u32 pid, tid; /* if PERF_SAMPLE_TID */
	__u64 time; /* if PERF_SAMPLE_TIME */
	__u64 addr; /* if PERF_SAMPLE_ADDR */
	//__u64 phys_addr; /* if PERF_SAMPLE_PHYS_ADDR */
};

enum pbuftype {
	READ = 0,
	WRITE = 1,
	NPBUFTYPES
};

typedef struct CPU_TID {
	int cpuid;
	int tid;
}CPU_TID;
extern CPU_TID cputid[TIDNUM];

extern void *sample_thread(void*);
extern pthread_t sample_threads[TIDNUM];
extern int samplecpu_map[TIDNUM];

extern __u64 event1, event2;
extern struct perf_event_mmap_page *perf_page[TIDNUM][NPBUFTYPES];
extern int pfd[TIDNUM][NPBUFTYPES];

extern char filenames[TIDNUM][64];
extern FILE *fps[TIDNUM];

extern void INThandler(int);
extern long _perf_event_open(struct perf_event_attr *hw_event, pid_t pid, int cpu, int group_fd, unsigned long flags);
extern __u64 _get_read_attr();
extern __u64 _get_write_attr();
extern struct perf_event_mmap_page *_get_perf_page(int pfd);

extern void init();
extern void perf_setup();


#endif
