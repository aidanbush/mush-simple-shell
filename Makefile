# Author: Aidan Bush
# Assign: Assign 1
# Course: CMPT 360
# Date: Oct. 4, 18
# File: Makefile
# Description: it's a Makefile

CC=gcc
CFLAGS= -Wall -std=c99 -Wextra -g -pedantic

.PHONY: all test clean

all: mush

mush: mush.o

mush.o: mush.c command.h

command.o: command.c command.h

clean:
	$(RM) mush *.o
