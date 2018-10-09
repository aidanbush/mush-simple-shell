/* Author: Aidan Bush
 * Assign: Assign 1
 * Course: CMPT 360
 * Date: Oct. 4, 18
 * File: command.h
 * Description:
 */

#ifndef COMMAND_H
#define COMMAND_H

typedef struct {
	char **elems;
	int max_len;
	int cur_len;
} command_s;

command_s *get_command();

#endif /* COMMAND_H */
