/* Author: Aidan Bush
 * Assign: Assign 1
 * Course: CMPT 360
 * Date: Oct. 4, 18
 * File: command.h
 * Description: command header file.
 */

#ifndef COMMAND_H
#define COMMAND_H

#include <stddef.h>

#define COMMAND_START_SIZE	4

// resizeable string array
typedef struct {
	char **elems;
	int max_len;
	int cur_len;
} str_arr_s;

// prints the given str_arr_s putting spaces between elements
void print_str_arr(str_arr_s *arr);

// frees the given str_arr_s struct pointer and its contents
void free_str_arr(str_arr_s *arr);

// initializes and allocates space for a str_arr_s struct
str_arr_s *init_str_arr(int len);

// adds a string to a str_arr_s struct
int add_str_arr(str_arr_s *arr, char *str, size_t len);

// gets the next command from stdin and parses into a str_arr_s struct
str_arr_s *get_command();

#endif /* COMMAND_H */
