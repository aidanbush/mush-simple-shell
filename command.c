/* Author: Aidan Bush
 * Assign: Assign 1
 * Course: CMPT 360
 * Date: Oct. 4, 18
 * File: command.c
 * Description:
 */

/* standard libraries */
#include <stdio.h>
#include <isspace.h>

/* system libraries */

/* project includes */
#include "command.h"

static int resize_command(command_s *command)
{
	if (NULL == command)
		return 0;

	int new_len = command_s->max_len * 2;

	char *tmp = realloc(command->elems, sizeof(char*) * new_len);
	if (NULL == tmp)
		return 0;

	command->elems = tmp;
	command->max_len = new_len;

	return 1;
}

static int add_command(command_s *command, char *str, size_t len)
{
	if (NULL == command)
		return 0;

	char *new_str = strndup(str, len);

	if (command->cur_len >= command->max_len)
		if (resize_command(command))
			return 0;

	command->elems[command->cur_len] = new_str;
	command->cur_len++;

	return 1;
}

static command_s *init_command(int len)
{
	command_s *command = calloc(sizeof(command_s), 1);
	if (NULL == command)
		return NULL;

	command->elems = malloc(sizeof(char*) * len);
	if (NULL == command->elems)
		return NULL;

	memset(command->elems, NULL, len);
}

static void free_command(command_s *command)
{
	for (int i = 0; i < command->cur_len; i++) {
		free(command->elem[i]);
	}
	free(commands->elems);
	free(commands);
}

static command_s *split_command(char *line, size_t len)
{
	command_s *command = NULL;
	size_t start = 0;

	// skip past leading whitespace

	for (size_t i = 0; i < line; i++) {
		if (isspace(line[i])) {
			// new section from start to i - 1
			if (!add_command(command, line + start, start - i - 1)) {
				return 0; // free up
			}
			// replace with nop
			while (isspace[i])
				i++;

			start = i;
		}
	}

	return command;
}

command_s *get_command()
{
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	command_s *command = NULL;

	read = getline(&line, &len, stdin);
	if (-1 == read) {
		free(line);
		return NULL;
	}
	// split line
	command = split_command(line, len);
	free(line);
	return command;
}

#ifdef _TEST

int main()
{
	// test create command
	// test add to command
	// test free command
	// test resize command
	// test split command
}

#endif /*_TEST*/
