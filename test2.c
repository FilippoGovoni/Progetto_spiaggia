#include<stdio.h>
#include<stdlib.h>
#include<string.h>


int main(void)
{
    int i,controllo=0,t;
    int numero[10]={81,82,83,84,85,86,87,88,89,90};
    char test[100]={0};
    char testo[300];
    strcpy(test," ");
    strcpy(testo," ");
    strcpy(testo,"Ombrelloni disponibili\n");
    for(i=0;i<10;i++)
    {
        sprintf(test,"%s%d",test,numero[i]);
        t=strlen(test);
        test[t]=32;
    }
        printf("messaggio: %s\n",testo);
        printf("Liberi: %s\n",test);
        strcat(testo,test);
        printf("finale: %s\n",testo);
    

    return 0;
}