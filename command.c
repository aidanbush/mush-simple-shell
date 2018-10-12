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

// takes a command pointer and doubles the size + 1 of its elem array
// returns success or failure
static int resize_str_arr(str_arr_s *command)
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

int add_str_arr(str_arr_s *command, char *str, size_t len)
{
	if (NULL == command)
		return 0;

	char *new_str = strndup(str, len);

	if (command->cur_len >= command->max_len)
		if (!resize_str_arr(command))
			return 0;

	command->elems[command->cur_len] = new_str;
	command->cur_len++;

	return 1;
}

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
	if (NULL == command)
		return 0;

	char *env;

	// if environment variable replace
	if (1 < len && '$' == str[0]) {
		env = getenv_len(str + 1, len - 1);

		if (NULL != env) {
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

void free_str_arr(str_arr_s *command)
{
	for (int i = 0; i < command->cur_len; i++)
		free(command->elems[i]);

	free(command->elems);
	free(command);
}

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

	if (!isspace(line[start]) && start < len){
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

str_arr_s *get_command(char **str)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	str_arr_s *command = NULL;

	read = getline(&line, &len, stdin);
	if (-1 == read) {
		free(line);
		*str = NULL;
		return NULL;
	}

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

void test_init_str_arr()
{
	// test create valid command
	str_arr_s *command = init_str_arr((TEST_LEN));

	assert(NULL != command);
	assert((TEST_LEN) == command->max_len);
	assert(0 == command->cur_len);
	assert(NULL != command->elems);

	for (int i = 0; i < (TEST_LEN); i++)
		assert(NULL == command->elems[i]);

	free_str_arr(command);

	// test non accepted values
	assert(NULL == init_str_arr(-1));
	assert(NULL == init_str_arr(0));
}

void test_resize_str_arr()
{
	str_arr_s *command = init_str_arr((TEST_LEN));
	for (int i = 0; i < (TEST_LEN); i++)
		command->elems[0] = (char *) (TEST_VALUE);

	assert(1 == resize_str_arr(command));
	assert((TEST_LEN) < command->max_len);

	for (int i = 0; i < (TEST_LEN); i++)
		assert((char *) (TEST_VALUE) == command->elems[0]);

	for (int i = (TEST_LEN); i < command->max_len; i++)
		assert(NULL == command->elems[i]);

	free_str_arr(command);

	// test with NULL command
	assert(0 == resize_str_arr(NULL));
}

void test_add_str_arr()
{
	str_arr_s *command = init_str_arr((TEST_LEN));
	for (int i = 0; i < (TEST_LEN); i++)
		command->elems[0] = (char *) (TEST_VALUE);

	assert(1 == add_str_arr(command, TEST_STR, TEST_STR_LEN));
	assert(0 == strncmp(command->elems[0], TEST_STR, TEST_STR_LEN));
	assert(1 == command->cur_len);

	free_str_arr(command);
}

void test_split_command()
{
	char *command_line = (TEST_COM_STR_1);
	char *results1[] = TEST_COM_SPLIT_1;
	int len = (TEST_COM_STR_LEN_1);

	str_arr_s *command = split_command(command_line, len);

	assert(NULL != command);

	assert(TEST_COM_NUM_1 == command->cur_len);

	for (int i = 0; i < command->cur_len; i++)
		assert(0 == strncmp(command->elems[i], results1[i],
					strlen(results1[i])));

	free_str_arr(command);

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

	free_str_arr(command);

	// third test
	command_line = (TEST_COM_STR_3);
	len = (TEST_COM_STR_LEN_3);

	command = split_command(command_line, len);

	assert(NULL != command);

	assert(TEST_COM_NUM_3 == command->cur_len);

	free_str_arr(command);

	// test with empty string
	command = split_command("", 0);

	assert(0 == command->cur_len);

	free_str_arr(command);
}

int main()
{
	test_init_str_arr();

	test_resize_str_arr();

	test_add_str_arr();

	test_split_command();

	return 0;
}

#endif /*_TEST*/
