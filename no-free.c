#include <stdio.h>



int main() {
    char *a = NULL;
    char *b = NULL;

    a = foo("Hi there, Chris");
    free(a);
    b = foo("Goodbye");
    free(b);
    printf("From main: %s %s\n", a, b);
    return 0;
}

char *foo(char *p) {
    char *q = (char *)malloc(strlen(p)+1);
    strcpy(q, p);
    printf("From foo: the string is %s\n", q);
    return q;
}

