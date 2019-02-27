#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>	
#include<time.h>
#include<signal.h>
#include<semaphore.h>
#include<pthread.h>
#include"funzioni.h"

int confrontaDate(Data,Data,Data,Data);
int CodiceData(Data);
Data StringToData(char *);

int generaNumeri(){
    return rand() % 10;
}
Messaggio MSG;

void func_BOOK(int client_sock,Ombrellone *ombrellone,char data_inizio[SIZEMSG], Periodo ** ausilio,pthread_mutex_t mutex){
    int i=0,j=0,conta_liberi=0,numero_richiesta,numero_richiesta1,codnum,nmsg=0;
    int read_size,limit;
    int confronto,flag=0;
    char client_message[20][SIZEMSG],A[2],data_fine[15];
    FILE* modifiche;
    Data DATAIN,DATAFIN;
    Periodo * prenotazione;


    for(i=1;i<91;i++){
        if(ombrellone[i].stato==0)
            conta_liberi++;

        ombrellone[i].tempo=ausilio[i];
    }

    if (conta_liberi>0){
        strcpy(MSG.message,"Prenotazione Disponibile");
        MSG.segnale=0;
        write(client_sock,&MSG,sizeof(MSG));
        
        if(read_size=recv(client_sock,client_message[nmsg],100,0)>0){
            
            if(strncmp(client_message[nmsg],"BOOK",4)==0){
                for(i=0;i<2;i++)
                    A[i]=client_message[nmsg][i+5];
                
                numero_richiesta=atoi(A);
                pthread_mutex_lock(&mutex);

                if(ombrellone[numero_richiesta].stato==0 && numero_richiesta>0 && numero_richiesta <=90){
                    ombrellone[numero_richiesta].stato=2;
                    pthread_mutex_unlock(&mutex);

                    strcpy(MSG.message,"AVAILABLE\0");
                    MSG.segnale=0;
                    write(client_sock,&MSG,sizeof(MSG));
					
                    if((read_size = recv(client_sock , client_message[nmsg] , 100 , 0))>0){
                        for(i=0;i<2;i++)
                    		A[i]= client_message[nmsg][i+5];
                		numero_richiesta1=atoi(A);

                        if(strncmp(client_message[nmsg],"BOOK",4)==0){            //SE SI VUOLE PROCEDERE CON LA PRENOTAZIONE...

                            if(numero_richiesta==numero_richiesta1){                //CONTROLLO SUL NUMERO OMBRELLONE, NEL CASO LO CAMBI PER SBAGLIO

                                prenotazione=(Periodo *)malloc(sizeof(Periodo));
                                for(j=15;j<25;j++)
                                {
                                    if(client_message[nmsg][j]=='\n')
                                    limit=1;
                                }
                                if(limit){         //SE SI INSERISCE SOLO LA DATA DI FINE (SI DÀ COME DATA DI INIZIO QUELLA DI OGGI)...
                                    i=0;
                                    j=7;
                                    while(client_message[nmsg][j]==32)
                                    j++;
                                    limit=j+10;
                                    for(j;j<limit;j++){
                                        data_fine[i]=client_message[nmsg][j];
                                        i++;}
                                        i=0;
                                    ombrellone[numero_richiesta].stato=1;
                                    DATAIN=StringToData(data_inizio);
                                    DATAFIN=StringToData(data_fine);
                                
                                }else{                                     //SE SI INSERISCONO DATA DI INIZIO E DI FINE
                                    
                                    i=0;
                                    j=7;
                                    while(client_message[nmsg][j]==32)
                                    j++;
                                    limit=j+10;
                                    for(j;j<limit;j++){
                                        data_inizio[i]=client_message[nmsg][j];
                                        i++; }
                                    i=0;
                                    while(client_message[nmsg][j]==32)
                                    j++;
                                    limit=j+10;
                                    for(j;j<limit;j++){
                                        data_fine[i]=client_message[nmsg][j];
                                        i++;}
                                    ombrellone[numero_richiesta].stato=0;
                                    DATAIN=StringToData(data_inizio);
                                    DATAFIN=StringToData(data_fine);                       
                                
                                }
                                
                                prenotazione->datainizio=DATAIN;
                                prenotazione->datafine=DATAFIN;

                            
                                if(CodiceData(ombrellone[numero_richiesta].tempo->datainizio)!=0){              //SE È GIÀ STATA EFFETTUATA ALMENO UNA PRENOTAZIONE SU QUELL'OMBRELLONE...
                                    if(ombrellone[numero_richiesta].tempo->next!=NULL)
                                        ausilio[numero_richiesta]=ombrellone[numero_richiesta].tempo;
                                
                                    confronto=confrontaDate(prenotazione->datainizio,prenotazione->datafine,ombrellone[numero_richiesta].tempo->datainizio,ombrellone[numero_richiesta].tempo->datafine);
                                    
                                    
                                    if(confronto==-1){          //NECESSARIO, UNICA VOLTA IN CUI SI MODIFICA L'OMBRELLONE E NON L'AUSILIO
                                        prenotazione->next=ombrellone[numero_richiesta].tempo;
                                        ombrellone[numero_richiesta].tempo=prenotazione;
                                        flag=1;
                                    }


                                    if(confronto==0){
                                        ombrellone[numero_richiesta].stato=0;
                                        strcpy(MSG.message,"Ombrellone non disponibile, E' già occupato");
                                        MSG.segnale=1;
                                        write(client_sock,&MSG,sizeof(MSG));
                                        free(prenotazione);
                                    }

                                    while(ausilio[numero_richiesta]->next!=NULL && confronto==1){
                                        confronto=confrontaDate(prenotazione->datainizio,prenotazione->datafine,ausilio[numero_richiesta]->next->datainizio,ausilio[numero_richiesta]->next->datafine);
                                        if(confronto==-1){
                                            prenotazione->next=ausilio[numero_richiesta]->next;
                                            ausilio[numero_richiesta]->next=prenotazione;
                                            flag=1;
                                            break;
                                        }
                                        if (confronto==0){
                                            ombrellone[numero_richiesta].stato=0;
                                            strcpy(MSG.message,"Ombrellone non disponibile, E' già occupato");
                                            MSG.segnale=1;
                                            write(client_sock,&MSG,sizeof(MSG));
                                            free(prenotazione);
                                            break;
                                        }
                                        ausilio[numero_richiesta]=ausilio[numero_richiesta]->next;
                                    }
                                    if(confronto==1){
                                        ausilio[numero_richiesta]->next=prenotazione;
                                        flag=1;
                                    }
                                }else{
                                    free(ombrellone[numero_richiesta].tempo);
                                    ombrellone[numero_richiesta].tempo=prenotazione;
                                    flag=1;
                                }

                                if(flag){
                                    //generazione codice di cancellazione
                                    for(i=0;i<5;i++){
                                        codnum=generaNumeri();
                                        ombrellone[numero_richiesta].codice[i]=codnum+'0';
                                    }
                                    ombrellone[numero_richiesta].codice[6]='\0';    //Possibile errore!
                                    strcpy(client_message[nmsg],"Prenotazione eseguita\nCodice cancellazione: ");
                                    strcat(client_message[nmsg],ombrellone[numero_richiesta].codice);
                                    strcpy(MSG.message,client_message[nmsg]);
                                    MSG.segnale=1;
                                    write(client_sock,&MSG,sizeof(MSG));

                                    //scrittura a file momentanea

                                    if((modifiche=fopen("aggiornamenti.txt","a"))==NULL)
                                    {
                                        printf("errore apertura file\n");
                                        exit(-1);
                                    }
                                    fprintf(modifiche,"%d %d %d %s",ombrellone[numero_richiesta].numero,ombrellone[numero_richiesta].fila,ombrellone[numero_richiesta].stato,ombrellone[numero_richiesta].codice);
                                    while(ombrellone[numero_richiesta].tempo!=NULL){
                                        fprintf(modifiche,"\t%d/%d/%d %d/%d/%d",ombrellone[numero_richiesta].tempo->datainizio.giorno,ombrellone[numero_richiesta].tempo->datainizio.mese,ombrellone[numero_richiesta].tempo->datainizio.anno,ombrellone[numero_richiesta].tempo->datafine.giorno,ombrellone[numero_richiesta].tempo->datafine.mese,ombrellone[numero_richiesta].tempo->datafine.anno);
                                        ombrellone[numero_richiesta].tempo=ombrellone[numero_richiesta].tempo->next;
                                    }
                                    fprintf(modifiche,"\n");
                                    fclose(modifiche);
                                }
                                                

                            }else
                            {
                                strcpy(MSG.message,"Hai selezionato un ombrellone diverso");
                                MSG.segnale=1;
                                write(client_sock,&MSG,sizeof(MSG));
                            }
                        
                        
                        }
                        
                        if(strncmp(client_message[nmsg],"CANCEL",6)==0){          //SE SI CAMBIA IDEA E NON SI PROCEDE CON LA PRENOTAZIONE...
                            ombrellone[numero_richiesta].stato=0;
                            strcpy(MSG.message,"Prenotazione cancellata");
                            MSG.segnale=1;
                            write(client_sock,&MSG,sizeof(MSG));
                        }

                    }
                }else
                {
                    pthread_mutex_unlock(&mutex);
                    strcpy(MSG.message,"NAVAILABLE");
                    MSG.segnale=1;
                    write(client_sock,&MSG,sizeof(MSG));
                }
                


            }else{
                strcpy(MSG.message,"Inserisci BOOK e il numero dell'ombrellone che vuoi prenotare");
                MSG.segnale=1;
                write(client_sock,&MSG,sizeof(MSG));
            }


        }


    }else{           //SE NON CI SONO OMBRELLONI DISPONIBILI
        strcpy(MSG.message,"NAVAILABLE");
        MSG.segnale=1;
        write(client_sock,&MSG,sizeof(MSG));
    }


}

void func_CANCEL(int client_sock,char richiesta[SIZEMSG],Ombrellone *ombrellone){
    
    FILE *modifiche;
    char client_message[SIZEMSG];
    int numero_richiesta,read_size;
    int i;
    char A[2],cod[6];


    if((modifiche=fopen("aggiornamenti.txt","a"))==NULL){
        printf("errore apertura file\n");
        exit(-1);
    }
    for(i=0;i<2;i++)
        A[i]=richiesta[i+7];
    numero_richiesta=atoi(A);

    if(ombrellone[numero_richiesta].stato==0 || ombrellone[numero_richiesta].stato==2 ){
        strcpy(MSG.message,"L'ombrellone non può essere disdetto");
        MSG.segnale=1;
        write(client_sock,&MSG,sizeof(MSG));
    }else{
        strcpy(MSG.message,"Inserisci il codice per disdire la prenotazione");
        MSG.segnale=0;
        write(client_sock,&MSG,sizeof(MSG));
        if((read_size = recv(client_sock , client_message , 100 , 0))>0){
            for(i=0;i<5;i++)
                cod[i]=client_message[i];
            
            if(strcmp(ombrellone[numero_richiesta].codice,cod)==0){         //SE IL CODICE DI CANCELLAZIONE È GIUSTO... (CYBER-SECURITY)
                ombrellone[numero_richiesta].stato=0;
                strcpy(ombrellone[numero_richiesta].codice,"00000");
                ombrellone[numero_richiesta].tempo->datainizio.giorno=0;ombrellone[numero_richiesta].tempo->datainizio.mese=0;ombrellone[numero_richiesta].tempo->datainizio.anno=0;
                ombrellone[numero_richiesta].tempo->datafine.giorno=0;ombrellone[numero_richiesta].tempo->datafine.mese=0;ombrellone[numero_richiesta].tempo->datafine.anno=0;

                fprintf(modifiche,"%d %d %d %s",ombrellone[numero_richiesta].numero,ombrellone[numero_richiesta].fila,ombrellone[numero_richiesta].stato,ombrellone[numero_richiesta].codice);
                while(ombrellone[numero_richiesta].tempo!=NULL){
                    fprintf(modifiche,"\t%d/%d/%d %d/%d/%d",ombrellone[numero_richiesta].tempo->datainizio.giorno,ombrellone[numero_richiesta].tempo->datainizio.mese,ombrellone[numero_richiesta].tempo->datainizio.anno,ombrellone[numero_richiesta].tempo->datafine.giorno,ombrellone[numero_richiesta].tempo->datafine.mese,ombrellone[numero_richiesta].tempo->datafine.anno);
                    ombrellone[numero_richiesta].tempo=ombrellone[numero_richiesta].tempo->next;
                }
                fprintf(modifiche,"\n");
                strcpy(MSG.message,"CANCEL OK");
                MSG.segnale=1;
                write(client_sock,&MSG,sizeof(MSG));
                
            }else                                                           //SE IL CODICE NON È QUELLO GIUSTO C'È UN HACKER 
            {
                strcpy(MSG.message,"Codice errato");
                MSG.segnale=1;
                write(client_sock,&MSG,sizeof(MSG));
            }
          
        }/*receive socket*/
    }
    fclose(modifiche);

}

void func_AVAILABLE(int client_sock,char richiesta[SIZEMSG],Ombrellone *ombrellone){
    int i,j=0,t=0,z=0,lenght,cifra=0,nmsg=0;
    char client_message[20][SIZEMSG];
    int n_ombr_disponibili=0,fila_ombr;
    int ombr_disponibili[10]={0};
    char Liberi[30]={0};
    char carat;
    char A[2]={0};
    nmsg++;
    for(i=0;i<1;i++)
            A[i]= richiesta[i+10];

        fila_ombr=atoi(A);
    if(fila_ombr==0)          //SE È STATO INSERITO SOLO AVAILABLE SENZA SPECIFICHE
    {
        for(i=1;i<91;i++){
            if(ombrellone[i].stato==0)
                n_ombr_disponibili++;
        }

        if(n_ombr_disponibili>0){                     //SE CI SONO OMBR DISPONIBILI
            strcpy(client_message[nmsg],"AVAILABLE  ");
            lenght=strlen(client_message[nmsg]);

            if(n_ombr_disponibili>9){
                cifra=n_ombr_disponibili%10;
                carat=cifra+'0';
                client_message[nmsg][lenght]=carat;

                cifra=n_ombr_disponibili/10;
                carat=cifra+'0';

                lenght=strlen(client_message[nmsg]);
                client_message[nmsg][lenght-2]=carat;

                lenght=strlen(client_message[nmsg]);
                client_message[nmsg][lenght]='\0';
            }else{

                carat=n_ombr_disponibili+'0';
                client_message[nmsg][lenght]=carat;

                lenght=strlen(client_message[nmsg]);
                client_message[nmsg][lenght-2]='\0';

            }
            strcpy(MSG.message,client_message[nmsg]);
            MSG.segnale=1;
            nmsg++;
            write(client_sock,&MSG,sizeof(MSG));
        }else{                                      //SE NON CE NE SONO
            strcpy(MSG.message,"NAVAILABLE");
            MSG.segnale=1;
            write(client_sock,&MSG,sizeof(MSG));
        }

    }else{                         //SE È SPECIFICATA QUALE FILA DI OMBRELLONI CERCARE

        nmsg++;
        if(fila_ombr>0 && fila_ombr<10){
            for(t=1;t<91;t++)
                    {
                        if(ombrellone[t].fila==fila_ombr && ombrellone[t].stato==0)
                        {
                            ombr_disponibili[j]=ombrellone[t].numero;
                            j++;
                        }
                    }
                    strcpy(Liberi," ");
                    strcpy(client_message[nmsg]," ");
                    strcpy(client_message[nmsg],"Ombrelloni disponibili\n");
                    for(z=0;z<j;z++)
                    {
                        sprintf(Liberi,"%s%d",Liberi,ombr_disponibili[z]);
                        t=strlen(Liberi);
                        Liberi[t]=32;
                    }
                    strcat(client_message[nmsg],Liberi);
					strcpy(MSG.message,client_message[nmsg]);
                    MSG.segnale=1;
                    write(client_sock,&MSG,sizeof(MSG));
        }else{
            strcpy(MSG.message,"La fila selezionata non esiste");
            MSG.segnale=1;
            write(client_sock,&MSG,sizeof(MSG));
        }
        
    }
}







int confrontaDate(Data inizio, Data fine, Data occupatoi,Data occupatof){
    if(CodiceData(inizio)>CodiceData(occupatof)) //se la prenotazione richiesta è DOPO di quella effettuata, restituisco 1
        return 1;
    if(CodiceData(fine)<CodiceData(occupatoi))    //se la prenotazione richiesta è PRIMA di quella effettuata, restituisco -1
        return -1;
    return 0;
}

int CodiceData(Data data){
    return data.anno*100000+data.mese*1000+data.giorno;
}

Data StringToData(char * calenda){
    char A[4]={0};int z=0,i=0;
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
