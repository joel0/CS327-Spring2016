//
// Created by joelm on 2016-01-13.
//

#ifndef PROJECT_RATIONALNUMBERS_H
#define PROJECT_RATIONALNUMBERS_H

struct fraction {
    int numerator;
    int denominator;
};
#define Fraction struct fraction

void printRationals(int count);
void printNode(int node, Fraction* tree);
void calculateRationalsRecursive(int count, int numerator, int denominator, int node, Fraction* tree);
int calculateLeftChildren(int count);
int calculateRightChildren(int count);
int leftChild(int n);
int rightChild(int n);

#endif //PROJECT_RATIONALNUMBERS_H
