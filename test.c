#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

typedef struct 
{
    int numero;
    int riga;
    int colonna;
    int stato;   //0:libero 1:occupato 2:temporaneamente occupato
    char data[10]; 
}Ombrellone;

int main(void)
{
    int i=0;
    Ombrellone O[90];
    FILE* st;

    if((st=fopen("stato_spiaggia.txt","r"))==NULL)
    {
        printf("errore apertura file\n");
        exit(-1);
    }
    while(!feof(st))
    {
        fscanf(st,"%d %d %d %s",&O[i].numero,&O[i].riga,&O[i].stato,O[i].data);
        i++;
    }

        
    for(i=0;i<90;i++)
    printf("ombrellone %d,%d,%d,%s\n",O[i].numero,O[i].riga,O[i].stato,O[i].data);

    fclose(st);

    return 0;
}