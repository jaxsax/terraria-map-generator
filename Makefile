
BINDIR := bin

CC=clang
CFLAGS=-Weverything

$(BINDIR)/main: main.c | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $<

$(BINDIR):
	mkdir $(BINDIR)
