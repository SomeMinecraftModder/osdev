#ifndef MATH_H
#define MATH_H

#define ROUND_UP(N, S) ((((N) + (S) - 1) / (S)) * (S))
#define ROUND_DOWN(N, S) (((N) / (S)) * (S))

int max(int a, int b);
int min(int a, int b);
int sign(int x);
int abs(int a);

#endif
