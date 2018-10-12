/* Author: Aidan Bush
 * Assign: Assign 1
 * Course: CMPT 360
 * Date: Oct. 4, 18
 * File: command.h
 * Description:
 */

#ifndef COMMAND_H
#define COMMAND_H

#include <stddef.h>

#define COMMAND_START_SIZE	4

typedef struct {
	char **elems;
	int max_len;
	int cur_len;
} str_arr_s;

// prints the given command putting spaces between elements
void print_str_arr(str_arr_s *);

void free_str_arr(str_arr_s *);

str_arr_s *init_str_arr(int len);

int add_str_arr(str_arr_s *, char *, size_t);

// gets the next command from stdin and parses into command struct
str_arr_s *get_command();

#endif /* COMMAND_H */
