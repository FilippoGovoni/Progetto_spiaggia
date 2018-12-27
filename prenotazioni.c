#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

typedef struct 
{
    int numero;
    char codice[6];
    char datainizio[20]; 
    char datafine[20]; 
}Ombrellone;

int main(void)
{
    Ombrellone O[200];
    int i;
    FILE *F;

    if((F=fopen("prenotazioni.txt","wt"))==NULL)
    {   
        printf("errore apertura file\n");
        exit(-1);
    }   
    for(i=0;i<500;i++)
    {
        O[i].numero=0;
        strcpy(O[i].codice,"00000");
        strcpy(O[i].datainizio,"00/00/0000");
        strcpy(O[i].datafine,"00/00/0000");
    }
    for(i=0;i<500;i++)
        {
            fprintf(F,"%d %s %s %s\n",O[i].numero,O[i].codice,O[i].datainizio,O[i].datafine);
        }
    fclose(F);
    return 0;
}