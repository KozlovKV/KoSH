/*
	Хэдэр со структурами, используемыми почти во всех модулях шелла
*/
#pragma once

#include <unistd.h>
#include <sys/wait.h>
#include <wordexp.h>

#define MAXARGS 256
#define MAXCMDS 50
#define LINE_SZ 1024

/*
	Хранит список аргументов команды и битовый флаг для пайпов
*/
typedef struct command {
	char *cmdargs[MAXARGS];
	char cmdflag;
} Command;

/*
	Значения cmdflag. Записываются побитово
*/
#define OUTPIP  01
#define INPIP   02

/*
	И процесс, и задача, реализованы через двусвязные списки 
	(хорошей была бы идея отдельно создать структуру списка и отдельно уже собственно 
	процесс и задачу, но заморачиваться здесь над полиморфизмом мне не хотелось совершенно)
*/

typedef struct process
{
	char line[LINE_SZ];
	wordexp_t args;
	char pipesFlags;
	pid_t pid;
	siginfo_t statusInfo; // Поле для сохранения статуса процесса, полученного из waitid
	struct process* prev;
	struct process* next;
} Process;

typedef struct job {
	Process* headProcess;
	pid_t pgid;
	char initialFg; // Описывает, должна ли задача переводиться в фон при запуске и никак не используется в дальнейшем
	int bgNumber;
	char status;
	char notified;
	int inFd;
	int outFd;
	char* inPath;
	char* outPath;
	char appendFlag;
	struct job* prev;
	struct job* next;
} Job;

// Возможные значения поля Job.status
#define J_SETTING 0
#define J_RUNNING 1
#define J_DONE 2
#define J_EXIT 3
#define J_TERMINATED 4
#define J_STOPPED 5