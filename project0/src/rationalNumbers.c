/*
 * Created by Joel May on 2016-01-13.
 *
 * This program calculates the specified number of rational numbers.
 */

#include <stdio.h>
#include <stdlib.h>
#include "rationalNumbers.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Please provide a count of numbers to calculate (n).\n");
        return -1;
    }
    int count;
    count = atoi(argv[1]);
    printRationals(count);
    return 0;
}

/*
 * count:   the number of rational numbers to print
 *
 * Prints the desired number of rational numbers, each on a separate line.
 */
void printRationals(int count) {
    Fraction* tree;
    tree = malloc(sizeof(Fraction) * count);
    calculateRationalsRecursive(count, 1, 1, 1, tree);
    for (int i = 0; i < count; i++) {
        printNode(i, tree);
    }
}

/*
 * node:    the node in the provided tree to print
 * tree:    the tree (stored in an array) containing the node to print
 *
 * Prints the desired node from the tree.
 */
void printNode(int node, Fraction* tree) {
    printf("%d/%d\n", tree[node].numerator, tree[node].denominator);
}

/*
 * count:       the number of elements (including this one) to add to the tree
 * numerator:   the value of this node's numerator
 * denominator: the value of this node's denominator
 * node:        the 1-based node in the tree to populate along with its children
 * tree:        the tree where the values of this node and its children should be stored
 *
 * Recursively calculates the rational numbers and stores them in the tree.
 */
void calculateRationalsRecursive(int count, int numerator, int denominator, int node, Fraction* tree) {
    int leftChildren = calculateLeftChildren(count);
    int rightChildren = calculateRightChildren(count);

    tree[node - 1].numerator = numerator;
    tree[node - 1].denominator = denominator;
    if (leftChildren > 0) {
        calculateRationalsRecursive(leftChildren, numerator, numerator + denominator, leftChild(node), tree);
    }
    if (rightChildren > 0) {
        calculateRationalsRecursive(rightChildren, numerator + denominator, denominator, rightChild(node), tree);
    }
}

/*
 * count:   the number of total children (including the current node)
 *
 * Calculates how many nodes belong in the left tree as if we were doing an in-order traversal.
 */
int calculateLeftChildren(int count) {
    if (count == 1) {
        // No children, only the node itself
        return 0;
    }
    int width = 1;
    int remainingChildren = count;
    while (remainingChildren > width) {
        remainingChildren -= width;
        width += width;
    }
    // Evenly split the nodes that saturate the upper levels.
    int leftChildren = ((count - remainingChildren) - 1) / 2;
    // Add the left side of the partial level of nodes.
    leftChildren += (remainingChildren < width / 2) ? remainingChildren : (width / 2);
    return leftChildren;
}

/*
 * count:   the number of total children (including the current node)
 *
 * Calculates how many nodes belong in the right tree as if we were doing an in-order traversal.
 */
int calculateRightChildren(int count) {
    if (count == 1) {
        // No children, only the node itself
        return 0;
    }
    int width = 1;
    int remainingChildren = count;
    while (remainingChildren > width) {
        remainingChildren -= width;
        width += width;
    }
    // Evenly split the nodes that saturate the upper levels.
    int rightChildren = ((count - remainingChildren) - 1) / 2;
    // Add the right side of the partial level of nodes.
    rightChildren += (remainingChildren > width / 2) ? (remainingChildren - (width / 2)) : 0;
    return rightChildren;
}


// Array tree traversal functions

/*
 * n:   index of the node to find the child of
 *
 * Finds the index of the left child of the specified node.  Note that all values are 1-based.
 */
int leftChild(int n) {
    return 2 * n;
}

/*
 * n:   index of the node to find the child of
 *
 * Finds the index of the right child of the specified node.  Note that all values are 1-based.
 */
int rightChild(int n) {
    return (2 * n) + 1;
}