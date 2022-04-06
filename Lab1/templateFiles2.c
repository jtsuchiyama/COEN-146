// COEN 146L : Lab 1 - template to use for file transfer (steps 1, 2, and 3)
// You need to adapt this template according to the requirements of each of the steps

// Name: Jake Tsuchiyama
// Date: 1/5/22
// Title: Lab 1 - Copying Files w/ System Calls
// Description: The program copies binary and text files simultaneously using the system calls. 
// The program also displays the runtime of the copying process.

#include <stdio.h>   // fprintf(), fread(), fwrite(), fopen(), fclose()
#include <stdlib.h>  // malloc(), free()
#include <pthread.h> // pthread_create()
#include <unistd.h>  // read(), write()
#include <fcntl.h>   // open(), close()
#include <errno.h>   // errno
#include <time.h>    // clock()
#include <sys/mman.h> // mmap()

#include <sys/types.h>
#include <sys/stat.h>

#define BUF_SIZE 2048 //buffer size 

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

// copies a files form src_filename to dst_filename using syscalls open(), read(), write(), close()
int syscall_copy(char* src_filename, char* dst_filename)  {
	int src_fd = open(src_filename, O_RDONLY);	// opens a file for reading
	if (src_fd < 0) { // open() error checking
		fprintf(stderr, "unable to open %s for reading: %i\n", src_filename, errno);
		close(src_fd);
		exit(0);
	}
	// O_WRONLY - write only
	// O_CREAT - create file if it doesn't exist
	// O_TRUNC - truncates file to length 0 if it already exists (deletes existing file)
	// opens a file for writing; erases old file/creates a new file
	int dst_fd = open(dst_filename, O_WRONLY | O_CREAT | O_TRUNC);	
	if (dst_fd < 0) { // open() error checking
		fprintf(stderr, "unable to open/create %s for writing: %i\n", dst_filename, errno);
		close(dst_fd);
		exit(0);
	}

	// read / write loop
	void* buf = malloc((size_t)BUF_SIZE);   // allocate a buffer to store read data

	// reads and writes up to BUF_SIZE bytes from src_filename
	int nr;
	while((nr = read(src_fd, buf, BUF_SIZE)) != 0)
		write(dst_fd, buf, nr); // writes nr bits from the buf to the dst_fd	
	
	
	// closes src_fd file descriptor
	close(src_fd);	

	// closes dst_fd file descriptor
	close(dst_fd);	

	// frees memory used for buf
	free(buf);

	return 0;
}

// checks the runtime to copy a file using functions vs syscalls
void check_copy_times(char* src_filename, char* dst_filename) {
	clock_t func_start, func_end, syscall_start, syscall_end;	
	
	// capture runtime of func_copy() using start clock, call the copy, end the clock
	//func_start = clock();
	//func_copy(src_filename, dst_filename);	
	//func_end = clock();
	//double func_time = ((double) (func_end-func_start)) / CLOCKS_PER_SEC;

	// capture runtime of syscall_copy() using start clock, call the copy, end the clock
	syscall_start = clock();
	syscall_copy(src_filename, dst_filename);
	syscall_end = clock();
	double syscall_time = ((double) (syscall_end-syscall_start)) / CLOCKS_PER_SEC;	

	//printf("time to copy using functions: %.7f\n", func_time);
	printf("time to copy using syscalls: %.7f\n", syscall_time);
}

int main(int argc, char* argv[]) {
	if (argc != 3) {  // check correct usage
		fprintf(stderr, "usage: %s <src_filename> <dst_filename>\n", argv[0]);
		exit(1);
	}

	//get the source and destination files from the command line arguments
	char* src = argv[1];
	char* dest = argv[2];
	
	//call the check copy times function
	check_copy_times(src, dest);	

	return 0;
}
