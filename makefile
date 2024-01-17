main:
	gcc -Wall main.c jobs.c shell_cmds.c shell.c promptline.c parseline.c -o main.out
utils:
	gcc plus_one.c -o plus_one.out
	gcc write_with_delay.c -o write_with_delay.out
