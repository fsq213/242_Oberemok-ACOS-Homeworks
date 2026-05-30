#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <math.h>

void bill(char *arg) {
    void (*original_bill)(char *) = dlsym(RTLD_NEXT, "bill");
    
    printf("LOAD-TIME INTERPOSED: bill called with arg=\"%s\"\n", arg);
    
    if (original_bill) {
        original_bill(arg);
    } else {
        printf("bill: you passed %s (fallback)\n", arg);
    }
}

void sam(double arg1, double arg2) {
    void (*original_sam)(double, double) = dlsym(RTLD_NEXT, "sam");
    
    printf("LOAD-TIME INTERPOSED: sam called with arg1=%.2f, arg2=%.2f\n", arg1, arg2);
    
    if (original_sam) {
        original_sam(arg1, arg2);
    } else {
        double result = pow(arg1, arg2);
        printf("sam: %.2f ^ %.2f = %.2f (fallback)\n", arg1, arg2, result);
    }
}