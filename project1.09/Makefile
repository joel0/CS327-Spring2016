CC=gcc
CXX=g++
SDIR=src
ODIR=obj
EXEDIR=bin
CFLAGS=-g -Wall -Werror -std=gnu11
CXXFLAGS=-g -Wall -Werror -std=gnu++11
LDFLAGS = -lm -lcurses
TARNAME=may_joel.assignment-1.09
OFILES=$(ODIR)/main.o $(ODIR)/utils.o $(ODIR)/dungeon.o $(ODIR)/path.o $(ODIR)/heap.o $(ODIR)/monster.o $(ODIR)/movement.o $(ODIR)/turn.o $(ODIR)/screen.o $(ODIR)/dice_set.o $(ODIR)/monster_descrip.o $(ODIR)/item_descrip.o $(ODIR)/descrip.o $(ODIR)/item.o

all: environment $(OFILES)
	$(CXX) -o $(EXEDIR)/main $(OFILES) $(LDFLAGS)

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) -o $@ $^ -c $(CFLAGS)

$(ODIR)/%.o: $(SDIR)/%.cpp
	$(CXX) -o $@ $^ -c $(CXXFLAGS)

environment:
	mkdir -p bin obj

clean:
	rm -f $(EXEDIR)/* $(ODIR)/*.o $(TARNAME).tar.gz

tar: clean
	cp -rf . ../$(TARNAME)/
	mv ../$(TARNAME)/ ./$(TARNAME)/
	tar cvfz $(TARNAME).tar.gz $(TARNAME)/
	rm -rf $(TARNAME)
