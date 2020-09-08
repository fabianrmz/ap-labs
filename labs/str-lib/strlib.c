
#include <string.h>
#include <stdio.h>
extern char newString[150];

int mystrlen(char *str)
{
    int i = 0;
    int count = 0;
    while (str[i] != '\0')
    {
        i++;
        count++;
    }
    return count;
}

char *mystradd(char *origin, char *addition)
{
    int i = 0;
    while (origin[i] != '\0')
    {
        newString[i] = origin[i];
        i++;
    }
    int j = 0;
    while (addition[j] != '\0')
    {
        newString[i] = addition[j];
        j++;
        i++;
    }
    printf("New String          : %s\n", newString);
}

int mystrfind(char *origin, char *substr)
{
    int M = mystrlen(substr);
    int N = mystrlen(origin);
    for (int i = 0; i <= N - M; i++)
    {
        int j;
        for (j = 0; j < M; j++)
        {
            if (origin[i + j] != substr[j])
            {
                break;
            }
        }
        if (j == M)
        {
            return i;
        }
    }
    return -1;
}
