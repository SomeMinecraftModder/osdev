#include "math.h"

int abs(int a) {
    // If input is less than 0, return the opposite of the input, otherwise
    // return input
    if (a < 0) {
        return -a;
    }
    return a;
}

int max(int a, int b) {
    if (a > b) {
        return a;
    }
    return b;
}

int min(int a, int b) {
    if (a > b) {
        return b;
    }
    return a;
}

int sign(int x) {
    if (x < 0) {
        return -1;
    } else if (x > 0) {
        return 1;
    }
    return 0;
}
