#
#
CC=g++
#
# 
#
CFLAGS= -W -Wall -Werror -ansi -pedantic


all: rshell
	

rshell: src/main.cpp
	([ ! -d bin/ ] && mkdir bin) || [ -d bin ]
	$(CC) $(CFLAGS) src/main.cpp -o bin/rshell

clean: 
	rm -rf *.o
