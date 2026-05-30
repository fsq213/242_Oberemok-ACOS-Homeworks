#include <stdlib.h>

void bill(char *);
void fred(int);
void john(double);
void sam(double, double);

int main()
{
    printf("\n=== Starting Program ===\n\n");
    
    fred(42);
    john(16.0);
    bill("Hello World!");
    sam(2.0, 3.0);
    
    printf("\n=== Program Complete ===\n");
    exit(0);
}