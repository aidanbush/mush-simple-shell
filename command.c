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
#include <errno.h>

/* project includes */
#include "command.h"
#ifndef _TEST
#include "memwatch.h"
#endif

// takes a command pointer and doubles the size + 1 of its elem array
// returns success or failure
static int resize_str_arr(str_arr_s *command)
{
	if (command == NULL)
		return 0;

	int new_len = (command->max_len + 1) * 2;

	char **tmp = realloc(command->elems, sizeof(char *) * new_len);

	if (tmp == NULL)
		return 0;

	// null remaining values
	for (int i = command->max_len; i < new_len; i++)
		tmp[i] = NULL;

	command->elems = tmp;
	command->max_len = new_len;

	return 1;
}

// Takes a str_arr_s and adds a new string of length len to it, resizing if
// required.
// returns 1 on success otherwise 0
int add_str_arr(str_arr_s *command, char *str, size_t len)
{
	if (command == NULL)
		return 0;

	char *new_str = strndup(str, len);

	if (command->cur_len >= command->max_len)
		if (!resize_str_arr(command))
			return 0;

	command->elems[command->cur_len] = new_str;
	command->cur_len++;

	return 1;
}

// checks for if the string str of len length is an environment variable,
// returning that variables value if it exists, otherwise NULL
static char *getenv_len(char *str, size_t len)
{
	char *env;
	char c;

	c = str[len];
	str[len] = '\0';

	env = getenv(str);

	str[len] = c;

	return env;
}

static int add_command(str_arr_s *command, char *str, size_t len)
{
	if (command == NULL)
		return 0;

	char *env;

	// if environment variable replace
	if (len > 1 && '$' == str[0]) {
		env = getenv_len(str + 1, len - 1);

		if (env != NULL) {
			str = env;
			len = strlen(env);
		}
	}

	return add_str_arr(command, str, len);
}

str_arr_s *init_str_arr(int len)
{
	if (len <= 0)
		return NULL;

	str_arr_s *command = calloc(sizeof(str_arr_s), 1);

	if (command == NULL)
		return NULL;

	command->elems = malloc(sizeof(char *) * len);
	if (command->elems == NULL)
		return NULL;

	for (int i = 0; i < len; i++)
		command->elems[i] = NULL;

	command->max_len = len;
	command->cur_len = 0;

	return command;
}

void free_str_arr(str_arr_s *command)
{
	for (int i = 0; i < command->cur_len; i++)
		free(command->elems[i]);

	free(command->elems);
	free(command);
}

// TODO add escape characters
static str_arr_s *split_command(char *line, size_t len)
{
	str_arr_s *command = init_str_arr(COMMAND_START_SIZE);
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
				free_str_arr(command);
				return NULL;
			}
			// skip space after
			while (isspace(line[i]) && i < len)
				i++;

			start = i;
		}
		i++;
	}

	if (!isspace(line[start]) && start < len) {
		if (!add_command(command, line + start, i - start)) {
			free_str_arr(command);
			return NULL;
		}
	}

	return command;
}

void print_str_arr(str_arr_s *command)
{
	printf("cur_len: %d\n", command->cur_len);
	printf("max_len: %d\n", command->max_len);
	for (int i = 0; i < command->cur_len; i++)
		printf("|%s| ", command->elems[i]);
	printf("\n");
}

str_arr_s *get_command(char **str, int *eof)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	str_arr_s *command = NULL;

	*eof = 0;

	int old_errno = errno;

	errno = 0;

	read = getline(&line, &len, stdin);
	if (-1 == read) {
		free(line);
		*str = NULL;

		if (errno != EINTR)
			*eof = 1;
		else
			clearerr(stdin);

		errno = old_errno;
		return NULL;
	}
	errno = old_errno;

	// split line
	command = split_command(line, read);
	if (str == NULL)
		free(line);
	else
		*str = line;

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
// checkpatch complains about this test case
#define TEST_COM_STR_2		"  	messy  command 	line 	 "
#define TEST_COM_STR_3		"\n"

#define TEST_COM_SPLIT_1	{"mv", "1", "2", "3", "4", "5", "6", "7", "8", \
				"9", "0", "dir"}
#define TEST_COM_SPLIT_2	{"messy", "command", "line"}

#define TEST_COM_STR_LEN_1	strlen((TEST_COM_STR_1))
#define TEST_COM_STR_LEN_2	strlen((TEST_COM_STR_2))
#define TEST_COM_STR_LEN_3	strlen((TEST_COM_STR_3))

#define TEST_COM_NUM_1		12
#define TEST_COM_NUM_2		3
#define TEST_COM_NUM_3		0

void test_init_str_arr(void)
{
	// test create valid command
	str_arr_s *command = init_str_arr((TEST_LEN));

	assert(command != NULL);
	assert((TEST_LEN) == command->max_len);
	assert(command->cur_len == 0);
	assert(command->elems != NULL);

	for (int i = 0; i < (TEST_LEN); i++)
		assert(command->elems[i] == NULL);

	free_str_arr(command);

	// test non accepted values
	assert(init_str_arr(-1) == NULL);
	assert(init_str_arr(0) == NULL);
}

void test_resize_str_arr(void)
{
	str_arr_s *command = init_str_arr((TEST_LEN));

	for (int i = 0; i < (TEST_LEN); i++)
		command->elems[0] = (char *) (TEST_VALUE);

	assert(resize_str_arr(command) == 1);
	assert(command->max_len > (TEST_LEN));

	for (int i = 0; i < (TEST_LEN); i++)
		assert((char *) (TEST_VALUE) == command->elems[0]);

	for (int i = (TEST_LEN); i < command->max_len; i++)
		assert(command->elems[i] == NULL);

	free_str_arr(command);

	// test with NULL command
	assert(resize_str_arr(NULL) == 0);
}

void test_add_str_arr(void)
{
	str_arr_s *command = init_str_arr((TEST_LEN));

	for (int i = 0; i < (TEST_LEN); i++)
		command->elems[0] = (char *) (TEST_VALUE);

	assert(add_str_arr(command, TEST_STR, TEST_STR_LEN) == 1);
	assert(strncmp(command->elems[0], TEST_STR, TEST_STR_LEN) == 0);
	assert(command->cur_len == 1);

	free_str_arr(command);
}

void test_split_command(void)
{
	char *command_line = (TEST_COM_STR_1);
	char *results1[] = TEST_COM_SPLIT_1;
	int len = (TEST_COM_STR_LEN_1);

	str_arr_s *command = split_command(command_line, len);

	assert(command != NULL);

	assert(command->cur_len == (TEST_COM_NUM_1));

	for (int i = 0; i < command->cur_len; i++)
		assert(strncmp(command->elems[i], results1[i],
			strlen(results1[i])) == 0);

	free_str_arr(command);

	// second test
	command_line = (TEST_COM_STR_2);
	char *results2[] = TEST_COM_SPLIT_2;

	len = (TEST_COM_STR_LEN_2);

	command = split_command(command_line, len);

	assert(command != NULL);

	assert(command->cur_len == (TEST_COM_NUM_2));

	for (int i = 0; i < command->cur_len; i++)
		assert(strncmp(command->elems[i], results2[i],
			strlen(results2[i])) == 0);

	free_str_arr(command);

	// third test
	command_line = (TEST_COM_STR_3);
	len = (TEST_COM_STR_LEN_3);

	command = split_command(command_line, len);

	assert(command != NULL);

	assert(command->cur_len == (TEST_COM_NUM_3));

	free_str_arr(command);

	// test with empty string
	command = split_command("", 0);

	assert(command->cur_len == 0);

	free_str_arr(command);
}

int main(void)
{
	test_init_str_arr();

	test_resize_str_arr();

	test_add_str_arr();

	test_split_command();

	return 0;
}

#endif /*_TEST*/
