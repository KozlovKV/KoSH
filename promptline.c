#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include "shell.h"

char* updatePrompt() {
	char buf[1017] = "";
	getcwd(buf, sizeof(buf));
	sprintf(prompt, "\n[%s]\n$ ", buf);
	return prompt;
}

int promptline(char *prompt, char *line, int sizline)
{
	int n = 0;
	int readCount;
	--sizline;

	write(1, prompt, strlen(prompt));
	while (1) {
		readCount = read(0, (line + n), sizline-n);
		if (readCount < 0 && errno == EINTR) {
			readInterruptionFlag = 1;
			*(line) = '\0';
			return 1;
		}
		n += readCount;
		/*
		*  check to see if command line extends onto
		*  next line.  If so, append next line to command line
		*/

		if (*(line+n-2) == '\\' && *(line+n-1) == '\n') {
			*(line+n) = ' ';
			*(line+n-1) = ' ';
			*(line+n-2) = ' ';
			continue;   /*  read next line  */
		}
		*(line+n) = '\n';
		*(line+n+1) = '\0';
		return(n);      /* all done */
	}
}