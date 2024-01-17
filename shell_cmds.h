/*
  Модуль с шелл-специфичными командами
*/
#pragma once

#include "shell_structs.h"

/*
  Отвечает за запуск шелл-специфичных команд в подпроцессе.
*/
int processShellSpecificForkedCommand(Process* process);
void jobs_cmd();

/*
  Отвечает за запуск шелл-специфичных команд, которые должны исполняться в процессе самого шелла
  Данные команды сами по себе никак не взаимодействуют с вводом-выводом, поэтому требованием является 
  отсутствие пайпов (то есть 1 процесс в задаче), ввод-вывод через <, >, >> преопределить можно, но не имеет смысла
*/
int processShellSpecificMainCommand(Job* job);
void fg_cmd(char* argNum);
void bg_cmd(char* argNum);
void kill_cmd(char* argNum);
void cd_cmd(char* path);