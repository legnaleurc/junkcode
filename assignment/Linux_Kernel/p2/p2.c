#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

int main() {
	int r = -1, pid = getpid();
	r = syscall( __NR_hide_process, pid );
	printf( "%d, %d\n", pid, r );
	for( r = 0; r < 60; ++r ) {
		sleep( r );
	}
	return 0;
}
