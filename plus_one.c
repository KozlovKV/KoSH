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
	int num;
	fscanf(stdin, "%d", &num);
	fprintf(stderr, "%d\n", num+1);
	fprintf(stdout, "%d\n", num+1);
	return 0;
}
