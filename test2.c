#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// prova per git 455

int main(void)
{
    int i,controllo=0;
    char test[100]={0};
    printf("inserisci BOOK o CANCEL o AVAILABLE\n");
    scanf("%s",test);
    if(strcmp(test,"BOOK")==0)
    {
        controllo=1;
    }
    else
    {
        if(strcmp(test,"CANCEL")==0)
        controllo=2;
        else if(strcmp(test,"AVAILABLE")==0)
        controllo=3;

    }
    switch(controllo)
    {
        case 1: printf("ho letto BOOK\n"); break;
        case 2: printf("ho letto CANCEL\n"); break;
        case 3: printf("ho letto AVAILABLE\n"); break;
        
        default: printf("hai sbagliato\n"); break;
    }
   

    return 0;
}