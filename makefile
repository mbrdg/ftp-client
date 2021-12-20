CC = cc
CFLAGS = -Wall -Werror -pedantic -g

all: download

download: connection.c download.c
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f download
