#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printArray(char *arrayList[], int arraySize);
void mergeSort(char *arrayList[], int leftSide, int rightSide, int numberSign);
void mergeSortMain(char *arrayList[], int lRef, int middle, int rRef, int (*comp)(void *, void *));
int numberComoparassion(char *, char *);

int main(int argc, char *argv[])
{
    char *fileToRead;
    int numTest = 0;
    if (argc == 2)
    {
        fileToRead = argv[1];
    }
    else if (argc == 3)
    {
        fileToRead = argv[2];
        numTest = 1;
    }
    else
    {
        printf("Incomplete!, missing parameters\n");
        return 0;
    }

    int arraySize = 100;
    char **arrayList = malloc(sizeof(char *) * arraySize);
    //

    FILE *fp = fopen(fileToRead, "r");
    if (!fp)
    {
        printf("Error, Failed to open the file, are you sure %s exists?\n", fileToRead);
        return 0;
    }
    printf("File %s opened succesfully \n", fileToRead);
    char *line_buf = NULL;
    size_t line_buf_size = 0;
    int line_count = 0;
    ssize_t line_size;

    line_size = getline(&line_buf, &line_buf_size, fp);
    while (line_size >= 0)
    {

        arrayList[line_count] = malloc(sizeof(char) * line_buf_size);

        sprintf(arrayList[line_count], "%s", line_buf);
        arrayList[line_count][strlen(line_buf) - 1] = 0;
        //printf("%s \n",arrayList[line_count]);

        line_count++;
        line_size = getline(&line_buf, &line_buf_size, fp);
    }
    free(line_buf);
    fclose(fp);

    printf("Antes: \n");
    printArray(arrayList, arraySize);

    mergeSort(arrayList, 0, arraySize - 1, numTest);

    printf("\nDespues: \n");
    printArray(arrayList, arraySize);

    return 0;
}

void printArray(char *arrayList[], int arraySize)
{
    //printing zone
    printf("Array : ");
    printf("[");
    for (int i = 0; i < arraySize; i++)
    {
        if (i == arraySize - 1)
        {
            printf("%s] \n", arrayList[i]);
        }
        else
        {
            printf("%s, ", arrayList[i]);
        }
    }
    return;
}

int numberComoparassion(char *d1, char *d2)
{
    double x1, x2;
    x1 = atof(d1);
    x2 = atof(d2);
    if (x1 < x2)
    {
        return -1;
    }
    else if (x1 > x2)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
void mergeSort(char *arrayList[], int leftSide, int rightSide, int numberSign)
{
    if (leftSide < rightSide)
    {
        int mid = leftSide + (rightSide - leftSide) / 2;
        mergeSort(arrayList, leftSide, mid, numberSign);
        mergeSort(arrayList, mid + 1, rightSide, numberSign);
        mergeSortMain(arrayList, leftSide, mid, rightSide, (int (*)(void *, void *))(numberSign ? numberComoparassion : strcmp));
    }
}

void mergeSortMain(char *arrayList[], int lRef, int middle, int rRef, int (*comp)(void *, void *))
{
    int i, j, k;
    int leftSize = middle - lRef + 1;
    int rightSize = rRef - middle;

    char *L[leftSize];
    char *R[rightSize];

    for (i = 0; i < leftSize; i++)
        L[i] = arrayList[lRef + i];
    for (j = 0; j < rightSize; j++)
        R[j] = arrayList[middle + 1 + j];

    i = 0;
    j = 0;
    k = lRef;
    while (i < leftSize && j < rightSize)
    {
        if ((*comp)(L[i], R[j]) <= 0)
        {
            arrayList[k] = L[i];
            i++;
        }
        else
        {
            arrayList[k] = R[j];
            j++;
        }
        k++;
    }
    while (i < leftSize)
    {
        arrayList[k] = L[i];
        i++;
        k++;
    }
    while (j < rightSize)
    {
        arrayList[k] = R[j];
        j++;
        k++;
    }
}

