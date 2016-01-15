//
// Created by joelm on 2016-01-13.
//

#ifndef PROJECT_RATIONALNUMBERS_H
#define PROJECT_RATIONALNUMBERS_H

struct node {
    int depth;
    int inset;
};
#define Node struct node

struct fraction {
    int numerator;
    int denominator;
};
#define Fraction struct fraction

void printFraction(Fraction f);
void printRational(int index);
Fraction calculateNode(int index);
Fraction calculateNodeRecursive(int width, int relativeInset, Fraction value);
Node locationOfNode(int node);

#endif //PROJECT_RATIONALNUMBERS_H
