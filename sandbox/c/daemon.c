#include <unistd.h>
#include <sys/types.h>

#include <stdio.h>

int main() {
	const int ETIME = 5;
	pid_t pid = fork();
	if( pid < 0 ) {
		fprintf( stderr, "Fork error!\n" );
		return pid;
	} else if( pid > 0 ) {
		fprintf( stdout, "Daemon will run in %d \n", ETIME );
		return 0;
	} else {
		for( int i = 0; i < ETIME; ++i ) {
			fprintf( stdout, "Daemon message: %d\n", i );
			sleep( 1 );
		}
	}
	return 0;
}
