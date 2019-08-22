#define _GNU_SOURCE

void __attribute__ ((constructor (101))) malloc_init(void);
void __attribute__ ((constructor (102))) free_init(void);
void __attribute__ ((destructor (101))) cleanm(void);
void __attribute__ ((destructor (102))) cleanf(void);

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

void* (*original_malloc)(size_t) = NULL;
void (*original_free)(void*) = NULL;

//array and iterator for storing leaks
const int SIZE = 20;
size_t byteArray[20];
int iter = 0;

void cleanm(void) {
	//During this, can send out array of bytes leaked.
	printf("Cleaning Malloc...\n");

	//initilaize counters for checking leaks
	int leakCount = 0;
	unsigned long totalBytes = 0;
	
	int i;

	for (i = 0; i < iter; i++) {

		if(byteArray[i] != 0) {

			fprintf(stderr, "LEAK	%lu\n", byteArray[i]);
			leakCount++;
			totalBytes += byteArray[i];

		}
	
	}

	fprintf(stderr,"TOTAL	%d %lu\n", leakCount, totalBytes);		
   
}

void cleanf(void) {
	//can work with cleanm to send out array of leaked bytes
	//printf("cleaning up free\n");
	//not much to do here tbh, just call the destructor
}

//initializing malloc and free in next functions
void malloc_init(void) {
	
	int i; 

	if (original_malloc == NULL) {
		original_malloc = dlsym(RTLD_NEXT, "malloc");
	}
	
	//zero out the byte array
	for(i = 0; i < SIZE; i++) {
		byteArray[i] = 0;
	}

}

void free_init(void) {

	if (original_free == NULL) {
		original_free = dlsym(RTLD_NEXT, "free");
	}

}

void *malloc(size_t size) {

	size_t *ptr = original_malloc(size);
	//then need to store into an array to keep track of
	//the bytes being stored
	byteArray[iter] = size;
	iter++;

	return ptr;
}

void free(void *ptr) {

	int i;

	//need to check through array and check what is being
	//freed so it can track any leaks. then call free
	for(i = 0; i < iter; i++) {

		if(byteArray[i] == *(size_t *)ptr) {
			//delete byteArray[i], this space has been malloc then free
			byteArray[i] = 0;
		}

	}

	original_free(ptr);

}

