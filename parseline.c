#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "shell.h"
#include "shell_structs.h"
#include "jobs.h"

static char *blankskip(char *);

Job* parseline(char *line)
{
	Job* headJob = createNewJob(NULL);
	Job* newJob = headJob;
	char *infile = NULL, *outfile = NULL;
	Command cmds[MAXCMDS];
	int nargs = 0, ncmds = 0;
	char *currentLinePtr;
	char error = 0;
	static char delim[] = " \t|&<>;\n\"";

	/* initialize  */
	currentLinePtr = line;
	for (int cmdIndex = 0; cmdIndex < MAXCMDS; ++cmdIndex) {
		cmds[cmdIndex].cmdflag = 0;
		for (int argIndex = 0; argIndex < MAXARGS; ++argIndex) {
			cmds[cmdIndex].cmdargs[argIndex] = (char*) NULL;
		}
	}

	while (*currentLinePtr) {        /* until line has been parsed */
		currentLinePtr = blankskip(currentLinePtr);       /*  skip white space */
		if (!*currentLinePtr) break; /*  done with line */

		/*  handle <, >, |, &, and ;  */
		if (*currentLinePtr == '&' || *currentLinePtr == ';') {
			if (*currentLinePtr == '&')
				newJob->initialFg = 0;
			*currentLinePtr++ = '\0';
			createNewProcessInJob(newJob, cmds[ncmds]);
			++ncmds;
			nargs = 0;
			newJob = createNewJob(newJob);
			continue;
		}
		if (*currentLinePtr == '>') {
			if (newJob->outPath) {
				fprintf(stderr, "syntax error\n");
				error = 1;
				break;
			}
			if (*(currentLinePtr+1) == '>') {
				newJob->appendFlag = 1;
				*currentLinePtr++ = '\0';
			}
			*currentLinePtr++ = '\0';
			currentLinePtr = blankskip(currentLinePtr);
			if (!*currentLinePtr) {
				fprintf(stderr, "syntax error\n");
				error = 1;
				break;
			}

			outfile = currentLinePtr;
			currentLinePtr = strpbrk(currentLinePtr, delim);
			newJob->outPath = (char*) malloc(currentLinePtr - outfile + 1);
			memcpy(newJob->outPath, outfile, currentLinePtr - outfile);
			newJob->outPath[currentLinePtr - outfile] = '\0';
			if (isspace(*currentLinePtr))
				*currentLinePtr++ = '\0';
			continue;
		}
		if (*currentLinePtr == '<') {
			*currentLinePtr++ = '\0';
			currentLinePtr = blankskip(currentLinePtr);
			if (!*currentLinePtr || newJob->inPath) {
				fprintf(stderr, "syntax error\n");
				error = 1;
				break;
			}
			infile = currentLinePtr;
			currentLinePtr = strpbrk(currentLinePtr, delim);
			newJob->inPath = (char*) malloc(currentLinePtr - infile + 1);
			memcpy(newJob->inPath, infile, currentLinePtr - infile);
			newJob->inPath[currentLinePtr - infile] = '\0';
			if (isspace(*currentLinePtr))
				*currentLinePtr++ = '\0';
			continue;
		}
		if (*currentLinePtr == '|') {
			if (nargs == 0) {
				fprintf(stderr, "syntax error\n");
				error = 1;
				break;
			}
			cmds[ncmds].cmdflag |= OUTPIP;
			createNewProcessInJob(newJob, cmds[ncmds]);
			cmds[++ncmds].cmdflag |= INPIP;
			*currentLinePtr++ = '\0';
			nargs = 0;
			continue;
		}
		if (*currentLinePtr == '"') {
			*currentLinePtr++ = '\0';
			cmds[ncmds].cmdargs[nargs++] = currentLinePtr;
			cmds[ncmds].cmdargs[nargs] = NULL;
			currentLinePtr = strpbrk(currentLinePtr, "\"\n\0");
			if (*currentLinePtr)
				*currentLinePtr++ = '\0';
			continue;
		}
		/*  a command argument  */
		cmds[ncmds].cmdargs[nargs++] = currentLinePtr;
		cmds[ncmds].cmdargs[nargs] = NULL;
		currentLinePtr = strpbrk(currentLinePtr, delim);
		if (isspace(*currentLinePtr))
			*currentLinePtr++ = '\0';
	}  /* close while  */
	if (nargs > 0 && !error) {
		createNewProcessInJob(newJob, cmds[ncmds]);
	}

	/*  error check  */

	/*
	*  The only errors that will be checked for are
	*  no command on the right side of a pipe
	*  no command to the left of a pipe is checked above
	*/
	if (cmds[ncmds-1].cmdflag & OUTPIP) {
		if (nargs == 0) {
			fprintf(stderr, "syntax error\n");
			error = 1;
		}
	}

	// Избавляемся от пустой задачи в конце списка
	newJob = headJob;
	while (newJob->next) {
		if (newJob->next->headProcess == NULL) {
			freeJobs(newJob->next);
			newJob->next = NULL;
			break;
		}
		newJob = newJob->next;
	}

	// Если у головной задачи нет процессов или где-то произошла ошибка, очищаем память из-под созданных задач (задачи) и возвращаем NULL
	if (error || headJob->headProcess == NULL) {
		freeJobs(headJob);
		return NULL;
	}
	return headJob;
}

static char * blankskip(char *s)
{
	while (isspace(*s) && *s) ++s;
	return(s);
}