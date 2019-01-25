#include <stdio.h>
#include<string.h>
#include<stdlib.h>

#define Numero_Ombrelloni 90

typedef struct{
    int giorno;
    int mese;
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
    Periodo tempo;
    //datainizio
    //datafine
    //next
}Ombrellone;

int CodiceData (Data);
int confrontaDate(Data,Data,Data,Data);


int confrontaDate(Data inizio, Data fine, Data occupatoi,Data occupatof){
    if(CodiceData(inizio)>CodiceData(occupatof) || CodiceData(fine)<CodiceData(occupatoi))
        return 1;
    return 0;
}

int CodiceData(Data data){
    switch(data.mese){
        case 1:
            return data.giorno;
        case 2:
            return data.giorno+31;
        case 3:
            return data.giorno+31+28;
        case 4: 
            return data.giorno+31+28+31;
        case 5:
            return data.giorno+31+28+31+30;
        case 6:
            return data.giorno+31+28+31+30+31;
        case 7:
            return data.giorno+31+28+31+30+31+30;
        case 8:
            return data.giorno+31+28+31+30+31+30+31;
        case 9:
            return data.giorno+31+28+31+30+31+30+31+31;
        case 10:
            return data.giorno+31+28+31+30+31+30+31+31+30;
        case 11:
            return data.giorno+31+28+31+30+31+30+31+31+30+31;
        case 12:
            return data.giorno+31+28+31+30+31+30+31+31+30+31+30;
    }
}

int main(){
    Ombrellone ombr[90]; 
    FILE* statospiaggia;
    int i=0,n;
    Periodo prenotazione; prenotazione.next=NULL;
    Periodo ausilio; ausilio.next=NULL;  //diventerà ausilio[i]


    if((statospiaggia=fopen("stato_spiaggia.txt","r+"))==NULL)
    {
        printf("errore apertura file\n");
        exit(-1);
    }


    while(!feof(statospiaggia)){
        fscanf(statospiaggia,"%d %d %d %d-%d %d-%d",&ombr[i].numero,&ombr[i].fila,&ombr[i].stato,&ombr[i].tempo.datainizio.giorno,&ombr[i].tempo.datainizio.mese,&ombr[i].tempo.datafine.giorno,&ombr[i].tempo.datafine.mese);
        ombr[i].tempo.next=NULL;


        while(fgetc(statospiaggia)!=10 && (i<90) ){
            printf("entrato posizione %d\n",i);
            ombr[i].tempo.next=(Periodo *)malloc(sizeof(Periodo));
            fscanf(statospiaggia,"%d-%d %d-%d",&ombr[i].tempo.next->datainizio.giorno,&ombr[i].tempo.next->datainizio.mese,&ombr[i].tempo.next->datafine.giorno,&ombr[i].tempo.next->datafine.mese);
            ausilio=ombr[i].tempo;
            printf("%d-%d %d-%d\t",ausilio.datainizio.giorno,ausilio.datainizio.mese,ausilio.datafine.giorno,ausilio.datafine.mese);
            printf("%d-%d %d-%d\t",ausilio.next->datainizio.giorno,ausilio.next->datainizio.mese,ausilio.next->datafine.giorno,ausilio.next->datafine.mese);
            ombr[i].tempo=*ombr[i].tempo.next;
            printf("ultima prenotazione: %d-%d %d-%d\t",ombr[i].tempo.datainizio.giorno,ombr[i].tempo.datainizio.mese,ombr[i].tempo.datafine.giorno,ombr[i].tempo.datafine.mese);
            if (ombr[i].tempo.next==NULL)
                printf("CONTROLLO VALORE NEXT OK\n");
        }
        i++;
    }
    rewind(statospiaggia);

    while(n<1 || n>90){printf("Inserisci il numero dell'ombrellone \n");scanf("%d",&n);} n--;
  
    while( (prenotazione.datainizio.giorno>31) || (prenotazione.datainizio.giorno<1) || (prenotazione.datainizio.mese<1) || (prenotazione.datainizio.mese>12) ){
        printf("Inserisci data di inizio e data di fine\n");
        scanf("%d-%d %d-%d",&prenotazione.datainizio.giorno,&prenotazione.datainizio.mese,&prenotazione.datafine.giorno,&prenotazione.datafine.mese);
    }

    if(ombr[n].stato==2){
        if(ausilio.next!=NULL)
            ombr[n].tempo=ausilio;      //LO DEVE CONFRONTARE CON TUTTE LE DATE
        
        ritorna:
        if(confrontaDate(prenotazione.datainizio,prenotazione.datafine,ombr[n].tempo.datainizio,ombr[n].tempo.datafine)){
            if(ombr[n].tempo.next==NULL){                       //inserire condizione di ordinamento
                ombr[n].tempo.next=(Periodo *)malloc(sizeof(Periodo));
                ombr[n].tempo.next=&prenotazione;
                printf("prenotazione effettuata con allocazione\n");
                ombr[n].tempo=*ombr[n].tempo.next;
                printf("GG %d %d %d %d-%d %d-%d\n",ombr[n].numero,ombr[n].fila,ombr[n].stato,ombr[n].tempo.datainizio.giorno,ombr[n].tempo.datainizio.mese,ombr[n].tempo.datafine.giorno,ombr[n].tempo.datafine.mese);
                printf("CONTROLLO AUSILIO %d-%d   \n",ausilio.next->datafine.giorno,ausilio.next->datafine.mese);
                printf("%d-%d\n",ausilio.next->next->datafine.giorno,ausilio.next->next->datafine.mese);
            }else{
                ombr[n].tempo=*ombr[n].tempo.next;
                printf("CAMBIO DI VALORE DI OMBRELLONE\n");
                goto ritorna; 
            }
        }else
            printf("prenotazione non disponibile, ombrellone già occupato\n");   
    }else{
        ombr[n].stato=2;
        ombr[n].tempo=prenotazione;
        printf("prenotazione effettuata\n");
        printf("PRENOTAZIONE OMBR  NUM.%d FILA.%d STATO.%d DATAIN.%d-%d DATAFINE.%d-%d\n",ombr[n].numero,ombr[n].fila,ombr[n].stato,ombr[n].tempo.datainizio.giorno,ombr[n].tempo.datainizio.mese,ombr[n].tempo.datafine.giorno,ombr[n].tempo.datafine.mese);
    }

    if(ausilio.next!=NULL)
        ombr[n].tempo=ausilio;

 /* CONTROLLO  
    printf("%d %d %d %d-%d %d-%d\t",ombr[n].numero,ombr[n].fila,ombr[n].stato,ombr[n].tempo.datainizio.giorno,ombr[n].tempo.datainizio.mese,ombr[n].tempo.datafine.giorno,ombr[n].tempo.datafine.mese);
    while(ombr[n].tempo.next!=NULL){
        printf("%d-%d %d-%d\n",ombr[n].tempo.next->datainizio.giorno,ombr[n].tempo.next->datainizio.mese,ombr[n].tempo.next->datafine.giorno,ombr[n].tempo.next->datafine.mese);
        ombr[n].tempo.next=ombr[n].tempo.next->next;
    }
*/

    for(i=0;i<90;i++){
        if(ombr[i].tempo.next==NULL){
            fprintf(statospiaggia,"%d %d %d %d-%d %d-%d\n",ombr[i].numero,ombr[i].fila,ombr[i].stato,ombr[i].tempo.datainizio.giorno,ombr[i].tempo.datainizio.mese,ombr[i].tempo.datafine.giorno,ombr[i].tempo.datafine.mese);
        }else{
            printf("allocato qualcosa\n");
            fprintf(statospiaggia,"%d %d %d %d-%d %d-%d",ombr[i].numero,ombr[i].fila,ombr[i].stato,ombr[i].tempo.datainizio.giorno,ombr[i].tempo.datainizio.mese,ombr[i].tempo.datafine.giorno,ombr[i].tempo.datafine.mese);
            while(ombr[i].tempo.next!=NULL){
                printf("stampo il next\n");
                ombr[i].tempo=*ombr[i].tempo.next;
                fprintf(statospiaggia,"\t%d-%d %d-%d",ombr[i].tempo.datainizio.giorno,ombr[i].tempo.datainizio.mese,ombr[i].tempo.datafine.giorno,ombr[i].tempo.datafine.mese);
            }
            fprintf(statospiaggia,"\n");
        }
    }

    fclose(statospiaggia);
    
}






/*
int main(){
   Data datai;  datai.giorno=0;datai.mese=0;
   Data dataf;  dataf.giorno=0;dataf.mese=0;
   Data dataoc; dataoc.giorno=0;dataoc.mese=0;
    while( (datai.giorno>31) || (datai.giorno<1) || (datai.mese<1) || (datai.mese>12) ){
        printf("Inserisci la data di inizio\n");
        scanf("%d %d",&datai.giorno, &datai.mese);
    }
        printf("Il codice risultante è %d\n",CodiceData(datai));
    while( (dataf.giorno>31) || (dataf.giorno<1) || (dataf.mese<1) || (dataf.mese>12) ){
        printf("Inserisci la data di fine\n");
        scanf("%d %d",&dataf.giorno, &dataf.mese);
    }
       printf("Il codice risultante è %d\n",CodiceData(dataf));
    while( (dataoc.giorno>31) || (dataoc.giorno<1) || (dataoc.mese<1) || (dataoc.mese>12) ){
        printf("Inserisci la data di Occupato\n");
        scanf("%d %d",&dataoc.giorno, &dataoc.mese);
    } 
      printf("Il codice risultante è %d\n",CodiceData(dataoc));
    if(confrontaDate(datai,dataf,dataoc))
        printf("Prenotazione disponibile\n");
    else
        printf("Ombrellone occupato, GG\n");

}*/

 /*   printf("Inserisci il numero dell'ombrellone\n");
    scanf("%d",&n);n--;
    ombr[n].stato=2;
    printf("Inserisci data di inizio e data di fine\n");
    scanf("%d-%d %d-%d",&ombr[n].tempo.datainizio.giorno,&ombr[n].tempo.datainizio.mese,&ombr[n].tempo.datafine.giorno,&ombr[n].tempo.datafine.mese );
    printf("ombrellone numero %d è allo stato %d con date %d-%d e %d-%d \n",ombr[n].numero,ombr[n].stato,ombr[n].tempo.datainizio.giorno,ombr[n].tempo.datainizio.mese,ombr[n].tempo.datafine.giorno,ombr[n].tempo.datafine.mese);
 */