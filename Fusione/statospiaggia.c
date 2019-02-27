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
    char codice[6];
    char datainizio[20]; 
    char datafine[20]; 
}Ombrellone;

int main(void)
{
    int i,j,c=1;
    Ombrellone A[9][10];
    FILE *f;
    if((f=fopen("stato_spiaggia.txt","wt"))==NULL)
    {  
        printf("errore apertura file\n") ;
        exit(-1);
    }
    for(i=0;i<9;i++)
    {
        for(j=0;j<10;j++)
        {
           A[i][j].numero=c;
           A[i][j].riga=i+1;
           A[i][j].colonna=j+1;
           A[i][j].stato=0;
           strcpy(A[i][j].codice,"00000");
           strcpy(A[i][j].datainizio,"00/00/0000");
           strcpy(A[i][j].datafine,"00/00/0000");
           //memset(A[i][j].data, 0, sizeof(A[i][j].data));
           c++;
        }
    }

    for(i=0;i<9;i++)
    {
        for(j=0;j<10;j++)
        {
            //numero,nriga,stato,data
            fprintf(f,"%d %d %d %s %s %s\n",A[i][j].numero,A[i][j].riga,A[i][j].stato,A[i][j].codice,A[i][j].datainizio,A[i][j].datafine);
        }
    }

    fclose(f);


}