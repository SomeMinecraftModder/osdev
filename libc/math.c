#include "math.h"

int abs(int in) {
    // If input is less than 0, return the opposite of the input, otherwise return input
    if (in < 0) {
        return -in;
    } else {
        return in;
    }
}

long labs(long in) {
    if (in < 0) {
        return -in;
    } else {
        return in;
    }
}

long long llabs(long long in) {
    if (in < 0) {
        return -in;
    } else {
        return in;
    }
}
