/* Author: Aidan Bush
 * Assign: 1
 * Course: CMPT 360
 * Date: Oct. 4, 18
 * File: mush.c
 * Description: Main mush shell file.
 */

/* standard libraries */
#include <unistd.c>
#include <stdio.h>

/* system libraries */
#include <linux/limits.h>

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
void loop()
{
	bool exit = false;
	char *line = NULL;

	while (!exit) {
		if (!print_prompt) {
			return;
		}

		// grab input
		get_command();

		// parse input

		// if command run
		// wait for child to die
	}
}

int main(int argc, char *argv[])
{
	loop();
	return 1;
}
