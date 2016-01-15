all: part1 part2

part1: src/hyperbinary.c
	gcc -g -Wall -Werror src/hyperbinary.c -o bin/hyperbinary -I include

part2: src/rationalNumbers.c
	gcc -g -Wall -Werror src/rationalNumber.c -o bin/rationalNumber -I include

clean:
	rm -r bin/*
