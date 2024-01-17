#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <ctype.h>

#include "shell.h"
#include "shell_structs.h"
#include "jobs.h"
#include "shell_cmds.h"

int processShellSpecificForkedCommand(Process* process) {
	if (strcmp(process->args.we_wordv[0], "jobs") == 0) {
		jobs_cmd();
		return 1;
	}
	return 0;
}

void jobs_cmd() {
	printJobsNotifications(headBgJobFake->next, 0);
}

int processShellSpecificMainCommand(Job* job) {
	Process* process = job->headProcess;
	if (process->next || process->args.we_wordc < 2) {
		return 0;
	}
	if (process->args.we_wordv[1][0] == '%') {
		if (strcmp(process->args.we_wordv[0], "fg") == 0) {
			fg_cmd(process->args.we_wordv[1]);
			return 1;
		}
		if (strcmp(process->args.we_wordv[0], "bg") == 0) {
			bg_cmd(process->args.we_wordv[1]);
			return 2;
		}
		if (strcmp(process->args.we_wordv[0], "kill") == 0) {
			kill_cmd(process->args.we_wordv[1]);
			return 3;
		}
	}
	if (strcmp(process->args.we_wordv[0], "cd") == 0) {
		cd_cmd(process->args.we_wordv[1]);
		return 4;
	}
	return 0;
}

static int parseFromIntFromPercentArg(char* arg) {
	int num;
	for (int i = 1; arg[i]; ++i) {
		if (!isdigit(arg[i])) {
			fprintf(stderr, "Cannot recognize background number\n");
			return -1;
		}
	}
	sscanf(arg+1, "%d", &num);
	return num;
}

void fg_cmd(char* argNum) {
	int bgNumber = parseFromIntFromPercentArg(argNum);
	if (bgNumber == -1)
		return;
	Job* bgJob = getJobByBgNumber(headBgJobFake->next, bgNumber);
	if (!bgJob)	{
		fprintf(stderr, "Background job %d wasn't found\n", bgNumber);
		return;
	}
	extractJobFromList(bgJob);
	tcsetpgrp(terminalDescriptor, bgJob->pgid);
	sigsend(P_PGID, bgJob->pgid, SIGCONT);
	waitFgJob(bgJob);
	tcsetpgrp(terminalDescriptor, shellPgid);
	tcsetattr(terminalDescriptor, TCSAFLUSH, &defaultTerminalSettings);
}

void bg_cmd(char* argNum) {
	int bgNumber = parseFromIntFromPercentArg(argNum);
	if (bgNumber == -1)
		return;
	Job* bgJob = getJobByBgNumber(headBgJobFake->next, bgNumber);
	if (!bgJob)	{
		fprintf(stderr, "Background job %d wasn't found\n", bgNumber);
		return;
	}
	sigsend(P_PGID, bgJob->pgid, SIGCONT);
}

void kill_cmd(char* argNum) {
	int bgNumber = parseFromIntFromPercentArg(argNum);
	if (bgNumber == -1)
		return;
	Job* bgJob = getJobByBgNumber(headBgJobFake->next, bgNumber);
	if (!bgJob)	{
		fprintf(stderr, "Background job %d wasn't found\n", bgNumber);
		return;
	}
	sigsend(P_PGID, bgJob->pgid, SIGKILL);
}

void cd_cmd(char* path) {
	if (chdir(path) != 0)
		perror("Failed to changed directory");
}