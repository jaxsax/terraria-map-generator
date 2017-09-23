
BINDIR := bin

CC=clang
CFLAGS=-Weverything -Wno-padded --std=c11

$(BINDIR)/main: main.c main.h | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $<

$(BINDIR):
	mkdir -p $(BINDIR)
