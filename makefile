#makefile 

CC=gcc  #compiler
CFLAGS=-g -Wall
MAIN=main

all: compile_main

compile_main:
	$(CC) main.c $(CFLAGS) -o $(MAIN)
clean:
	rm $(main)