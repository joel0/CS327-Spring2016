/*
 * Created by Joel May on 2016-01-12
 *
 * This program calculates the hyperbinary sequence of integers.
 */

#include <stdio.h>
#include <stdlib.h>
#include "hyperbinary.h"

int main(char* args[]) {
    printHyperbinary(100000000);
    return 0;
}

/*
 * count:   the number of elements in the hyperbinary sequence to print
 *
 * Prints the hyperbinary sequence with one element per line.
 */
void printHyperbinary(int count) {
    // The stored cache should be the first half sequence, including the middle element for odd sized sequences.
    int cacheSize = (count / 2) + (count % 2);
    int* hbSequence = malloc(sizeof(int) * cacheSize);
    int i;
    for (i = 0; i < cacheSize; i++) {
        hbSequence[i] = hyperBinary(i, hbSequence);
        printf("%d\n", hbSequence[i]);
    }
    for (i = cacheSize; i < count; i++) {
        printf("%d\n", hyperBinary(i, hbSequence));
    }
}

/*
 * n:               the element from the hyperbinary sequence to calculate
 * priorElements:   the array of previously calculated elements
 * return:          the value of the hyperbinary sequence
 *
 * The priorElements array must be filled up to at least n/2.
 */
int hyperBinary(int n, int* priorElements) {
    // Hard coded case
    if (n == 0) {
        return 1;
    }

    // Cases that depend on previous results
    if (n % 2 == 0) {
        return priorElements[(n / 2) - 1] + priorElements[n / 2];
    }
    return priorElements[(n - 1) / 2];
}