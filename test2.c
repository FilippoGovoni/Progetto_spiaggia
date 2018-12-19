#include<stdio.h>
#include<stdlib.h>
#include<string.h>


int main(void)
{
    int i,controllo=0;
    char test[100]={0};
    printf("inserisci BOOK\n");
    fgets(test,20,stdin);

    if(test[4]==0 || test[5]==32 || test[4]==32)
    printf("1\n");
    else
    printf("2\n");
    
   

    return 0;
}