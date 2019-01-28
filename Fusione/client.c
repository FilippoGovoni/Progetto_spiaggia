#include<stdio.h>	
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<signal.h>


int main(int argc, char **argv)
{
	int sock,s_lengt,lunghezza_messaggio;
	int j,t,flag=0;
	struct sockaddr_in server;
    char message[100];

    if(argc== 1 || argc==4 || argc==5)
	{
		printf("Numero di argomenti incorretto\n");
		exit(-1);
	}
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


	strcpy(message,argv[1]);
	if((strcmp(message,"AVAILABLE")==0)&&  argc==2 )
	{
		t=strlen(message);
		message[t]=' ';
	strcat(message,"0");
	}

    if(argc==3)
	{
		t=strlen(message);
		message[t]=' ';
		strcat(message,argv[2]);
	}
	
	if( send(sock , message, strlen(message) , 0) < 0)
	{
		puts("invio messaggio fallito");
		return 1;
	}
    if( recv(sock , message , 2000 , 0) < 0)
		{
			puts("ricezione messaggio fallita");
			exit(-3);
		}
	if(message[lunghezza_messaggio-5]=='\n' && message[lunghezza_messaggio-4]=='F')
	{
		printf("Server: ");
		for(j=0;j<(lunghezza_messaggio-4);j++)
		{
			printf("%c",message[j]);
		}
		exit(-4);
	}
	else
	{
		printf("Server: %s \n",message);
	}

    while(1)
	{
		
		printf("Inserisci richiesta : ");
		fgets(message,40,stdin);
		if( send(sock , message, strlen(message) , 0) < 0)
		{
			puts("invio messaggio fallito");
			return 1;
		}

		if( recv(sock , message , 2000 , 0) < 0)
		{
			puts("ricezione messaggio fallita");
			break;
		}
		lunghezza_messaggio=strlen(message);
			if(message[lunghezza_messaggio-5]=='\n' && message[lunghezza_messaggio-4]=='F')
		{
			printf("Server: ");
			for(j=0;j<(lunghezza_messaggio-4);j++)
			{
				printf("%c",message[j]);
			}
			break;
		}
		else
		{
			printf("Server: %s \n",message);
		}
	}
	
	close(sock);
	return 0;
}

