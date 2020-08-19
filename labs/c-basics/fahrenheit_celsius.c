#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define   LOWER  0       /* lower limit of table */
#define   UPPER  300     /* upper limit */
#define   STEP   20      /* step size */

/* print Fahrenheit-Celsius table */

int main(int argc, char *argv[])
{

    
    
    
    int count = 0; 
    while(argv[++count] != NULL);
    
    if(count==2){
       
        char *a = argv[1];
        int num = atoi(a);
        int celcius = (5.0/9.0)*(num-32);
        printf("Fahrenheit: %d, Celcius: %6.1f\n", num, (double)celcius);
    }else if(count==4){
       
        
        char *start1 = argv[1];
        int start = atoi(start1);

        char *end1 = argv[2];
        int end = atoi(end1);

        char *increment1 = argv[3];
        int increment = atoi(increment1);

        int fahr;
        for (fahr = start; fahr <= end; fahr = fahr + increment){
            printf("Fahrenheit: %3d, Celcius: %6.1f\n", fahr, (5.0/9.0)*(fahr-32));
        }
    }
    
    

    
    return 0;
}