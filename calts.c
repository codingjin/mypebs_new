#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define MAXLEN 64
char filename[64];
FILE *fp;

int singlecounter = 0;
int linecounter = 0;

typedef struct Record {
	unsigned long long pn;
	unsigned long long timestamp;
	unsigned long long tsd;
	int firstflag;
	int singleflag;
}Record;

int main(int argc, char **argv) {
	
	if (argc != 2) {
		fprintf(stderr, "Incorrect Command!\n");
		printf("Usage: ./calts profile_filename(r)\n");
	}
	assert(argc==2);

	strcpy(filename, argv[1]);
	fp = fopen(filename, "r");
	if (!fp) 
		fprintf(stderr, "Failed to fopen the file[%s]\n", filename);
	assert(fp != NULL);

	char *linebuffer = (char*)malloc(sizeof(char) * MAXLEN);
	if (!linebuffer)
		fprintf(stderr, "Failed to malloc memory to linebuffer!\n");
	assert(linebuffer != NULL);

	size_t len = 0;
	ssize_t readlen;
	while (getline(&linebuffer, &len, fp) != -1)
		++linecounter;

	Record *records = (Record*)malloc(sizeof(Record) * linecounter);
	if (!records)
		fprintf(stderr, "Failed to malloc memory for records!\n");
	assert(records != NULL);

	rewind(fp);
	int index = 0;
	while ((readlen = getline(&linebuffer, &len, fp)) != -1) {
		linebuffer[readlen-1] = 0;
		char *numend;
		records[index].pn = strtoull(linebuffer, &numend, 10);
		records[index].timestamp = strtoull(numend, NULL, 10);
		records[index].tsd = 0;
		records[index].firstflag = 1;
		records[index].singleflag = 1;
		++index;
	}

	int ret_fp = fclose(fp);
	if (ret_fp)	fprintf(stderr, "fclose failed\n");
	assert(ret_fp == 0);
	fp = NULL;
	free(linebuffer);
	linebuffer = NULL;

	// cope with the first record first
	for (int i=1;i<linecounter;++i) {
		if (records[0].pn == records[i].pn)	{
			records[0].singleflag = 0;
			break;
		}
	}
	if (records[0].singleflag == 1)	++singlecounter;

	unsigned long long reuse_distance_sum = 0;
	int reuse_counter = 0;
	int firstcounter = linecounter;
	for (int i=linecounter-1;i>0;--i) {
		for (int j=i-1;j>=0;--j) {
			if (records[i].pn == records[j].pn) {
				records[j].singleflag = 0;

				records[i].singleflag = 0;
				records[i].firstflag = 0;
				records[i].tsd = records[i].timestamp - records[j].timestamp;
				reuse_distance_sum += records[i].tsd;
				++reuse_counter;
				--firstcounter;
				break;
			}
		}

		if (records[i].singleflag == 1)	++singlecounter;
	}
	printf("reuse_distance=%llu\n", reuse_distance_sum/reuse_counter);
	printf("distinct_pages=%d distinct_pages=%f%\n", firstcounter, (float)firstcounter*100/(float)linecounter);
	printf("single_access_pages=%d single_access_pages=%f%\n", singlecounter, (float)singlecounter*100/(float)linecounter);
	// total, single are verified to be correct!
	//printf("total=%d single=%d percentage=%f\n", linecounter, singlecounter, (float)singlecounter/(float)linecounter);
	//printf("reuse_counter=%d reuse_distance_sum=%llu\n", reuse_counter, reuse_distance_sum);
	/* 
	int check_firstcounter=0;
	for (int i=0;i<linecounter;++i) {
		if (records[i].firstflag==1)	++check_firstcounter;
	}
	printf("check_firstcounter=%d firstcounter=%d\n", check_firstcounter, firstcounter);
	*/
	return 0;
}



