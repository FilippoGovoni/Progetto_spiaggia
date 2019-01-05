#include<stdio.h>
#include<stdlib.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write
#include<time.h>
#include<pthread.h>
#include"funzioni.h"

pthread_t cliente;

void *gestore_client(void*);

#define Numero_Ombrelloni 90
#define Numero_righe 9
#define Numero_colonne 10

int main(void)
{
	int socket_desc,client_sock,c;
    int i=0,j,t=0;
	struct sockaddr_in server , client;
    FILE* statospiaggia;
	char client_message[100][2000];
    char data_inizio[20]={0};
    parametri dato;
    pthread_t cliente;
    time_t temp;

    srand((unsigned) time(&temp));

    //lettura dati iniziali spiggia

    if((statospiaggia=fopen("stato_spiaggia.txt","r+"))==NULL)
    {
        printf("errore apertura file\n");
        exit(-1);
    }
    while(!feof(statospiaggia))
    {
        fscanf(statospiaggia,"%d %d %d %s %s %s",&dato.ombrellone[i].numero,&dato.ombrellone[i].riga,&dato.ombrellone[i].stato,dato.ombrellone[i].codice,dato.ombrellone[i].datainizio,dato.ombrellone[i].datafine);
        i++;
    }
    printf("Inserisci la data di oggi\n");
    fgets(data_inizio,15,stdin);
    t=strlen(data_inizio);
    data_inizio[t-1]='\0';

    //controllo stato ombrellone inizio
    for(t=0;t<90;t++)
    {
        if(strcmp(dato.ombrellone[t].datainizio,data_inizio)==0)
        {
            dato.ombrellone[t].stato=1;
        }
    }
     //controllo stato ombrellone fine
    for(t=0;t<90;t++)
    {
        if(strcmp(dato.ombrellone[t].datafine,data_inizio)==0)
        {
            dato.ombrellone[t].stato=0;
        }
    }
    dato.f=statospiaggia;

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
	listen(socket_desc , 3);
	
	//Attesa di connessioni
	puts("In attesa di connessioni...");
	c = sizeof(struct sockaddr_in);
	
	//accept
	while(client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c))
    {
       puts("Connessione OK");
       dato.sock=client_sock;
       if(pthread_create(&cliente,NULL,gestore_client,(void*)&dato)<0)
       {
           perror("errore creazione thread");
           return 2;
       }

    }
    fclose(statospiaggia);
	close(socket_desc);
	return 0;
}

void *gestore_client(void *dato)
{
    parametri dati= *(parametri*)dato;
    int i=0,j,controllo=3,read_size;
    FILE* statospiaggia;
    char BOOK[5]={0};
    char CANCEL[7]={0};
    char AVAILABLE[10]={0};
    char client_message[100][2000];
    
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
            func_BOOK(dati.sock,client_message,dati.f,dati.ombrellone);
            break;
            case 1://CANCEL
            func_CANCEL(dati.sock,client_message[i],dati.ombrellone);
            break;
            case 2: //AVAILABLE
            func_AVAILABLE(dati.sock,client_message[i],dati.ombrellone);
            break;

            default:
		    write(dati.sock ,"L'opzione scelta non è disponibile\nFINE",41);
            strcpy(client_message[i], " ");
			
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

}