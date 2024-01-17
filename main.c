#include <string.h>
#include <termios.h>
#include <stdio.h>

#include "shell.h"
#include "shell_structs.h"
#include "jobs.h"
#include "shell_cmds.h"

int terminalDescriptor;
pid_t shellPgid;
struct termios defaultTerminalSettings;
int bgFreeNumber = 1;
Job* headBgJobFake = NULL;
char readInterruptionFlag = 0;
char prompt[1024];

/*
	Основной цикл работы шелла. Здесь оставлю комментарии около конкретных строчек
*/
int main(int argc, char *argv[]) {
	char line[LINE_SZ];
	Job* newJobsHead;
	Job* currentJob;
	Job* nextJob;

	initShell();

	// Запрашиваем команду повторно до тех пор, пока пользователь не пришёл EOF
	while (promptline(updatePrompt(), line, sizeof(line)) > 0) {
		// Проверка на прерывание чтения в promptline сигналом
		if (readInterruptionFlag) {
			fprintf(stderr, "\n");
			readInterruptionFlag = 0;
			continue;
		}
		// Пытаемся спарсить из прочитанной строки задачи и процессы в них
		if ((newJobsHead = parseline(line)) == NULL)
			continue;
		// printJobs(newJobsHead);
		nextJob = newJobsHead;
		while (nextJob != NULL) {
			// Обновляем статусы фоновых задач перед обработкой текущей задачи для того, чтобы команда jobs показала актуальные статусы
			updateJobsStatuses(headBgJobFake->next);

			currentJob = nextJob;
			nextJob = nextJob->next;
			// Извлекаем текущую задачу из списка новых задач. Далее она будет либо исполнена на первом плане и очищена, либо отправлена в список фоновых
			extractJobFromList(currentJob);
			// Если задача не относится к шелл-специфичным командам, которые надо исполнять в процессе шелла, вызываем processJob
			if (!processShellSpecificMainCommand(currentJob))
				processJob(currentJob);
			
			// Обновляем статусы фоновых задач и выводим изменившиеся, затем очищаем память от завершившихся фоновых задач
			updateJobsStatuses(headBgJobFake->next);
			printJobsNotifications(headBgJobFake->next, 1);
			cleanJobs(headBgJobFake->next);
		}
	}
	// При наличии фоновых задач, отправляем им SIGHUP
	if (headBgJobFake->next) {
		fprintf(stderr, "\nThese background jobs will get SIGHUP:\n");
		sendSigHups(headBgJobFake->next);
		updateJobsStatuses(headBgJobFake->next);
		printJobsNotifications(headBgJobFake->next, 0);
	}
	fprintf(stderr, "Bye!\n");
	// Очищаем список фоновых задач, включая фековую голову списка
	freeJobs(headBgJobFake);
	return 0;
}