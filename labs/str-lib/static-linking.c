#include <stdio.h>
char newString[150];
int main(int argc, char *argv[])
{

    if (argc == 4)
    {
        
        printf("Initial Lenght      : %i\n", mystrlen(argv[1]));
        mystradd(argv[1], argv[2]);
        if(mystrfind(newString, argv[3])!= -1){
            printf("Substring was found:      : yes\n");
        }else{
            printf("Substring was found:      : no\n");
        }
        
        return 0;
    }
}
