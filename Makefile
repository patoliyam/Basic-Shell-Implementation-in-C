# Group 29 C Shell Makefile

CC = gcc
CFLAGS  = -Wall -g
OBJ = group-29-c-shell.o

all: group-29-c-shell

group-29-c-shell : $(OBJ)
	$(CC) $(CFLAGS) -o group-29-c-shell $(OBJ) 

%.o: %.c
	$(CC) $(CFLAGS) -c $<
