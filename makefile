CC = cc
CFLAGS = -Wall -Werror -pedantic -g -std=c99

all: download

download: url.c download.c
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f download
