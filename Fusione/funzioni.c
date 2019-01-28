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


void func_BOOK(int client_sock,Ombrellone *ombrellone,char data_inizio[SIZEMSG], Periodo ** ausilio){
    int i,conta_liberi=0,numero_richiesta,numero_richiesta1,codnum;
    int read_size;
    int confronto,flag=0;
    char client_message[SIZEMSG],A[2],data_fine[15];
    FILE* modifiche;
    Data DATAIN,DATAFIN;
    Periodo * prenotazione;


    if((modifiche=fopen("aggiornamenti.txt","a"))==NULL){
        printf("errore apertura file\n");
        exit(-1);
    }

    for(i=1;i<91;i++){
        if(ombrellone[i].stato==0)
            conta_liberi++;
    }

    if (conta_liberi>0){
        strcpy(client_message,"Prenotazione Disponibile\0");
        write(client_sock,client_message,strlen(client_message));
        
        if(read_size=recv(client_sock,client_message,30,0)>0){
            
            if(strncmp(client_message,"BOOK",4)==0){
                for(i=0;i<2;i++)
                    A[i]=client_message[i+5];
                
                numero_richiesta=atoi(A);
                if(ombrellone[numero_richiesta].stato==0 && numero_richiesta>0 && numero_richiesta <=90){
                    ombrellone[numero_richiesta].stato=2;
                    strcpy(client_message,"AVAILABLE\0");
                    write(client_sock , client_message, strlen(client_message));
					
                    if((read_size = recv(client_sock , client_message , 30 , 0))>0){
                        for(i=0;i<2;i++)
                    		A[i]= client_message[i+5];
                		numero_richiesta1=atoi(A);

                        if(strncmp(client_message,"BOOK",4)==0){            //SE SI VUOLE PROCEDERE CON LA PRENOTAZIONE...

                            if(numero_richiesta==numero_richiesta1){                //CONTROLLO SUL NUMERO OMBRELLONE, NEL CASO LO CAMBI PER SBAGLIO

                                prenotazione=(Periodo *)malloc(sizeof(Periodo));
                                if(client_message[17]=='\n' || client_message[18]=='\n'){         //SE SI INSERISCE SOLO LA DATA DI FINE (SI DÀ COME DATA DI INIZIO QUELLA DI OGGI)...
                                    
                                    for(i=0;i<=10;i++)
                                        data_fine[i]=client_message[i+8];
                                    
                                   // data_fine[18]='\0';
                                    ombrellone[numero_richiesta].stato=1;
                                    DATAIN=StringToData(data_inizio);
                                    DATAFIN=StringToData(data_fine);
                                
                                }else{                                                           //SE SI INSERISCONO DATA DI INIZIO E DI FINE
                                    for(i=0;i<10;i++)
                                        data_inizio[i]=client_message[i+8];
                                    for(i=0;i<10;i++)
                                        data_fine[i]=client_message[i+19];

                                    ombrellone[numero_richiesta].stato=0;
                                    DATAIN=StringToData(data_inizio);
                                    DATAFIN=StringToData(data_fine);                       
                                
                                }
                                prenotazione->datainizio=DATAIN;
                                prenotazione->datafine=DATAFIN;

                                if(CodiceData(ombrellone[numero_richiesta].tempo->datainizio)!=0){              //SE È GIÀ STATA EFFETTUATA ALMENO UNA PRENOTAZIONE SU QUELL'OMBRELLONE...
                                    if(ombrellone[numero_richiesta].tempo->next!=NULL)
                                        ausilio[numero_richiesta]=ombrellone[numero_richiesta].tempo;
                                    
                                    confronto=confrontaDate(prenotazione->datainizio,prenotazione->datafine,ausilio[numero_richiesta]->datainizio,ausilio[numero_richiesta]->datafine);
                                    
                                    
                                    if(confronto==-1){          //NECESSARIO, UNICA VOLTA IN CUI SI MODIFICA L'OMBRELLONE E NON L'AUSILIO
                                        prenotazione->next=ombrellone[numero_richiesta].tempo;
                                        ombrellone[numero_richiesta].tempo=prenotazione;
                                        flag=1;
                                    }


                                    if(confronto==0){
                                        strcpy(client_message,"ombrellone non disp, già occupato m8\n");
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
                                            strcpy(client_message,"ombrellone non disponibile, già occupato\n");
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
                                    strcpy(client_message,"Prenot eseguita\n Codice canc: ");
                                    strcat(client_message,ombrellone[numero_richiesta].codice);
                                    strcat(client_message,"\nFINE");
                                
                                    //scrittura a file momentanea 

                                    fprintf(modifiche,"%d %d %d %s",ombrellone[numero_richiesta].numero,ombrellone[numero_richiesta].fila,ombrellone[numero_richiesta].stato,ombrellone[numero_richiesta].codice);
                                    while(ombrellone[numero_richiesta].tempo!=NULL){
                                        fprintf(modifiche,"\t%d/%d/%d %d/%d/%d",ombrellone[numero_richiesta].tempo->datainizio.giorno,ombrellone[numero_richiesta].tempo->datainizio.mese,ombrellone[numero_richiesta].tempo->datainizio.anno,ombrellone[numero_richiesta].tempo->datafine.giorno,ombrellone[numero_richiesta].tempo->datafine.mese,ombrellone[numero_richiesta].tempo->datafine.anno);
                                        ombrellone[numero_richiesta].tempo=ombrellone[numero_richiesta].tempo->next;
                                    }
                                    fprintf(modifiche,"\n");

                                }

								write(client_sock ,client_message, strlen(client_message));
                                                

                            }else
                                write(client_sock ,"Hai selezionato un ombrellone diverso\nFINE",43);
                        
                        
                        }
                        
                        if(strncmp(client_message,"CANCEL",6)==0){          //SE SI CAMBIA IDEA E NON SI PROCEDE CON LA PRENOTAZIONE...
                            ombrellone[numero_richiesta].stato=0;
                            write(client_sock ,"Prenotazione cancellata\nFINE", 29);
                        }

                    }
                }else
                {
                    write(client_sock,"NAVAILABLE\nFINE",16);
                }
                


            }else{
                write(client_sock ,"Inserisci BOOK e il numero dell'ombrellone che vuoi prenotare\nFINE", 67);
                strcpy(client_message, " ");
            }


        }


    }else                   //SE NON CI SONO OMBRELLONI DISPONIBILI
        write(client_sock ,"NAVAILABLE\nFINE",16);
    
    fclose(modifiche);


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

    if(ombrellone[numero_richiesta].stato==0 || ombrellone[numero_richiesta].stato==2 )
        write(client_sock ,"L'ombrellone non può essere disdetto\nFINE",43);
    else{
        write(client_sock ,"Inserisci il codice per disdire la prenotazione\n",49);
        if((read_size = recv(client_sock , client_message , 6 , 0))>0){
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
                write(client_sock ,"CANCEL OK\nFINE",15);
                
            }else                                                           //SE IL CODICE NON È QUELLO GIUSTO C'È UN HACKER 
            {
                write(client_sock ,"Codice errato\nFINE",19);
            }
            



        }/*receive soket*/
    }
    fclose(modifiche);

}

void func_AVAILABLE(int client_sock,char richiesta[SIZEMSG],Ombrellone *ombrellone){
    int i,lenght,cifra=0;
    char client_message[SIZEMSG];
    int ombr_disponibili=0,fila_ombr;
    char carat;
    char A[2];

    if(richiesta[9]=='\0')          //SE È STATO INSERITO SOLO AVAILABLE SENZA SPECIFICHE
    {
        for(i=1;i<91;i++){
            if(ombrellone[i].stato==0)
                ombr_disponibili++;
        }

        if(ombr_disponibili>0){                     //SE CI SONO OMBR DISPONIBILI
            strcpy(client_message,"AVAILABLE ");
            lenght=strlen(client_message);

            if(ombr_disponibili>9){
                cifra=ombr_disponibili/10;
                carat=cifra+'0';
                client_message[lenght]=carat;

                cifra=ombr_disponibili%10;
                carat=cifra+'0';

                lenght=strlen(client_message);
                client_message[lenght-2]=carat;

                lenght=strlen(client_message);
                client_message[lenght]='\0';
            }else{

                carat=ombr_disponibili+'0';
                client_message[lenght]=carat;

                lenght=strlen(client_message);
                client_message[lenght-2]='\0';

            }

            write(client_sock , client_message,lenght);
                    
        }else{                                      //SE NON CE NE SONO
            write(client_sock ,"NAVAILABLE\nFINE",16);
        }

    }else{                         //SE È SPECIFICATA QUALE FILA DI OMBRELLONI CERCARE
        
        for(i=0;i<2;i++)
            A[i]= richiesta[i+10];

        fila_ombr=atoi(A);

        if(fila_ombr>0 && fila_ombr<10){
            for(i=1;i<91;i++){
                if(ombrellone[i].fila==fila_ombr && ombrellone[i].stato==0)
                    ombr_disponibili++;
            }

            strcpy(client_message,"AVAILABLE ");
            lenght=strlen(client_message);

            if(ombr_disponibili<10){
                carat=ombr_disponibili+'0';
                client_message[lenght]=carat;

                lenght=strlen(client_message);
                client_message[lenght-2]='\0';

            }else{

                client_message[lenght]='1';
                lenght=strlen(client_message);
                client_message[lenght-2]='0';
                lenght=strlen(client_message);
                client_message[lenght]='\0';

            }

            write(client_sock , client_message,lenght);



        }else{
            write(client_sock ,"La fila selezionata non esiste\nFINE",36);
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
    char A[4];int z=0,i=0;
    Data DATA;

    if(calenda[1]<48 || calenda[2]>57){
        calenda[1]=calenda[0];
        calenda[0]='0';
    }

    if (calenda[4]<48 || calenda[4]>57){
        calenda[4]=calenda[3];
        calenda[3]='0';
    }

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
