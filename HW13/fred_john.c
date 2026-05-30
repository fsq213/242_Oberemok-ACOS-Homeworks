#include <stdio.h>
#include <math.h>

void fred(int arg)
{
    printf("INTERPOSED: fred called with arg=%d\n", arg);
    printf("fred: you passed %d (interposed version)\n", arg);
}

void john(double arg)
{
    printf("INTERPOSED: john called with arg=%.2f\n", arg);
    double result = sqrt(arg);
    printf("john: sqrt(%.2f) = %.2f (interposed version)\n", arg, result);
}