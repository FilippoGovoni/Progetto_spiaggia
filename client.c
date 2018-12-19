#include<stdio.h>	//printf
#include<string.h>	//strlen
#include<sys/socket.h>	//socket
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>


int main(int argc , char *argv[])
{
	int sock,s_lengt;
	struct sockaddr_in server;
	char message[1000][100];
	
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1)
	{
		printf("Errore creazione socket");
	}
	puts("Socket created");
	
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons( 8888 );

	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("Connessione fallita. Errore");
		return 1;
	}
	
	puts("Connesso\n");
	
	while(1)
	{
		printf("Inserisci richiesta : ");
		fgets(message[i],25,stdin);
		
		if( send(sock , message[i], strlen(message[i]) , 0) < 0)
		{
			puts("invio messaggio fallito");
			return 1;
		}
		
		if( recv(sock , message[i] , 2000 , 0) < 0)
		{
			puts("ricezione messaggio fallita");
			break;
		}
		
		printf("server: %s \n",message[i]);
		if(strcmp(message[i],"Prenotazione avvenuta con successo\0")==0 || strcmp(message[i],"Hai selezionato un ombrellone diverso\0")==0 || strcmp(message[i],"Prenotazione cancellata\0")==0)
		{
			printf("Conversazione terminata \n");
			break;
		}
		i++;
	}
	
	close(sock);
	return 0;
}