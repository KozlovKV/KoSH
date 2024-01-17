#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h>
#include <time.h>

int main(int argc, char** argv) {
	if (argc != 3) {
		fprintf(stderr, "Invalid args\n");
		exit(-1);
	}
	int delay;
	sscanf(argv[1], "%d", &delay);
	sleep(delay);
	fprintf(stdout, "%s\n", argv[2]);
	return 0;
}
