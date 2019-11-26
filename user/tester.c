#include "types.h"
#include "user.h"
#include "fcntl.h"

#define assert(x) if (x) {} else { \
   printf(1, "%s: %d ", __FILE__, __LINE__); \
   printf(1, "assert failed (%s)\n", # x); \
   printf(1, "TEST FAILED\n"); \
   kill(getpid()); \
   exit(); \
}

// Function prototypes
int null_test_one(int);
int null_test_two(int);
int bounds_test(int);

int main()
{
	int p = getpid();
	printf(1, "Running three tests ...\n");
	printf(1, "Null test one: \n");
	null_test_one(p);
	printf(1, "Null test two: \n");
	null_test_two(p);
	printf(1, "Bounds test:   \n");
	bounds_test(p);
	exit();
}

int null_test_one(int p)
{
	int pid = fork();
	if(pid < 0)
	{
		printf(1, "Fork failed. NULL TEST ONE FAILED STRANGELY!\n");
		kill(getpid());
		exit();
	}
	else if(pid == 0)
	{
		uint *nullPtr = (uint*) 0;
		printf(1, "null dereference: ");
		printf(1, "%x %x\n", nullPtr, *nullPtr);
		// this process should be killed. If not, then the test failed.
		printf(1, "NULL TEST ONE FAILED!\n");
		kill(p);
		exit();
	}
	else
	{
		wait();
		printf(1, "NULL TEST ONE PASSED!\n");
		return 0;
	}
}

// any dereference from first page should fault
int null_test_two(int p)
{
	int pid = fork();
	if(pid < 0)
	{
		printf(1, "Fork failed. NULL TEST TWO FAILED STRANGELY!\n");
		exit();
	}
	else if(pid == 0)
	{
		uint *badPtr = (uint*) 4095;
		printf(1, "bad dereference (0x0fff): ");
		printf(1, "%x %x\n", badPtr, *badPtr);
		// this process should be killed. If not, then the test failed.
		printf(1, "NULL TEST TWO FAILED!\n");
		kill(p);
		exit();
	}
	else
		wait();
	printf(1, "NULL TEST TWO PASSED!\n");
	return 0;
}

// Ensure syscall arg checks null page
int bounds_test(int p)
{
	char *arg;
	int fd = open("tmp", O_WRONLY|O_CREATE);
	assert(fd != -1);
	// at zero
	arg = (char*) 0x0;
	assert(write(fd, arg, 10) == -1);
	// within null page
	arg = (char*) 0x400;
	assert(write(fd, arg, 1024) == -1);
	// spanning null page and code
	arg = (char*) 0xfff;
	assert(write(fd, arg, 2) == -1);
	printf(1, "BOUNDS TEST PASSED!\n");
	return 0;
}

