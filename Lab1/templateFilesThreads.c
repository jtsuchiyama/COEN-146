// COEN 146L : Lab 1 - template to use for creating multiple thread to make file transfers (step 4)

// Name: Jake Tsuchiyama
// Date: 1/5/22
// Title: Lab 1 - Copying Multiple Files w/ Threads
// Description: This program copies 10 files by creating threads and copying 
// one file on each thread. 

#include <stdio.h>   // fprintf(), fread(), fwrite(), fopen(), fclose()
#include <stdlib.h>  // malloc(), free()
#include <pthread.h> // pthread_create()
#include <unistd.h>  // read(), write()
#include <fcntl.h>   // open(), close()
#include <errno.h>   // errno
#include <time.h>    // clock()

#define BUF_SIZE 2048 //buffer size

// data structure to hold copy
struct copy_struct {
	int thread_id; 
	char* src_filename;
	char* dst_filename; 
};

// copies a files from src_filename to dst_filename using functions fopen(), fread(), fwrite(), fclose()
int func_copy(char* src_filename, char* dst_filename) {
	
	FILE* src = fopen(src_filename, "r");	// opens a file for reading
	if (src == NULL) { // fopen() error checking
		fprintf(stderr, "unable to open %s for reading: %i\n", src_filename, errno);
		exit(0);
	}

	FILE* dst = fopen(dst_filename, "w");	// opens a file for writing; erases old file/creates a new file
	if (dst == NULL) { // fopen() error checking
		fprintf(stderr, "unable to open/create %s for writing: %i\n", dst_filename, errno);
		exit(0);
	}

	void* buf = malloc((size_t)BUF_SIZE);  // allocate a buffer to store read data

	// reads and writes content of file is loop iterations until end of file
	while(fread(buf, 1, 1, src) == 1)
	{
		fwrite(buf, 1, 1, dst);
	}	
	
	
	// closes src file pointer
	fclose(src);	

	// closes dst file pointer
	fclose(dst);	

	// frees memory used for buf
	free(buf);
	
	return 0;
}

// thread function to copy one file
void* copy_thread(void* arg) {
	struct copy_struct params = *(struct copy_struct*)arg;  // cast/dereference void* to copy_struct
	printf("thread[%i] - copying %s to %s\n", params.thread_id, params.src_filename, params.dst_filename);

	//call file copy function
	func_copy(params.src_filename, params.dst_filename);	

	pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
	// check correct usage of arguments in command line
	if (argc != 21) 
	{
		fprintf(stderr, "usage: %s <src1_filename> ... <src9_filename> <dst1_filename> ... <dst9_filename>", argv[0]);
	}
	
	char* src_filename;
	char* dst_filename;
	char* src_filenames[10]; // array of 10 source files
	char* dst_filenames[10]; // array of 10 destination files
	int num_threads = 10; // number of threads to create

	int i;

	// Loading the source and destination file names from the arguments into the arrays
	for(i=1;i < argc; i++)
	{
		if(i < (num_threads + 1))
			src_filenames[i-1] = argv[i];

		else
			dst_filenames[i-1-num_threads] = argv[i];	
	}

	pthread_t threads[num_threads]; //initialize threads
	struct copy_struct thread_params[num_threads]; // structure for each thread
	for (i = 0; i < num_threads; i++) {
		// initialize thread parameters
		struct copy_struct param = {i, src_filenames[i], dst_filenames[i]};
		thread_params[i] = param;	

		// create each copy thread
		pthread_t thread;
		threads[i] = thread;		

		// use pthread_create(.....);
		pthread_create(&threads[i], NULL, copy_thread, (void*) &thread_params[i]);
	}

	// wait for all threads to finish
	for (i = 0; i < num_threads; i++) {
		pthread_join(threads[i], NULL);
	}

	pthread_exit(NULL);
}
