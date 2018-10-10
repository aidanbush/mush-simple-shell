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

/* project includes */
#include "command.h"

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
	}
	// parent

	wait(NULL);
}

void loop()
{
	int exit = 0;
	command_s *command;

	while (!exit) {
		if (!print_prompt()) {
			return;
		}

		command = get_command();
		if (0 == command->cur_len) {
			// change to goto
			free_command(command);
			continue;
		}

		// if command
		// run command
		// print_command(command);
		run_command(command);

		// wait for child to die
	}
}

int main()
{
	loop();
	return 1;
}
