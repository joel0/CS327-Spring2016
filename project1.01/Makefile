CC=gcc
SDIR=src
ODIR=obj
EXEDIR=bin
CFLAGS=-g -Wall -Werror

all: environment $(ODIR)/main.o
	$(CC) -o $(EXEDIR)/main $(ODIR)/main.o $(CFLAGS)

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) -o $@ $^ -c $(CFLAGS)

environment:
	mkdir -p bin obj

clean:
	rm -f $(EXEDIR)/* $(ODIR)/*.o
