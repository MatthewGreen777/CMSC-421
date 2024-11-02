#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/syscalls.h>
#include <linux/errno.h>

SYSCALL_DEFINE3(memory_copy, unsigned char __user *, to, unsigned char __user *, from, int, size) {
	int i; // declare int to be used for loop
	unsigned char test; // variable used to hold data when transfering

	// Checks that memory being point at are accessible
	if (!access_ok(from, size) || !access_ok(to, size)) {
		return -EFAULT; // return -EFAULT, -1, if not accessible
	}

	// loop to interate through each byte of size from and copy to to
	for (i = 0; i < size; i++) {
		// retrive byte from the from pointer and checks that it passes
		if (get_user(test, from +i)) {
			return -EFAULT; // byte accessed failed
		}
		// write the byte retrieved to the destination buffer and chekc if succeeded
		if (put_user(test, to + i)) {
			return -EFAULT; // byte not coppied
		}
	}

	return 0;
}
