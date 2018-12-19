#include<stdio.h>
#include<stdlib.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write


#define Numero_Ombrelloni 90
#define Numero_righe 9
#define Numero_colonne 10


typedef struct
{
    int numero;
    int riga;
    int colonna;
    int stato;   //0:libero 1:occupato 2:temporaneamente occupato
    char datainizio[20]; 
    char datafine[20];
}Ombrellone;



int main(int argc , char *argv[])
{
	int socket_desc,client_sock,c,read_size,flag=0,cont_ombrelloni=0,numero_richiesta;
    int i=0,j,z=0,numero_richiesta1,controllo=3;
    char A;
	struct sockaddr_in server , client;
    FILE* statospiaggia;
	char client_message[2000];
    char BOOK[5];
    char CANCEL[7];
    char AVAILABLE[10];
    chat data[15];
    Ombrellone SPIAGGIA[90];

    
    //lettura dati iniziali spiggia

    if((statospiaggia=fopen("stato_spiaggia.txt","r"))==NULL)
    {
        printf("errore apertura file\n");
        exit(-1);
    }
    while(!feof(statospiaggia))
    {
        fscanf(statospiaggia,"%d %d %d %s %s",&SPIAGGIA[i].numero,&SPIAGGIA[i].riga,&SPIAGGIA[i].stato,SPIAGGIA[i].datainizio,SPIAGGIA[i].datafine);
        i++;
    }



	//Creazione socket

	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if(socket_desc <0)
	{
		printf("errore creazione socket\n");
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
	puts("In attesa di connesioni...");
	c = sizeof(struct sockaddr_in);
	
	//accept
	client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	if (client_sock < 0)
	{
		perror("accept fallita");
		return 1;
	}
	puts("Connessione OK");
	
	//Riceve il primo messaggio dal client

	while( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0 )
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
        
        if(strcmp(test,"BOOK")==0)
        {
            controllo=0;
        }
        else
        {
            if(strcmp(test,"CANCEL")==0)
            controllo=1;
            else if(strcmp(test,"AVAILABLE")==0)
            controllo=2;

        }
        switch(controllo)
        {
            case 0://BOOK

            break;
            case 1://CANCEL



            break;
            case 2: //AVAILABLE
            

            break;
            default:
            strcpy(client_message[i],"L'opzione scelta non è disponibile\0");
		    write(client_sock , client_message[i], strlen(client_message[i]));
            strcpy(client_message[i], " ");
			i++;
            break;
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
	
	return 0;
}