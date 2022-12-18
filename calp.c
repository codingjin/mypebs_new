#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAXLEN 64
char rawfilename[64];
char uniqfilename[64];
FILE *fpr;
FILE *fpu;

#define BUCKETSIZE 1027

typedef struct Urecord {
	unsigned long long pn;
	struct Urecord *next;
	bool mark;
	bool singleflag;
}Urecord;

Urecord buckets[BUCKETSIZE];

unsigned int hashfunc(unsigned long long key);
void display_buckets();
void clear_marks();
void free_buckets();
bool is_single(unsigned long long key);
bool is_mark(unsigned long long key);
void clear_mark(unsigned long long key);

unsigned long long frequency, pageno, linecounter=0;

typedef struct Record {
	unsigned long long pn;
	unsigned long long reuse_distance;
	bool firstflag;
	bool singleflag;
}Record;


int main(int argc, char **argv) {
	
	if (argc != 3) {
		fprintf(stderr, "Incorrect Command!\n");
		printf("Usage: ./calp rawfilename(raw) uniqfilename(uniq)\n");
	}
	assert(argc==3);

	strcpy(rawfilename, argv[1]);
	strcpy(uniqfilename, argv[2]);

	fpr = fopen(rawfilename, "r");
	if (!fpr)
		fprintf(stderr, "Failed to fopen the file[%s]\n", rawfilename);
	assert(fpr != NULL);

	fpu = fopen(uniqfilename, "r");
	if (!fpu)
		fprintf(stderr, "Failed to fopen the file[%s]\n", uniqfilename);
	assert(fpu != NULL);

	char *linebuffer = (char*)malloc(sizeof(char) * MAXLEN);
	if (!linebuffer)
		fprintf(stderr, "Failed to malloc memory for linebuffer\n");
	assert(linebuffer != NULL);

	size_t len = 0;
	ssize_t readlen;
	//Build HashTable based on the uniqfile
	// HashTable Initialization
	for (int i=0;i<BUCKETSIZE;++i) {
		buckets[i].pn = 0;
		buckets[i].next = NULL;
		buckets[i].mark = false;
		buckets[i].singleflag = false;
	}

	while ((readlen=getline(&linebuffer, &len, fpu)) != -1) {
		linebuffer[readlen-1] = 0;
		char *numend;
		frequency = strtoull(linebuffer, &numend, 10); assert(frequency>=1);
		linecounter += frequency;
		pageno = strtoull(numend, NULL, 10);
		unsigned int bucketid = hashfunc(pageno);
		Urecord *ur = (Urecord*)malloc(sizeof(Urecord));
		assert(ur != NULL);
		ur->pn = pageno;
		ur->next = NULL;
		ur->mark = false;
		if (frequency > 1)	ur->singleflag=false;
		else				ur->singleflag=true;

		if (buckets[bucketid].next == NULL)
			buckets[bucketid].next = ur;
		else {
			Urecord *iterator = buckets[bucketid].next;
			assert(iterator != NULL);
			while (iterator->next)	iterator=iterator->next;
			iterator->next = ur;
		}
	}
	int ret_fp = fclose(fpu);
	if (ret_fp)	fprintf(stderr, "fclose failed\n");
	assert(ret_fp==0);
	fpu = NULL;
	//display_buckets();
	//printf("lineno=%d\n", linecounter);

	// begin the reuse-distance calculation, based on distinct pages number
	// 1 readin the records from the rawfile
	Record *records = (Record*)malloc(sizeof(Record) * linecounter);
	assert(records!=NULL);
	unsigned long long index = 0;
	while ((readlen=getline(&linebuffer, &len, fpr)) != -1) {
		linebuffer[readlen-1] = 0;
		pageno = strtoull(linebuffer, NULL, 10);
		records[index].pn = pageno;
		records[index].reuse_distance = 0;
		records[index].singleflag = is_single(pageno);
		if (records[index].singleflag)
			records[index].firstflag = true;
		else
			records[index].firstflag = !is_mark(pageno);
		
		++index;
		/*
		if (index == 3147) {
			for (int i=0;i<index;++i) {
				printf("index[%d] pn[%llu] reuse_distance[%llu] single[%d] first[%d]\n",
						i, records[i].pn, records[i].reuse_distance, records[i].singleflag, records[i].firstflag);
			}
			return 0;
		}
		*/
	}
	ret_fp = fclose(fpr);
	if (ret_fp) fprintf(stderr, "fclose failed!\n");
	assert(ret_fp==0);
	fpr = NULL;

	// 2 calculate the reuse_distance
	unsigned long long reuse_counter = 0;
	unsigned long long total_reuse_distance = 0;
	for (unsigned long long i=linecounter-1; i>0;--i) {
		if (!records[i].firstflag) {
			++reuse_counter;
			if (records[i].pn != records[i-1].pn) {
				assert(i!=1); // if i==1, then records[1] is the first, contradict!
				if (i==2) {
					assert(records[i].pn==records[0].pn);
					++total_reuse_distance;
					continue;
				}
				// j>0 at the begining
				unsigned long long j = i-1; 
				unsigned long long distinct_counter = 0;
				while (j>0) {
					if (records[j].pn != records[i].pn) {
						if (!is_mark(records[j].pn)) {
							++distinct_counter;
						}
					}else { // records[j].pn==records[i].pn
						total_reuse_distance += distinct_counter;
						distinct_counter = 0;
						unsigned long long k = j+1;
						while (k<i) {
							clear_mark(records[k].pn);
							++k;
						}
						break;
					}
					--j;
				}
				if (j==0)	assert(records[i].pn == records[0].pn);
				total_reuse_distance += distinct_counter;
				distinct_counter = 0;
				for (unsigned long long k=1;k<i;++k) {
					clear_mark(records[k].pn);
				}
			} // end of if records[i].pn != records[i-1].pn
		} // end of if !records[i].firstflag
	}



	printf("reuse_counter=%llu total_reuse_distance=%llu\n", reuse_counter, total_reuse_distance);
	float ave_distance = (float)total_reuse_distance / (float)reuse_counter;
	printf("ave_reuse_distance=%.2lf\n", ave_distance);
	//printf("linecounter=%llu\n", linecounter); //display_buckets();
	free(records);
	records = NULL;
	free(linebuffer);
	linebuffer = NULL;
	free_buckets();


	return 0;
}

void clear_mark(unsigned long long key)
{
	unsigned int bucketid = hashfunc(key);
	assert(bucketid < BUCKETSIZE);
	Urecord *iterator = buckets[bucketid].next;
	assert(iterator != NULL);
	while (iterator) {
		if (iterator->pn == key) {
			iterator->mark=false;
			return;
		}
		iterator = iterator->next;
	}
	assert(NULL);
}

bool is_single(unsigned long long key)
{
	unsigned int bucketid = hashfunc(key);
	assert(bucketid < BUCKETSIZE);
	Urecord *iterator = buckets[bucketid].next;
	assert(iterator != NULL);
	while (iterator) {
		if (key == iterator->pn)	return iterator->singleflag;
		iterator = iterator->next;
	}
	assert(NULL);
}

bool is_mark(unsigned long long key)
{
	unsigned int bucketid = hashfunc(key);
	assert(bucketid < BUCKETSIZE);
	Urecord *iterator = buckets[bucketid].next;
	assert(iterator != NULL);
	while (iterator) {
		if (key == iterator->pn) {
			bool mark = iterator->mark;
			if (!mark)	iterator->mark=true;
			return mark;
		}
		iterator = iterator->next;
	}
	assert(NULL);
}

void clear_marks()
{
	for (int i=0;i<BUCKETSIZE;++i) {
		if (buckets[i].next) {
			Urecord *iterator = buckets[i].next;
			while (iterator) {
				iterator->mark = false;
				iterator = iterator->next;
			}
		}
	}
}

void free_buckets()
{
	for (int i=0;i<BUCKETSIZE;++i) {
		if (buckets[i].next) {
			if ((buckets[i].next)->next) {
				Urecord *firsti = buckets[i].next;
				Urecord *secondi = firsti->next;
				while (secondi->next) {
					secondi = secondi->next;
					free(firsti->next);
					firsti->next = secondi;
				}
				free(secondi);
				secondi = NULL;
				free(firsti);
				firsti = NULL;
			}else {
				free(buckets[i].next);
				buckets[i].next = NULL;
			}
		}
	}
}

void display_buckets()
{
	unsigned long long counter = 0;
	for (int i=0;i<BUCKETSIZE;++i) {
		if (buckets[i].next) {
			Urecord *iterator = buckets[i].next;
			assert(iterator!=NULL);
			while (iterator) {
				++counter;
				printf("counter[%llu] pageno[%llu] single[%d]\n", counter, iterator->pn, iterator->singleflag);
				iterator = iterator->next;
			}
		}
	}
}

unsigned int hashfunc(unsigned long long key)
{
	unsigned int value = (unsigned int)(key % BUCKETSIZE);
	assert(value>=0 && value<BUCKETSIZE);
	return value;
}

