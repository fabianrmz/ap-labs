#include <stdio.h>

int main(){
    // Place your magic here
     char c;
    char word[50];
    int count = 0;
    while((c = getchar()) != EOF){
        if(c=='\n') {
            count--;
            while(count>=0) {
                printf("%c", word[count--]);
            }
            count = 0;
            printf("\n");
        }
        else {
            word[count++]=c;
        }
    }    
    return 0;
}
