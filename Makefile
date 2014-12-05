#
#
CC=g++
#
# 
#
CFLAGS= -W -Wall -Werror -g


all: rshell ls cp 
	

rshell: src/main.cpp
	([ ! -d bin ] && mkdir bin) || [ -d bin ]
	$(CC) $(CFLAGS) src/main.cpp -o bin/rshell

ls: src/ls.cpp
	([ ! -d bin ] && mkdir bin) || [ -d bin ]
	$(CC) $(CFLAGS) src/ls.cpp -o bin/ls

cp: src/cp.cpp
	([ ! -d bin ] && mkdir bin) || [ -d bin ]
	$(CC) $(CFLAGS) src/cp.cpp -o bin/cp


clean: 
	rm -rf *.o
