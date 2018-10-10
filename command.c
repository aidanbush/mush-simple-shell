/* Author: Aidan Bush
 * Assign: Assign 1
 * Course: CMPT 360
 * Date: Oct. 4, 18
 * File: command.c
 * Description:
 */

/* standard libraries */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* project includes */
#include "command.h"

#define COMMAND_START_SIZE	4

// takes a command pointer and doubles the size + 1 of its elem array
// returns success or failure
static int resize_command(command_s *command)
{
	if (NULL == command)
		return 0;

	int new_len = (command->max_len + 1) * 2;

	char **tmp = realloc(command->elems, sizeof(char*) * new_len);
	if (NULL == tmp)
		return 0;

	// null remaining values
	for (int i = command->max_len; i < new_len; i++)
		tmp[i] = NULL;

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
		if (!resize_command(command))
			return 0;

	command->elems[command->cur_len] = new_str;
	command->cur_len++;

	return 1;
}

static command_s *init_command(int len)
{
	if (len <= 0)
		return NULL;

	command_s *command = calloc(sizeof(command_s), 1);
	if (NULL == command)
		return NULL;

	command->elems = malloc(sizeof(char*) * len);
	if (NULL == command->elems)
		return NULL;

	for (int i = 0; i < len; i++)
		command->elems[i] = NULL;

	command->max_len = len;
	command->cur_len = 0;

	return command;
}

void free_command(command_s *command)
{
	for (int i = 0; i < command->cur_len; i++)
		free(command->elems[i]);

	free(command->elems);
	free(command);
}

static command_s *split_command(char *line, size_t len)
{
	command_s *command = init_command(COMMAND_START_SIZE);
	size_t start = 0;
	size_t i = 0;

	// skip past leading whitespace
	while (isspace(line[start]))
		start++;

	i = start;

	while (i < len) {
		if (isspace(line[i])) {
			// new section from start to i - 1
			if (!add_command(command, line + start, i - start)) {
				free_command(command);
				return NULL;
			}
			// skip space after
			while (isspace(line[i]) && i < len)
				i++;

			start = i;
		}
		i++;
	}

	if (!isspace(line[start]) && start < len){
		if (!add_command(command, line + start, i - start)) {
			free_command(command);
			return NULL;
		}
	}

	return command;
}

void print_command(command_s *command)
{
	for (int i = 0; i < command->cur_len; i++)
		printf("%s ", command->elems[i]);
	printf("\n");
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

// tests
#ifdef _TEST

// #define TEST_LEN_MIN	1
#define TEST_LEN		10
#define TEST_VALUE		5
#define TEST_STR		"test_string"
#define TEST_STR_LEN		strlen((TEST_STR))

#define TEST_COM_STR_1		"mv 1 2 3 4 5 6 7 8 9 0 dir"
#define TEST_COM_STR_2		"  	messy  command 	line 	 "

#define TEST_COM_SPLIT_1	{"mv", "1", "2", "3", "4", "5", "6", "7", "8", \
				"9", "0", "dir"}
#define TEST_COM_SPLIT_2	{"messy", "command", "line"}

#define TEST_COM_STR_LEN_1	strlen((TEST_COM_STR_1))
#define TEST_COM_STR_LEN_2	strlen((TEST_COM_STR_2))

#define TEST_COM_NUM_1		12
#define TEST_COM_NUM_2		3


void test_init_command()
{
	// test create valid command
	command_s *command = init_command((TEST_LEN));

	assert(NULL != command);
	assert((TEST_LEN) == command->max_len);
	assert(0 == command->cur_len);
	assert(NULL != command->elems);

	for (int i = 0; i < (TEST_LEN); i++)
		assert(NULL == command->elems[i]);

	free_command(command);

	// test non accepted values
	assert(NULL == init_command(-1));
	assert(NULL == init_command(0));
}

void test_resize_command()
{
	command_s *command = init_command((TEST_LEN));
	for (int i = 0; i < (TEST_LEN); i++)
		command->elems[0] = (char *) (TEST_VALUE);

	assert(1 == resize_command(command));
	assert((TEST_LEN) < command->max_len);

	for (int i = 0; i < (TEST_LEN); i++)
		assert((char *) (TEST_VALUE) == command->elems[0]);

	for (int i = (TEST_LEN); i < command->max_len; i++)
		assert(NULL == command->elems[i]);

	free_command(command);

	// test with NULL command
	assert(0 == resize_command(NULL));
}

void test_add_command()
{
	command_s *command = init_command((TEST_LEN));
	for (int i = 0; i < (TEST_LEN); i++)
		command->elems[0] = (char *) (TEST_VALUE);

	assert(1 == add_command(command, TEST_STR, TEST_STR_LEN));
	assert(0 == strncmp(command->elems[0], TEST_STR, TEST_STR_LEN));
	assert(1 == command->cur_len);

	free_command(command);
}

void test_split_command()
{
	char *command_line = (TEST_COM_STR_1);
	char *results1[] = TEST_COM_SPLIT_1;
	int len = (TEST_COM_STR_LEN_1);

	command_s *command = split_command(command_line, len);

	assert(NULL != command);

	assert(TEST_COM_NUM_1 == command->cur_len);

	for (int i = 0; i < command->cur_len; i++)
		assert(0 == strncmp(command->elems[i], results1[i],
					strlen(results1[i])));

	free_command(command);

	// second test
	command_line = (TEST_COM_STR_2);
	char *results2[] = TEST_COM_SPLIT_2;
	len = (TEST_COM_STR_LEN_2);

	command = split_command(command_line, len);

	assert(NULL != command);

	assert(TEST_COM_NUM_2 == command->cur_len);

	for (int i = 0; i < command->cur_len; i++)
		assert(0 == strncmp(command->elems[i], results2[i],
					strlen(results2[i])));

	free_command(command);

	// test with empty string
	command = split_command("", 0);

	assert(0 == command->cur_len);

	free_command(command);
}

int main()
{
	test_init_command();

	test_resize_command();

	test_add_command();

	test_split_command();

	return 0;
}

#endif /*_TEST*/
