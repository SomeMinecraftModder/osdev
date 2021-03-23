#ifndef MATH_H
#define MATH_H

#define ROUND_UP(N, S) ((((N) + (S) - 1) / (S)) * (S))
#define ROUND_DOWN(N, S) (((N) / (S)) * (S))

int abs(int in);
long labs(long in);
long long llabs(long long in);

#endif
