#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <dlfcn.h>

void fred(int arg)
{
    printf("fred: you passed %d\n", arg);
}

void john(double arg)
{
    double result = sqrt(arg);
    printf("john: sqrt(%.2f) = %.2f\n", arg, result);
}

void bill(char *arg)
{
    printf("bill: you passed %s\n", arg);
}

void sam(double arg1, double arg2)
{
    double result = pow(arg1, arg2);
    printf("sam: %.2f ^ %.2f = %.2f\n", arg1, arg2, result);
}