#include<stdio.h>
#include<stdlib.h>
#include<string.h>	
#include<sys/socket.h>
#include<arpa/inet.h>	
#include<unistd.h>	
#include<signal.h>
#include<time.h>
#include<sys/types.h>
#include<pthread.h>
#include<semaphore.h>
#include"funzioni.h"

//Numero_Ombrelloni 90
//Numero_righe 9
//Numero_colonne 10

pthread_t writer;
pthread_t cliente;
pthread_mutex_t mutex;

void *gestore_client(void*);
void *filewriter(void*);

Ombrellone ombrellone[91]; 
    

int main(void)
{
	int socket_desc,client_sock,c;
    int i=0,j,t=0,pid;
	struct sockaddr_in server , client;
    FILE* statospiaggia;
    FILE* modifiche;
	char client_message[100][2000];
    char data_inizio[20]={0};
    parametri dato;

    //
    Data dataoggi;
    int confronto;
    Periodo * prenotazione;
    Periodo * ausilio[91];   
    //

    time_t temp;

    srand((unsigned) time(&temp));

    //lettura dati iniziali spiggia
    if((statospiaggia=fopen("stato_spiaggia.txt","r+"))==NULL)
    {
        printf("errore apertura file\n");
        exit(-1);
    }
    if((modifiche=fopen("aggiornamenti.txt","a"))==NULL)
        {
            printf("errore apertura file\n");
            exit(-1);
        }
    while(!feof(statospiaggia)){
        ombrellone[i].tempo=(Periodo *)malloc(sizeof(Periodo));
        fscanf(statospiaggia,"%d %d %d %s %d-%d-%d %d-%d-%d",&ombrellone[i].numero,&ombrellone[i].fila,&ombrellone[i].stato,&ombrellone[i].codice,&ombrellone[i].tempo->datainizio.giorno,&ombrellone[i].tempo->datainizio.mese,&ombrellone[i].tempo->datainizio.anno,&ombrellone[i].tempo->datafine.giorno,&ombrellone[i].tempo->datafine.mese,&ombrellone[i].tempo->datafine.anno);
        ausilio[i]=ombrellone[i].tempo;

        while((fgetc(statospiaggia)!='\n') && (i<91)){
            ausilio[i]->next=(Periodo *)malloc(sizeof(Periodo));
            fscanf(statospiaggia,"%d-%d-%d %d-%d-%d",&ausilio[i]->next->datainizio.giorno,&ausilio[i]->next->datainizio.mese,&ausilio[i]->next->datainizio.anno,&ausilio[i]->next->datafine.giorno,&ausilio[i]->next->datafine.mese,&ausilio[i]->next->datafine.anno);
            ausilio[i]=ausilio[i]->next;
        }
       // ausilio[i]=ombrellone[i].tempo;      AUSILIO PUNTA ALL'ULTIMO ELEMENTO INSERITO NELLA RIGA
       i++;
    }



    printf("Inserisci la data di oggi\n");
    fgets(data_inizio,15,stdin);
    t=strlen(data_inizio);
    data_inizio[t-1]='\0';
    dataoggi=StringToData(data_inizio);

    //controllo stato ombrellone inizio
    for(i=1;i<91;i++)
    {
        if(ombrellone[i].tempo->datainizio==dataoggi)
        {
            ombrellone[i].stato=1;
            fprintf(modifiche,"%d %d %d %s",ombrellone[i].numero,ombrellone[i].fila,ombrellone[i].stato,ombrellone[i].codice);
            fprintf(statospiaggia,"\t%d-%d-%d %d-%d-%d",ombrellone[i].tempo->datainizio.giorno,ombrellone[i].tempo->datainizio.mese,ombrellone[i].tempo->datainizio.anno,ombrellone[i].tempo->datafine.giorno,ombrellone[i].tempo->datafine.mese,ombrellone[i].tempo->datafine.anno);
			
        }
    }
     //controllo stato ombrellone fine
    for(i=1;i<91;i++)
    {
        if(strcmp(ombrellone[t].datafine,data_inizio)==0)
        {
            ombrellone[t].stato=0;
            strcpy(ombrellone[t].codice,"00000");
            strcpy(ombrellone[t].datainizio,"00/00/0000");
            strcpy(ombrellone[t].datafine,"00/00/0000");
            fprintf(modifiche,"%d %d %d %s %s %s\n",ombrellone[t].numero,ombrellone[t].riga,ombrellone[t].stato,ombrellone[t].codice,ombrellone[t].datainizio,ombrellone[t].datafine);
											    
        }
    }
    fclose(modifiche);
    // Creazione thread per la gestione dei file
    if(pthread_create(&writer,NULL,filewriter,(void*)&dato)<0)
       {
           perror("errore creazione thread");
           return 2;
       }
	//Creazione socket

	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if(socket_desc <0)
	{
		puts("errore creazione socket\n");
	}
	puts("Socket OK");
	
	//creazione della struttura sockaddr_in
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 8888 );
	
	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("bind fallita. Errore");
		return 1;
	}
	puts("bind OK");
	
	//Listen
	listen(socket_desc ,8);
	
	//Attesa di connessioni
	puts("In attesa di connessioni...");
	c = sizeof(struct sockaddr_in);
	fclose(statospiaggia);
	//accept
	while(client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c))
    {
       printf("Connessione aperta con %d\n",client_sock);
       dato.sock=client_sock;
       if(pthread_create(&cliente,NULL,gestore_client,(void*)&dato)<0)
       {
           perror("errore creazione thread");
           return -1;
       }

    }
	close(socket_desc);  
    

	return 0;
}

void *gestore_client(void *dato)
{
    parametri dati= *(parametri*)dato;
    int i=0,j,controllo=3,read_size;
    FILE *statospiaggia;
    char BOOK[5]={0};
    char CANCEL[7]={0};
    char AVAILABLE[10]={0};
    char client_message[100][2000];
    if((statospiaggia=fopen("stato_spiaggia.txt","r+"))==NULL)
    {
        printf("errore apertura file\n");
        exit(-1);
    }
    while(!feof(statospiaggia))
    {
        fscanf(statospiaggia,"%d %d %d %s %s %s",&ombrellone[i].numero,&ombrellone[i].riga,&ombrellone[i].stato,ombrellone[i].codice,ombrellone[i].datainizio,ombrellone[i].datafine);
        i++;
    }
    //Inizio conversazione
	if( (read_size = recv(dati.sock,client_message[i],2000,0))>0)
	{ 
        //controllo sulla richiesta del client
        for(j=0;j<4;j++)
        {
            BOOK[j]=client_message[i][j];
		}
		BOOK[4]='\0';
		for(j=0;j<6;j++)
		{
			CANCEL[j]=client_message[i][j];
		}
		CANCEL[6]='\0';
        for(j=0;j<9;j++)
        {
            AVAILABLE[j]=client_message[i][j];
		}
        AVAILABLE[9]='\0';
        
        if(strcmp(BOOK,"BOOK\0")==0)
        controllo=0;
        if(strcmp(CANCEL,"CANCEL\0")==0)
        controllo=1;
        if(strcmp(AVAILABLE,"AVAILABLE\0")==0)
        controllo=2;
        switch(controllo)
        {
            case 0://BOOK
            func_BOOK(dati.sock,mutex,ombrellone,dati.Data);
            break;
            case 1://CANCEL
            func_CANCEL(dati.sock,client_message[i],ombrellone);
            break;
            case 2: //AVAILABLE
            func_AVAILABLE(dati.sock,client_message[i],ombrellone);
            break;

            default:
		    write(dati.sock ,"L'opzione scelta non è disponibile\nFINE",36);
			
            break;
        }
    }  
        if(read_size == 0)
	{
		puts("Il client si è disconnesso");
		fflush(stdout);
	}
	else if(read_size == -1)
	{
		perror("ricezione fallita");
	}

    if(close(dati.sock)<0)
    perror("errore chiusura socket\n");

    pthread_exit(NULL);
    fclose(statospiaggia);
}

void *filewriter(void *dato)
{
    parametri dati=*(parametri*)dato;
    int i,t;
    FILE* statospiaggia;
    FILE* modifiche;
    Ombrellone prova;

    if((statospiaggia=fopen("stato_spiaggia.txt","r+"))==NULL)
    {
        printf("errore apertura file\n");
        exit(-1);
    }
    while(1)
    {
        sleep(30);
        if((modifiche=fopen("aggiornamenti.txt","r"))==NULL)
        {
            printf("errore apertura file\n");
            exit(-1);
        }
        while(!feof(statospiaggia))
        {           //MODIFICA
            fscanf(statospiaggia,"%d %d %d %s %s %s",&ombrellone[i].numero,&ombrellone[i].riga,&ombrellone[i].stato,ombrellone[i].codice,ombrellone[i].datainizio,ombrellone[i].datafine);
            i++;
        }
        while(!feof(modifiche))
        {           //MODIFICA
            fscanf(modifiche,"%d %d %d %s %s %s",&prova.numero,&prova.riga,&prova.stato,prova.codice,prova.datainizio,prova.datafine);
            for(i=0;i<90;i++)
            {
                if(prova.numero==0)
                {
                    break;
                }
                if(prova.numero== ombrellone[i].numero)
                {
                    ombrellone[i].stato=prova.stato;
                    strcpy(ombrellone[i].codice,prova.codice);
                    strcpy(ombrellone[i].datainizio,prova.datainizio);
                    strcpy(ombrellone[i].datafine,prova.datafine);
                }
            }
        }
        fclose(modifiche);
        if((modifiche=fopen("aggiornamenti.txt","w"))==NULL)
        {
            printf("errore apertura file\n");
            exit(-1);
        }
        fclose(modifiche);
        rewind(statospiaggia);
        //for(t=0;t<90;t++)
        //fprintf(statospiaggia,"%d %d %d %s %s %s\n",ombrellone[t].numero,ombrellone[t].riga,ombrellone[t].stato,ombrellone[t].codice,ombrellone[t].datainizio,ombrellone[t].datafine);
		
        
        for(t=1;t<91;t++){
        if(ausilio[t].next!=NULL)
                ombrellone[t].tempo=&ausilio[t];
        if(ombrellone[t].tempo->next==NULL){
            fprintf(statospiaggia,"%d %d %d %s %d-%d-%d %d-%d-%d\n",ombrellone[t].numero,ombrellone[t].fila,ombrellone[t].stato,ombrellone[t].codice,ombrellone[t].tempo->datainizio.giorno,ombrellone[t].tempo->datainizio.mese,ombrellone[t].tempo->datainizio.anno,ombrellone[t].tempo->datafine.giorno,ombrellone[t].tempo->datafine.mese,ombrellone[t].tempo->datafine.anno);
        }else{
            fprintf(statospiaggia,"%d %d %d %s %d-%d-%d %d-%d-%d",ombrellone[t].numero,ombrellone[t].fila,ombrellone[t].stato,ombrellone[t].codice,ombrellone[t].tempo->datainizio.giorno,ombrellone[t].tempo->datainizio.mese,ombrellone[t].tempo->datainizio.anno,ombrellone[t].tempo->datafine.giorno,ombrellone[t].tempo->datafine.mese,ombrellone[t].tempo->datafine.anno);
            while(ombrellone[t].tempo->next!=NULL){
                ombrellone[t].tempo=ombrellone[t].tempo->next;
                fprintf(statospiaggia,"\t%d-%d-%d %d-%d-%d",ombrellone[t].tempo->datainizio.giorno,ombrellone[t].tempo->datainizio.mese,ombrellone[t].tempo->datainizio.anno,ombrellone[t].tempo->datafine.giorno,ombrellone[t].tempo->datafine.mese,ombrellone[t].tempo->datafine.anno);
            }
            fprintf(statospiaggia,"\n");
        }
    }

    }

    
}