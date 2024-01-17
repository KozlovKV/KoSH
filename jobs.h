/*
  Модуль, отвечающий за вспомогательные операции над системой задач и процессов 
*/
#pragma once

#include "shell_structs.h"

/*
  Создаёт новую задачу на heap и возвращает её.
  Если headJob != NULL, прикрепит задачу в хвост списка
*/
Job* createNewJob(Job* headJob);

/*
  Создаёт в задаче job новый процесс на heap и возвращает его.
  Процесс прикрепится в хвост списка процессов в job (если job->headProcess == NULL, новый процесс будет назначен головным)
  Копирует аргументы и флаги из cmd, выделяя под каждый непустой аргумент из cmdargs память динамически
*/
Process* createNewProcessInJob(Job* job, Command cmd);

/*
  Функция, вынимающая задачу из списка и соединяющая задачи перед и после в случае их существования
  Во многих местах вызывается лишь на всякий случай
*/
void extractJobFromList(Job* job);

Job* getJobByBgNumber(Job* headJob, int bgNumber);
Process* getProcessByPid(Job* job, pid_t pid);

/*
  Вызывает различные предикаты для проверки статусов у процессов задачи и выбора нового флага
  для поля status.
  Если статус задачи поменялся переводит флаг notified в значение 0
*/
void updateJobStatus(Job* job);
/*
  Проводит неблокирующий wait на все возможные события в группе задачи до тех пор, 
  пока группа процессов существует либо в каком-либо из процессов произошло изменение.
  Записывает информацию о событии в структуру связанного с этим событием процесса
*/
void updateJobSiginfo(Job* job);
/*
  Для всех задач в списке вызывает updateJobSiginfo, а затем updateJobStatus
*/
void updateJobsStatuses(Job* headJob);
/*
  Для задач в списке, все процессы в которых завершились, а пользователь был оповещён финальном состоянии задачи,
  вызывает freeJob, очищая память и убивая зомби
*/
void cleanJobs(Job* headJob);

/*
  Возвращает 1, когда все процессы в задаче завершились любым из возможных образов (по exit или по сигналу)
*/
int isAllProcessesEnded(Job* job);
/*
  Возвращает 1, когда хотя бы один процесс в задаче находится в режиме исполнения 
  (через wait либо не было получено события для него, либо si_code == CLD_CONTINUED)
*/
int isJobRunning(Job* job);
/*
  Возвращает 1, если все процессы в задаче завершились без ошибок 
  (si_code == CLD_EXITED && si_status == 0)
*/
int isJobDone(Job* job);
/*
  Возвращает 1, если хотя бы один процесс в задаче завершился с ошибкой 
  (si_code == CLD_EXITED && si_status != 0)
*/
int isJobExited(Job* job);
/*
  Возвращает 1, если хотя бы один процесс в задаче был убит сигналом
*/
int isJobTerminated(Job* job);
/*
  Возвращает 1, если хотя бы один процесс в задаче был остановлен
*/
int isJobStopped(Job* job);

/*
  Служебные функции для вывода информации о задачах и процессах
*/
void printJobs(Job* headJob);
void printJob(Job* job);
void printProcesses(Job* job);
void printProcess(Process* process);

/*
  Набор функций, которые отвечают за печать списка задач, каждая из которых будет выведена в формате:
  [BG_NUMBER]   PGID   (+ STATUS) // + будет выводиться в том случае, если новый статус ранее не демонстрировался пользователю
    Process Args 1
    Process Args 2
    ...
  ...
*/
void printJobsNotifications(Job* headJob, int onlyNotNotified);
/*
  Кроме вывода информации о задачи, переводит флаг notified в значение 1
*/
void printJobNotification(Job* job);
void printProcessesNotification(Job* job);
void printProcessNotification(Process* process);

/*
  Для каждой задачи в списке вызывает freeJob
*/
void freeJobs(Job* headJob);
/*
  Очищает память из-под задачи:
  - Вызывает freeProcesses для job->headProcess
  - Очищает пути перенаправления ввода-вывода, если они были определены
  - Вызывает killZombies, если job->pgid != 0
*/
void freeJob(Job* job);

/*
  Отправляет всем задачам в списке SIGHUP и SIGCONT 
  (используется для пробуждения и убийства фоновых процессов перед выходом из шелла, тоже дискуссионный момент)
*/
void sendSigHups(Job* headJob);

/*
  Очищает выделенную под процесс память
*/
void freeProcesses(Process* headProcess);