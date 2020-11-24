# Copyright (c) 2020 Geert Janssen, MIT License

INCLUDES =
CPPFLAGS = $(INCLUDES)
CFLAGS   = -O2 -g
LDFLAGS  =

.PHONY: all
all: words

words: words.o
words.o: words.c wordlist.h

.PHONY: clean
clean:
	-rm -f *.o words
