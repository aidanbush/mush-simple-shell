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
#include <signal.h>
#include <sys/types.h>
#include <errno.h>

/* project includes */
#include "command.h"

int exit_shell;
str_arr_s *history = NULL;

// sigint handler, does nothing with the signal
void sigint_handler(int par)
{
	return;
}

// create the new sigint handler
int create_sigint_handler(void)
{
	struct sigaction sa = {
		.sa_handler = &sigint_handler
	};

	sigemptyset(&sa.sa_mask);

	int err = sigaction(SIGINT, &sa, NULL);
	if (err == -1)
		return 0;

	return 1;
}

// prints the prompt
int print_prompt(void)
{
	char path[PATH_MAX];

	if (getcwd(path, PATH_MAX) == NULL) {
		fprintf(stderr, "error in getting path name\n");
		return 0;
	}

	printf("%s $ ", path);

	return 1;
}

// run the given command
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

	// check if fails
	while (-1 == wait(&exit_code))
		if (errno != EINTR)
			break;

	exit_code = (WEXITSTATUS(exit_code));

	if (0 == exit_code)
		printf("[%d] terminated successfully\n", pid);
	else
		printf("[%d] terminated with code %d\n", pid, exit_code);
}

void cd(str_arr_s *command)
{
	char *home = NULL;

	if (2 < command->cur_len) {
		fprintf(stderr, "cd: too many arguments\n");
	} else if (2 == command->cur_len) {
		chdir(command->elems[1]);
	} else {
		home = getenv("HOME");
		if (NULL == home) {
			fprintf(stderr, "cd: $HOME undefined\n");
			return;
		}
		chdir(home);
	}
}

// prints the history
void print_history(void)
{
	for (int i = 0; i < history->cur_len; i++)
			printf("%s", history->elems[i]);
}

int set_env(char *command, char *eq_pos)
{
	// fix eq_pos
	char c = eq_pos[0];

	eq_pos[0] = '\0';

	int err = setenv(command, eq_pos + 1, 1);

	eq_pos[0] = c;

	return err;
}

int built_in(str_arr_s *command)
{
	// fix
	char *equal_pos = NULL;

	if (0 == strncmp(command->elems[0], "cd",
				strnlen(command->elems[0], 3))) {
		cd(command);
		return 1;
	} else if (0 == strncmp(command->elems[0], "history",
				strnlen(command->elems[0], 8))) {
		print_history();
		return 1;
	} else if (0 == strncmp(command->elems[0], "exit",
				strnlen(command->elems[0], 5))) {
		exit_shell = 1;
		return 1;
	} else if (command->cur_len == 1 &&
			(equal_pos = strchr(command->elems[0], '=')) != NULL) {
		set_env(command->elems[0], equal_pos);
		return 1;
	}

	return 0;
}

void add_to_history(char *cmd_str)
{
	add_str_arr(history, cmd_str, strlen(cmd_str)); // replace with actual length
}

void loop(void)
{
	str_arr_s *command;
	char *cmd_str = NULL;
	int eof = 0;

	while (!exit_shell) {
		if (!print_prompt()) {
			exit_shell = 1;
			continue;
		}

		command = get_command(&cmd_str, &eof);
		if (NULL == command) {
			if (eof)
				exit_shell = 1;

			printf("\n");
			continue;
		}

		if (0 == command->cur_len) {
			// change to goto
			free_str_arr(command);
			continue;
		}

		// if built in
		if (!built_in(command))
			run_command(command);

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
	if (!create_sigint_handler())
		return 1;

	history = init_str_arr(COMMAND_START_SIZE);

	loop();

	free_str_arr(history);
	history = NULL;

	return 0;
}
