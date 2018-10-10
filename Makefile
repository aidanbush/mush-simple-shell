# Author: Aidan Bush
# Assign: Assign 1
# Course: CMPT 360
# Date: Oct. 4, 18
# File: Makefile
# Description: it's a Makefile

CC=gcc
CFLAGS= -Wall -std=c99 -Wextra -g -pedantic -D_POSIX_C_SOURCE=200908L

.PHONY: all test clean

all: mush

mush: mush.o command.o

mush.o: mush.c command.h

command.o: command.c command.h

# tests
CFLAGS+=-D_TEST

test: command

command: command.o

clean:
	$(RM) command mush *.o
