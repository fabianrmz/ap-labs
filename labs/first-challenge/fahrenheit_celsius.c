#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define   LOWER  0       /* lower limit of table */
#define   UPPER  300     /* upper limit */
#define   STEP   20      /* step size */

/* print Fahrenheit-Celsius table */

int main(int argc, char *argv[])
{
    
    
    
    int fahr;
    for (fahr = LOWER; fahr <= UPPER; fahr = fahr + STEP){
        printf("Fahrenheit: %3d, Celcius: %6.1f\n", fahr, (5.0/9.0)*(fahr-32));
    }
    
    printf("-----------------------------\nInput:\n");
    char *a = argv[1];
    int num = atoi(a);
    int celcius = (5.0/9.0)*(num-32);
    printf("Fahrenheit: %d, Celcius: %6.1f\n", num, (double)celcius);
    
    return 0;
}