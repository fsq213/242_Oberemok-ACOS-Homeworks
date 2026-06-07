CC = gcc
CFLAGS = -Wall -Wextra -std=c99

executer: executer.c
	$(CC) $(CFLAGS) -o executer executer.c

clean:
	rm -f executer

.PHONY: clean