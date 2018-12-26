#include<stdio.h>	//printf
#include<string.h>	//strlen
#include<sys/socket.h>	//socket
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>


int main(void)
{
	int sock,s_lengt,lunghezza_messaggio;
	int i=0,j;
	struct sockaddr_in server;
	char message[100][1000];
	
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
		fgets(message[i],40,stdin);
		
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
		lunghezza_messaggio=strlen(message[i]);
		if(message[i][lunghezza_messaggio-5]=='\n' && message[i][lunghezza_messaggio-4]=='F')
		{
			printf("Server: ");
			for(j=0;j<(lunghezza_messaggio-4);j++)
			{
				printf("%c",message[i][j]);
			}
			break;
		}
		else
		{
			printf("Server: %s \n",message[i]);
		}
		i++;
	}
	
	close(sock);
	return 0;
}