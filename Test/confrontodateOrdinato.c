#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
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
    Periodo * tempo;
}Ombrellone;

int CodiceData (Data);
int confrontaDate(Data,Data,Data,Data);

int main(){
    Ombrellone ombr[91];
    FILE* statospiaggia;
    int i=1,flag=1,n,confronto,flag2=0;
    Periodo * prenotazione;
    Periodo ausilio[91]={0};   
    ausilio[0].next=NULL;


    if((statospiaggia=fopen("stato_spiaggia.txt","r+"))==NULL)
    {
        printf("errore apertura file\n");
        exit(-1);
    }
    
    fprintf(statospiaggia,"RIGA 1\n");      //NECESSARIO
    while(!feof(statospiaggia)){
        ombr[i].tempo=(Periodo *)malloc(sizeof(Periodo));
        fscanf(statospiaggia,"%d %d %d %d-%d %d-%d",&ombr[i].numero,&ombr[i].fila,&ombr[i].stato,&ombr[i].tempo->datainizio.giorno,&ombr[i].tempo->datainizio.mese,&ombr[i].tempo->datafine.giorno,&ombr[i].tempo->datafine.mese);
        ombr[i].tempo->next=NULL; ausilio[i].next=NULL;

      //  sleep(10);
        while((fgetc(statospiaggia)!='\n') && (i<91) ){
            printf("entrato posizione %d\n",i);
            ombr[i].tempo->next=(Periodo *)malloc(sizeof(Periodo));
            fscanf(statospiaggia,"%d-%d %d-%d",&ombr[i].tempo->next->datainizio.giorno,&ombr[i].tempo->next->datainizio.mese,&ombr[i].tempo->next->datafine.giorno,&ombr[i].tempo->next->datafine.mese);
            
            
            if(flag){ausilio[i]=*ombr[i].tempo;flag=0;}
            
            printf("%d-%d %d-%d\t",ausilio[i].datainizio.giorno,ausilio[i].datainizio.mese,ausilio[i].datafine.giorno,ausilio[i].datafine.mese);
            printf("%d-%d %d-%d\t",ausilio[i].next->datainizio.giorno,ausilio[i].next->datainizio.mese,ausilio[i].next->datafine.giorno,ausilio[i].next->datafine.mese);
            ombr[i].tempo=ombr[i].tempo->next;
        }
        flag=1;
        i++;
    }
    rewind(statospiaggia);
    fprintf(statospiaggia,"RIGA 1\n");      //NECESSARIO

    while(n<1 || n>90){printf("Inserisci il numero dell'ombrellone \n");scanf("%d",&n);} 
    prenotazione=(Periodo *)malloc(sizeof(Periodo));prenotazione->next=NULL;
    while( prenotazione->datainizio.giorno>31 || prenotazione->datainizio.giorno<1 || prenotazione->datainizio.mese<1 || prenotazione->datainizio.mese>12 || CodiceData(prenotazione->datainizio)>CodiceData(prenotazione->datafine) ){
        printf("Inserisci data di inizio e data di fine\n");
        scanf("%d-%d %d-%d",&prenotazione->datainizio.giorno,&prenotazione->datainizio.mese,&prenotazione->datafine.giorno,&prenotazione->datafine.mese);
    }

    if(ombr[n].stato==2){                      //ESISTE GIÀ UNA PRENOTAZIONE
        confronto=confrontaDate(prenotazione->datainizio,prenotazione->datafine,ombr[n].tempo->datainizio,ombr[n].tempo->datafine);
        if(ausilio[n].next!=NULL){              //CI SONO DUE O PIU PRENOTAZIONI
            ombr[n].tempo=&ausilio[n];      //LO DEVE CONFRONTARE CON TUTTE LE DATE
            printf("AUSILIO ESISTE\n");

            if(confronto==-1){
                printf("DA INSERIRE PRIMA DEL PRIMO ELEMENTO\n");
                //printf("CONTROLLO OMBR %d-%d %d-%d\n",ombr[n].tempo->datainizio.giorno,ombr[n].tempo->datainizio.mese,ombr[n].tempo->datafine.giorno,ombr[n].tempo->datafine.mese);
                //printf("CONTROLLO OMBR NEXT %d-%d %d-%d\n",ombr[n].tempo->next->datainizio.giorno,ombr[n].tempo->next->datainizio.mese,ombr[n].tempo->next->datafine.giorno,ombr[n].tempo->next->datafine.mese);
                
                prenotazione->next=ombr[n].tempo;

                printf("CONTROLLO PRENOTAZIONE %d-%d %d-%d\n",prenotazione->datainizio.giorno,prenotazione->datainizio.mese,prenotazione->datafine.giorno,prenotazione->datafine.mese);
                printf("CONTROLLO PRENOTAZIONE NEXT %d-%d %d-%d\n",prenotazione->next->datainizio.giorno,prenotazione->next->datainizio.mese,prenotazione->next->datafine.giorno,prenotazione->next->datafine.mese);

                ombr[n].tempo=prenotazione;
                printf("CONTROLLO OMBR NEXT %d-%d %d-%d\n",ombr[n].tempo->next->datainizio.giorno,ombr[n].tempo->next->datainizio.mese,ombr[n].tempo->next->datafine.giorno,ombr[n].tempo->next->datafine.mese);
                
                printf("CONTROLLO PRENOTAZIONE NEXT %d-%d %d-%d\n",prenotazione->next->datainizio.giorno,prenotazione->next->datainizio.mese,prenotazione->next->datafine.giorno,prenotazione->next->datafine.mese);


                
                printf("CONTROLLO OMBR %d-%d %d-%d\n",ombr[n].tempo->datainizio.giorno,ombr[n].tempo->datainizio.mese,ombr[n].tempo->datafine.giorno,ombr[n].tempo->datafine.mese);
                printf("CONTROLLO OMBR NEXT %d-%d %d-%d\n",ombr[n].tempo->next->datainizio.giorno,ombr[n].tempo->next->datainizio.mese,ombr[n].tempo->next->datafine.giorno,ombr[n].tempo->next->datafine.mese);
                ausilio[n] = *ombr[n].tempo;ausilio[n].next=ombr[n].tempo->next;
                printf("CONTROLLO AUSILIO %d-%d %d-%d\n",ausilio[n].datainizio.giorno,ausilio[n].datainizio.mese,ausilio[n].datafine.giorno,ausilio[n].datafine.mese);
                printf("CONTROLLO AUSILIO NEXT %d-%d %d-%d\n",ausilio[n].next->datainizio.giorno,ausilio[n].next->datainizio.mese,ausilio[n].next->datafine.giorno,ausilio[n].next->datafine.mese);

                printf("CONTROLLO OMBR %d-%d %d-%d\n",ombr[n].tempo->datainizio.giorno,ombr[n].tempo->datainizio.mese,ombr[n].tempo->datafine.giorno,ombr[n].tempo->datafine.mese);
                printf("CONTROLLO OMBR NEXT %d-%d %d-%d\n",ombr[n].tempo->next->datainizio.giorno,ombr[n].tempo->next->datainizio.mese,ombr[n].tempo->next->datafine.giorno,ombr[n].tempo->next->datafine.mese);
                
                sleep(10);
            }
            if(confronto==0)
                printf("Ombrellone già occupato, sorry m8\n");
            if(confronto==1){
                confronto=confrontaDate(prenotazione->datainizio,prenotazione->datafine,ombr[n].tempo->next->datainizio,ombr[n].tempo->next->datafine);
                while(confronto==1){
                    ombr[n].tempo=ombr[n].tempo->next;
                    confronto=confrontaDate(prenotazione->datainizio,prenotazione->datafine,ombr[n].tempo->next->datainizio,ombr[n].tempo->next->datafine);
                }
                if(confronto==0)
                    printf("Ombrellone già occupato, sorry m8\n");
                if(confronto==-1){
                    prenotazione->next=ombr[n].tempo->next;
                    ombr[n].tempo->next=prenotazione;
                }
            }
        }else{                                  //C'È SOLO UNA PRENOTAZIONE
            printf("AUSILIO NON ESISTE\n");
            if(confronto==1)
                ombr[n].tempo->next=prenotazione;
            if(confronto==-1){
                prenotazione->next=ombr[n].tempo;
                ombr[n].tempo=prenotazione;
            }
            if(confronto=0)
                printf("Ombrellone già occupato, sorry m8\n");
        }

        
        
        
        /*confronto=confrontaDate(prenotazione->datainizio,prenotazione->datafine,ombr[n].tempo->datainizio,ombr[n].tempo->datafine);
        if(confronto==1){
            confronto=confrontaDate(prenotazione->datainizio,prenotazione->datafine,ombr[n].tempo->next->datainizio,ombr[n].tempo->next->datafine);
            while(confronto==1 ){
                if(ombr[n].tempo->next!=NULL){
                    ombr[n].tempo=ombr[n].tempo->next;
                    confronto=confrontaDate(prenotazione->datainizio,prenotazione->datafine,ombr[n].tempo->next->datainizio,ombr[n].tempo->next->datafine);
                }else{
                    ombr[n].tempo->next=prenotazione;
                    goto FINE;
                }
            }
            Inserimento(ombr[n].tempo->next,prenotazione);
            goto FINE;
        }else{
            if(confronto==0)
                printf("Ombrellone già occupato, sorry m8\n");
            else{
                Inserimento(ombr[n].tempo,prenotazione);
                ausilio[n]=*prenotazione;
                FINE:
                printf("prenotazione effettuata\n");
            }
        }*/
        
        
        
        /*confronto=confrontaDate(prenotazione->datainizio,prenotazione->datafine,ombr[n].tempo->datainizio,ombr[n].tempo->datafine);
        ritorna:
        if(confrontaDate(prenotazione->datainizio,prenotazione->datafine,ombr[n].tempo->datainizio,ombr[n].tempo->datafine)){
            if(ombr[n].tempo->next==NULL){                       //inserire condizione di ordinamento
 
                ombr[n].tempo->next=prenotazione;
                printf("prenotazione effettuata con allocazione\n");
            }else{
                ombr[n].tempo=ombr[n].tempo->next;
                printf("CAMBIO DI VALORE DI OMBRELLONE\n");
                goto ritorna; 
            }
        }else
            printf("prenotazione non disponibile, ombrellone già occupato\n");*/

    }else{
        ombr[n].stato=2;
        //confrontaDate(prenotazione->datainizio,prenotazione->datafine,ombr[i].tempo->datainizio,ombr[i].tempo->datafine);
        //free(ombr[n].tempo);
        ombr[n].tempo=prenotazione;
        printf("prenotazione effettuata\n");
        printf("PRENOTAZIONE OMBR  NUM.%d FILA.%d STATO.%d DATAIN.%d-%d DATAFINE.%d-%d\n",ombr[n].numero,ombr[n].fila,ombr[n].stato,ombr[n].tempo->datainizio.giorno,ombr[n].tempo->datainizio.mese,ombr[n].tempo->datafine.giorno,ombr[n].tempo->datafine.mese);
    }
    

    for(i=1;i<91;i++){
        if(ausilio[i].next!=NULL)
            ombr[i].tempo=&ausilio[i];
        if(ombr[i].tempo->next==NULL){
            fprintf(statospiaggia,"%d %d %d %d-%d %d-%d\n",ombr[i].numero,ombr[i].fila,ombr[i].stato,ombr[i].tempo->datainizio.giorno,ombr[i].tempo->datainizio.mese,ombr[i].tempo->datafine.giorno,ombr[i].tempo->datafine.mese);
        }else{
            /*if(ausilio[i].next!=NULL){
                //ombr[i].tempo=&ausilio[i];
                printf("CONTROLLO: %dposizione %d-%d %d-%d\n",i,ombr[i].tempo->datainizio.giorno,ombr[i].tempo->datainizio.mese,ombr[i].tempo->datafine.giorno,ombr[i].tempo->datafine.mese);
            }*/
            printf("allocato qualcosa\n");
            fprintf(statospiaggia,"%d %d %d %d-%d %d-%d",ombr[i].numero,ombr[i].fila,ombr[i].stato,ombr[i].tempo->datainizio.giorno,ombr[i].tempo->datainizio.mese,ombr[i].tempo->datafine.giorno,ombr[i].tempo->datafine.mese);
            while(ombr[i].tempo->next!=NULL){
            //    printf("stampo il next\n");
                ombr[i].tempo=ombr[i].tempo->next;
                fprintf(statospiaggia,"\t%d-%d %d-%d",ombr[i].tempo->datainizio.giorno,ombr[i].tempo->datainizio.mese,ombr[i].tempo->datafine.giorno,ombr[i].tempo->datafine.mese);
            }
            fprintf(statospiaggia,"\n");
        }
    }

    fclose(statospiaggia);
    
}


int confrontaDate(Data inizio, Data fine, Data occupatoi,Data occupatof){
    if(CodiceData(inizio)>CodiceData(occupatof)) //se la prenotazione richiesta è DOPO di quella effettuata, restituisco 1
        return 1;
    if(CodiceData(fine)<CodiceData(occupatoi))    //se la prenotazione richiesta è PRIMA di quella effettuata, restituisco -1
        return -1;
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
