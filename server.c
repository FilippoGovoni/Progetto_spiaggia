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
	int socket_desc , client_sock , c , read_size,i,j,flag=0,cont_ombrelloni=0,numero_richiesta;
    char A;
	struct sockaddr_in server , client;
    FILE* statospiaggia;
	char client_message[2000];
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



	//Create socket

	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if(socket_desc <0)
	{
		printf("errore creazione socket\n");
	}
	puts("Socket OK");
	
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 8888 );
	
	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		//print the error message
		perror("bind fallita. Errore");
		return 1;
	}
	puts("bind OK");
	
	//Listen
	listen(socket_desc , 3);
	
	//Accept and incoming connection
	puts("In attesa di connesioni...");
	c = sizeof(struct sockaddr_in);
	
	//accept connection from an incoming client
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
        printf("Client:\n");
		printf("%s\n",client_message);
        switch(client_message)
        {
            case 'BOOK':
            for(i=0;i<Numero_,BO)==0righe;i++)
            {
                if(ombrellone[i].stato==0)
                cont_ombrelloni++;
            }


            case 'AVAILABLE':



            
            case 'CANCEL':



            default:
        }
     
            
            if(cont_ombrelloni>0)
            {
               if(flag==0)//inserire semaforo per la sincronizzazione dei thread
                {
                    flag=1;
                    bzero(client_message,1000);
		            write(client_sock , 'Prenotazione disponibile\nInserire richiesta', strlen(client_message));
                    if(read_size = recv(client_sock , client_message , 2000 , 0)>0)
                    {
                        printf("Client : %s\n",client_message);
                        A= client_message[5];
                        numero_richiesta=A-'0';
                        if(ombrellone[numero_richiesta].stato==0)
                        {
                            ombrellone[numero_richiesta].stato=2;
                            //AVAILABLE
                        }
                        else
                        {
                            //NAVAILABLE
                        }
                    }
                    else
                    {
                        perror("errore di ricezione messaggio\n");
                    }
                }  
                else
                {
                    bzero(client_message,1000);
		            write(client_sock , 'NOK', strlen(client_message));
                    //inserire ciclo con semaforo
                } 
            }
            else
            {
                bzero(client_message,1000);
		        write(client_sock , 'NAVAILABLE', strlen(client_message));
            }
	
	
	if(read_size == 0)
	{
		puts("Client disconnected");
		fflush(stdout);
	}
	else if(read_size == -1)
	{
		perror("recv failed");
	}
	
	return 0;
}