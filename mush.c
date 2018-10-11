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

void run_command(command_s *command)
{
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

	wait(NULL);
}

void run_cd(command_s *command) {
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

int built_in(command_s *command) {
	if (0 == strncmp(command->elems[0], "cd", strnlen(command->elems[0], 3))) {
		run_cd(command);
		return 1;
	} else if (0 == strncmp(command->elems[0], "history", strnlen(command->elems[0], 8))) {
		// run_history(command);
		printf("history\n");
		return 1;
	} else if (0 == strncmp(command->elems[0], "exit", strnlen(command->elems[0], 5))) {
		exit_shell = 1;
		return 1;
	}
	return 0;
}

void loop()
{
	command_s *command;

	while (!exit_shell) {
		if (!print_prompt()) {
			return;
		}

		command = get_command();
		if (NULL == command) {
			exit_shell = 1;
			printf("\n");
			continue;
		}

		// add to history

		if (0 == command->cur_len) {
			// change to goto
			free_command(command);
			continue;
		}

		//print_command(command);
		// if built in
		if (built_in(command)) {
		} else {
			run_command(command);
		}
	}
}

int main()
{
	loop();
	return 1;
}
