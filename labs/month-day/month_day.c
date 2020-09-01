#include <stdio.h>
#include <stdlib.h>
static char daytab[2][13] = {{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}, {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};
static char *name[] = {
		"Ilegal Month",
		"January", "February", "March",
		"April", "May", "June",
		"July", "August", "September",
		"October", "November", "December"
	};
void month_day(int year, int yearday, int *pmonth, int *pday)
{
    int bisiest  = year%4 == 0 && year%100 != 0 || year%400 == 0;
    //printf("%d\n", bisiest);
    if( (bisiest==1 && yearday>366) || (bisiest==0 && yearday>365) ){
        *pmonth, *pday =0;
        return ;
    }
    for (size_t i = 1; i < 13; i++)
    {
        if(yearday> daytab[bisiest][i]){
            yearday=yearday-(int)daytab[bisiest][i];
            *pmonth=i;
        }

        
    }
    *pday=yearday;
    
}
int main(int argc, char *argv[])
{
    if (argc == 3)
    {
        int year = atoi(argv[1]);
        int yearday = atoi(argv[2]);
        int pmonth=0;
        int pday=0;
        if (year > 0 && yearday > 0)
        {
            month_day(year, yearday,&pmonth,&pday);
            if(pmonth==0 && pday==0){
                printf("Not valid day or year\n");
            }else
            {
                printf("%s %d, %d\n",name[pmonth] ,pday, year);
            }
            
            
        }
        else
        {
            printf("Invalid inputs\n");
        }
    }

    return 0;
}