CC = cc
CFLAGS = -Wall -Werror -pedantic -g -O3

all: download

download: connection.c download.c
	$(CC) $(CFLAGS) $^ -o $@

DOC = ./doc
docs:
	pandoc -V lang=pt -V geometry:margin=1in -V fontsize=11pt --toc README.md -s -o $(DOC)/README.pdf

.PHONY: clean
clean:
	rm -f download
