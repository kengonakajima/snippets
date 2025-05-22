CC = gcc
CFLAGS = $(shell pkg-config --cflags libproxy-1.0)
LDFLAGS = $(shell pkg-config --libs libproxy-1.0)

all: proxy_finder

proxy_finder: proxy_finder.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f proxy_finder

.PHONY: all clean