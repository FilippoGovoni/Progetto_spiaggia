#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#define Numero_ombrelloneelloni 90

typedef struct{
    int giorno;
    int mese;
    int anno;
}Data;


struct _periodo{
    Data datainizio;
    Data datafine;
    struct _periodo * next;
};
typedef struct _periodo Periodo;

typedef struct{
    int numero;
    int fila;
    int stato;
    Periodo * tempo;
}Ombrellone;

int CodiceData (Data);
int confrontaDate(Data,Data,Data,Data);


int confrontaDate(Data inizio, Data fine, Data occupatoi,Data occupatof){
    if(CodiceData(inizio)>CodiceData(occupatof) || CodiceData(fine)<CodiceData(occupatoi))  //se la prenotazione richiesta è DOPO di quella effettuata, restituisco 1
        return 1;
    return 0;
}
int main(){
    Ombrellone ombrellone[91];
    FILE* statospiaggia;
    int i=1,flag=1,n;
    Periodo prenotazione; prenotazione.next=NULL;
    Periodo ausilio[91]={0};   
    ausilio[0].next=NULL;


    if((statospiaggia=fopen("stato_spiaggia.txt","r+"))==NULL)
    {
        printf("errore apertura file\n");
        exit(-1);
    }

    fprintf(statospiaggia,"RIGA 1\n");
    while(!feof(statospiaggia)){
        ombrellone[i].tempo=(Periodo *)malloc(sizeof(Periodo));
        fscanf(statospiaggia,"%d %d %d %d-%d-%d %d-%d-%d",&ombrellone[i].numero,&ombrellone[i].fila,&ombrellone[i].stato,&ombrellone[i].tempo->datainizio.giorno,&ombrellone[i].tempo->datainizio.mese,&ombrellone[i].tempo->datainizio.anno,&ombrellone[i].tempo->datafine.giorno,&ombrellone[i].tempo->datafine.mese,&ombrellone[i].tempo->datafine.anno);
        ombrellone[i].tempo->next=NULL; ausilio[i].next=NULL;
        while((fgetc(statospiaggia)!='\n') && (i<91) ){
            ombrellone[i].tempo->next=(Periodo *)malloc(sizeof(Periodo));
            fscanf(statospiaggia,"%d-%d-%d %d-%d-%d",&ombrellone[i].tempo->next->datainizio.giorno,&ombrellone[i].tempo->next->datainizio.mese,&ombrellone[i].tempo->next->datainizio.anno,&ombrellone[i].tempo->next->datafine.giorno,&ombrellone[i].tempo->next->datafine.mese,&ombrellone[i].tempo->next->datafine.anno);
            if(flag){ausilio[i]=*ombrellone[i].tempo;flag=0;}
            ombrellone[i].tempo=ombrellone[i].tempo->next;
        }
        flag=1;
        i++;
    }
    rewind(statospiaggia);
    fprintf(statospiaggia,"RIGA 1\n");

    while(n<1 || n>90){printf("Inserisci il numero dell'ombrelloneellone \n");scanf("%d",&n);} 
  
    while( prenotazione.datainizio.giorno>31 || prenotazione.datainizio.giorno<1 || prenotazione.datainizio.mese<1 || prenotazione.datainizio.mese>12 || CodiceData(prenotazione.datainizio)>CodiceData(prenotazione.datafine) ){
        printf("Inserisci data di inizio e data di fine\n");
        scanf("%d-%d-%d %d-%d-%d",&prenotazione.datainizio.giorno,&prenotazione.datainizio.mese,&prenotazione.datainizio.anno,&prenotazione.datafine.giorno,&prenotazione.datafine.mese,&prenotazione.datafine.anno);
    }

    if(ombrellone[n].stato==2){
        if(ausilio[n].next!=NULL)
            ombrellone[n].tempo=&ausilio[n];      //LO DEVE CONFRONTARE CON TUTTE LE DATE
        
        ritorna:
        if(confrontaDate(prenotazione.datainizio,prenotazione.datafine,ombrellone[n].tempo->datainizio,ombrellone[n].tempo->datafine)){
            if(ombrellone[n].tempo->next==NULL){                       //inserire condizione di ordinamento
                ombrellone[n].tempo->next=(Periodo *)malloc(sizeof(Periodo));
                ombrellone[n].tempo->next=&prenotazione;
            }else{
                ombrellone[n].tempo=ombrellone[n].tempo->next;
                goto ritorna; 
            }
        }else
            printf("prenotazione non disponibile, ombrellone già occupato\n");   
    }else{
        ombrellone[n].stato=2;
        ombrellone[n].tempo=&prenotazione;
        printf("prenotazione effettuata\n");
    }
    

    for(i=1;i<91;i++){
        if(ausilio[i].next!=NULL)
                ombrellone[i].tempo=&ausilio[i];
        if(ombrellone[i].tempo->next==NULL){
            fprintf(statospiaggia,"%d %d %d %d-%d-%d %d-%d-%d\n",ombrellone[i].numero,ombrellone[i].fila,ombrellone[i].stato,ombrellone[i].tempo->datainizio.giorno,ombrellone[i].tempo->datainizio.mese,ombrellone[i].tempo->datainizio.anno,ombrellone[i].tempo->datafine.giorno,ombrellone[i].tempo->datafine.mese,ombrellone[i].tempo->datafine.anno);
        }else{
            fprintf(statospiaggia,"%d %d %d %d-%d-%d %d-%d-%d",ombrellone[i].numero,ombrellone[i].fila,ombrellone[i].stato,ombrellone[i].tempo->datainizio.giorno,ombrellone[i].tempo->datainizio.mese,ombrellone[i].tempo->datainizio.anno,ombrellone[i].tempo->datafine.giorno,ombrellone[i].tempo->datafine.mese,ombrellone[i].tempo->datafine.anno);
            while(ombrellone[i].tempo->next!=NULL){
                ombrellone[i].tempo=ombrellone[i].tempo->next;
                fprintf(statospiaggia,"\t%d-%d-%d %d-%d-%d",ombrellone[i].tempo->datainizio.giorno,ombrellone[i].tempo->datainizio.mese,ombrellone[i].tempo->datainizio.anno,ombrellone[i].tempo->datafine.giorno,ombrellone[i].tempo->datafine.mese,ombrellone[i].tempo->datafine.anno);
            }
            fprintf(statospiaggia,"\n");
        }
    }

    fclose(statospiaggia);
    
}

int CodiceData(Data data){
    return data.anno*100000+data.mese*1000+data.giorno;
}


Data StringToData(char * calenda){
    char A[4];int z=0,i=0;
    Data DATA;
    for(z=0;z<2;z++){
        A[i]=calenda[z];
        i++;
    }
    DATA.giorno=atoi(A);
    i=0;
    for(z=3;z<5;z++){
        A[i]=calenda[z];
        i++;
    }
    DATA.mese=atoi(A);
    i=0;
    for(z=6;z<10;z++){
        A[i]=calenda[z];
        i++;
    }
    DATA.anno=atoi(A);

    return DATA;

}