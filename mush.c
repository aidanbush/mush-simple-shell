/* Author: Aidan Bush
 * Assign: 1
 * Course: CMPT 360
 * Date: Oct. 4, 18
 * File: mush.c
 * Description: Main mush shell file.
 */

/* standard libraries */
#include <unistd.h>
#include <stdio.h>
#include <linux/limits.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

/* project includes */
#include "command.h"

volatile int exit_shell = 0;
str_arr_s *history = NULL;

int print_prompt()
{
	char path[PATH_MAX];

	if (getcwd(path, PATH_MAX) == NULL) {
		fprintf(stderr, "error in getting path name\n");
		return 0;
	}

	printf("%s $ ", path);

	return 1;
}

void run_command(str_arr_s *command)
{
	int exit_code = 0;
	int pid = fork();

	if (0 > pid) {
		// error
		fprintf(stderr, "unable to create process\n");
	} else if (0 == pid) {
		// child
		if (-1 == execvp(command->elems[0], command->elems)) {
			fprintf(stderr, "unable to run command %s\n",
					command->elems[0]);
		}
		exit(1);
	}
	// parent

	wait(&exit_code);
	if (0 == exit_code)
		printf("[%d] terminated successfully)\n", pid);
	else
		printf("[%d] terminated with code %d\n", pid, exit_code);
}

void run_cd(str_arr_s *command)
{
	char *home = getenv("HOME");

	if (2 < command->cur_len) {
		fprintf(stderr, "cd: too many arguments\n");
		return;
	} if (2 == command->cur_len) {
		chdir(command->elems[1]);
	} else {
		if (NULL == home) {
			fprintf(stderr, "cd: $HOME undefined\n");
			return;
		}
		chdir(home);
	}
}

void run_history()
{
	for (int i = 0; i < history->cur_len; i++)
			printf("%s", history->elems[i]);
}

int built_in(str_arr_s *command) {
	if (0 == strncmp(command->elems[0], "cd", strnlen(command->elems[0], 3))) {
		run_cd(command);
		return 1;
	} else if (0 == strncmp(command->elems[0], "history", strnlen(command->elems[0], 8))) {
		run_history();
		return 1;
	} else if (0 == strncmp(command->elems[0], "exit", strnlen(command->elems[0], 5))) {
		exit_shell = 1;
		return 1;
	}
	return 0;
}

void add_to_history(char *cmd_str)
{
	// add string to history
	add_str_arr(history, cmd_str, strlen(cmd_str)); // replace with actual length
}

void loop()
{
	str_arr_s *command;
	char *cmd_str = NULL;

	while (!exit_shell) {
		if (!print_prompt()) {
			return;
		}

		command = get_command(&cmd_str);
		if (NULL == command) {
			exit_shell = 1;
			printf("\n");
			continue;
		}

		if (0 == command->cur_len) {
			// change to goto
			free_str_arr(command);
			continue;
		}

		//print_str_arr(command);
		// if built in
		if (!built_in(command)) {
			run_command(command);
		}

		// add to history
		if (NULL != cmd_str) {
			add_to_history(cmd_str);
			free(cmd_str);
		}

		free_str_arr(command);
	}
}

int main()
{
	history = init_str_arr(COMMAND_START_SIZE);

	loop();

	free_str_arr(history);
	history = NULL;

	return 1;
}
