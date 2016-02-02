CC=gcc
SDIR=src
ODIR=obj
EXEDIR=bin
CFLAGS=-g -Wall -Werror -std=gnu11 -lm

all: environment $(ODIR)/main.o
	$(CC) -o $(EXEDIR)/main $(ODIR)/main.o $(CFLAGS)

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) -o $@ $^ -c $(CFLAGS)

environment:
	mkdir -p bin obj

clean:
	rm -f $(EXEDIR)/* $(ODIR)/*.o may_joel.assignment-1.02.tar.gz

tar: clean
	cp -rf . ../may_joel.assignment-1.02/
	mv ../may_joel.assignment-1.02/ ./may_joel.assignment-1.02/
	tar cvfz may_joel.assignment-1.02.tar.gz may_joel.assignment-1.02/
	rm -rf may_joel.assignment-1.02
