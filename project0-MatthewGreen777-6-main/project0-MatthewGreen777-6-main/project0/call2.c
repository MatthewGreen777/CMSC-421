#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#define __NR_memory_copy 549

long memory_copy_syscall(unsigned char *to, unsigned char *from, int size) {
	return syscall(__NR_memory_copy, to, from, size);
}

bool normal_case(); // test normal case of where from is a populated char array and to is an empty char array
bool to_pointer_null(); // test case of where the to pointer is null
bool from_pointer_null(); // test case of where the from pointer is null
bool  both_pointer_null(); // test case of where both pointers are null

int main(int argc, char *argv[]) {
	bool pass = true;
	pass = normal_case();
	pass = to_pointer_null();
	pass = from_pointer_null();
	pass = both_pointer_null();

	printf("Pass all test: %d\n", pass);
	return 0;
}

bool normal_case() {
	unsigned char from[] = "I am here";
	unsigned char to[20];

	if(memory_copy_syscall(to, from, strlen(from)+1) != 0) {
		return false;
	}

	return true;
}

bool to_pointer_null() {
	unsigned char from[] = "Hello";

	if (memory_copy_syscall(NULL, from, strlen(from)+1) >= 0) {
		return false;
	}
	return true;
}

bool from_pointer_null() {
	unsigned char to[10];

	if (memory_copy_syscall(to, NULL, 5) >= 0) {
		return false;
	}
	return true;
}

bool both_pointer_null() {
	if (memory_copy_syscall(NULL, NULL, 5) >= 0) {
		return false;
	}
	return true;
}

