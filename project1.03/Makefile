CC=gcc
SDIR=src
ODIR=obj
EXEDIR=bin
CFLAGS=-g -Wall -Werror -std=gnu11 -lm
TARNAME=may_joel.assignment-1.03
OFILES=$(ODIR)/main.o $(ODIR)/dungeon.o $(ODIR)/path.o $(ODIR)/path.o $(ODIR)/binheap.o

all: environment $(OFILES)
	$(CC) -o $(EXEDIR)/main $(OFILES) $(CFLAGS)

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) -o $@ $^ -c $(CFLAGS)

environment:
	mkdir -p bin obj

clean:
	rm -f $(EXEDIR)/* $(ODIR)/*.o $(TARNAME).tar.gz

tar: clean
	cp -rf . ../$(TARNAME)/
	mv ../$(TARNAME)/ ./$(TARNAME)/
	tar cvfz $(TARNAME).tar.gz $(TARNAME)/
	rm -rf $(TARNAME)
