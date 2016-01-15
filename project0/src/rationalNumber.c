/*
 * Created by Joel May on 2016-01-13.
 *
 * This program calculates the specified number of rational numbers.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rationalNumber.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: rationalNumber index\n    where index is the index of the node to print.\n");
        return -1;
    }
    int count;
    count = atoi(argv[1]);
    printRational(count);
    return 0;
}

/*
 * Prints the specified Fraction in the format of numerator/denominator with a trailing new line.
 */
void printFraction(Fraction f) {
    printf("%d/%d\n", f.numerator, f.denominator);
}

/*
 * Prints the rational number at the specified index.
 */
void printRational(int index) {
    Fraction rational = calculateNode(index);
    printFraction(rational);
}

/*
 * Uses a recursive method to calculate the rational number at the specified index.
 */
Fraction calculateNode(int index) {
    Node location;
    int width;
    Fraction rootFraction;

    // index is 0-based.  Node calculation is 1-based.
    location = locationOfNode(++index);
    rootFraction.numerator = 1;
    rootFraction.denominator = 1;
    width = (int)pow(2, location.depth - 1); // possible overflow

    return calculateNodeRecursive(width, location.inset, rootFraction);
}

/*
 * Recursively calculates the child node to find the target node.
 *
 * width:           the count of children the current node has in the depth of the target value
 * relativeInset:   the 1-based index of the target child from the leftmost descendant in its depth
 * value:           the current node's value
 */
Fraction calculateNodeRecursive(int width, int relativeInset, Fraction value) {
    // base case
    if (width == 1) {
        return value;
    }

    width /= 2;
    if (relativeInset > width) {
        // A descendant on the right side
        value.numerator += value.denominator; // (i + j) / j
        relativeInset -= width;
        return calculateNodeRecursive(width, relativeInset, value);
    }
    // A descendant of the left side
    value.denominator += value.numerator; // i / (i + j)
    return calculateNodeRecursive(width, relativeInset, value);
}

/*
 * Calculates the node's depth and inset.  Note that the returned values are 1-based (i.e. the root is (1, 1)).
 *
 * node:    the 1-based index of the value to locate
 */
Node locationOfNode(int node) {
    Node location;
    location.depth = 1;
    int width = 1;
    if (node == 1) {
        location.inset = 1;
        return location;
    }
    while (node > width) {
        node -= width;
        width += width;
        location.depth++;
    }
    location.inset = node;
    return location;
}

// OLD CODE BELOW
// It calculates and prints every node to the specified node.

//
///*
// * count:   the number of rational numbers to print
// *
// * Prints the desired number of rational numbers, each on a separate line.
// */
//void printRationals(int count) {
//    Fraction* tree;
//    tree = malloc(sizeof(Fraction) * count);
//    calculateRationalsRecursive(count, 1, 1, 1, tree);
//    for (int i = 0; i < count; i++) {
//        printNode(i, tree);
//    }
//}
//
///*
// * node:    the node in the provided tree to print
// * tree:    the tree (stored in an array) containing the node to print
// *
// * Prints the desired node from the tree.
// */
//void printNode(int node, Fraction* tree) {
//    printf("%d/%d\n", tree[node].numerator, tree[node].denominator);
//}
//
///*
// * count:       the number of elements (including this one) to add to the tree
// * numerator:   the value of this node's numerator
// * denominator: the value of this node's denominator
// * node:        the 1-based node in the tree to populate along with its children
// * tree:        the tree where the values of this node and its children should be stored
// *
// * Recursively calculates the rational numbers and stores them in the tree.
// */
//void calculateRationalsRecursive(int count, int numerator, int denominator, int node, Fraction* tree) {
//    int leftChildren = calculateLeftChildren(count);
//    int rightChildren = calculateRightChildren(count);
//
//    tree[node - 1].numerator = numerator;
//    tree[node - 1].denominator = denominator;
//    if (leftChildren > 0) {
//        calculateRationalsRecursive(leftChildren, numerator, numerator + denominator, leftChild(node), tree);
//    }
//    if (rightChildren > 0) {
//        calculateRationalsRecursive(rightChildren, numerator + denominator, denominator, rightChild(node), tree);
//    }
//}
//
///*
// * count:   the number of total children (including the current node)
// *
// * Calculates how many nodes belong in the left tree as if we were doing an in-order traversal.
// */
//int calculateLeftChildren(int count) {
//    if (count == 1) {
//        // No children, only the node itself
//        return 0;
//    }
//    int width = 1;
//    int remainingChildren = count;
//    while (remainingChildren > width) {
//        remainingChildren -= width;
//        width += width;
//    }
//    // Evenly split the nodes that saturate the upper levels.
//    int leftChildren = ((count - remainingChildren) - 1) / 2;
//    // Add the left side of the partial level of nodes.
//    leftChildren += (remainingChildren < width / 2) ? remainingChildren : (width / 2);
//    return leftChildren;
//}
//
///*
// * count:   the number of total children (including the current node)
// *
// * Calculates how many nodes belong in the right tree as if we were doing an in-order traversal.
// */
//int calculateRightChildren(int count) {
//    if (count == 1) {
//        // No children, only the node itself
//        return 0;
//    }
//    int width = 1;
//    int remainingChildren = count;
//    while (remainingChildren > width) {
//        remainingChildren -= width;
//        width += width;
//    }
//    // Evenly split the nodes that saturate the upper levels.
//    int rightChildren = ((count - remainingChildren) - 1) / 2;
//    // Add the right side of the partial level of nodes.
//    rightChildren += (remainingChildren > width / 2) ? (remainingChildren - (width / 2)) : 0;
//    return rightChildren;
//}
//
//
//// Array tree traversal functions
//
///*
// * n:   index of the node to find the child of
// *
// * Finds the index of the left child of the specified node.  Note that all values are 1-based.
// */
//int leftChild(int n) {
//    return 2 * n;
//}
//
///*
// * n:   index of the node to find the child of
// *
// * Finds the index of the right child of the specified node.  Note that all values are 1-based.
// */
//int rightChild(int n) {
//    return (2 * n) + 1;
//}