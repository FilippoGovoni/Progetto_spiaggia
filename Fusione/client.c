#include<stdio.h>	
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<signal.h>

typedef struct
{
    char message[100];
    int segnale;
}Messaggio;

Messaggio MSG[20];

int main(int argc, char **argv)
{
	int sock,s_lengt,lunghezza_messaggio,nmsg=0;
	int j,t,flag=0;
	int go=1;
	struct sockaddr_in server;
    char message[20][100];

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
	server.sin_port = htons( 58000 );

	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("Connessione fallita. Errore");
		return 1;
	}
	puts("Connesso\n");


	strcpy(message[nmsg],argv[1]);
	if((strcmp(message[nmsg],"AVAILABLE")==0)&&  argc==2 )
	{
		t=strlen(message[nmsg]);
		message[nmsg][t]=' ';
	strcat(message[nmsg],"0");
	}

    if(argc==3)
	{
		t=strlen(message[nmsg]);
		message[nmsg][t]=' ';
		strcat(message[nmsg],argv[2]);
	}
	
	if( send(sock , message[nmsg], strlen(message[nmsg]) , 0) < 0)
	{
		puts("invio messaggio fallito");
		return 1;
	}
	if( recv(sock , &MSG[nmsg] , sizeof(MSG[nmsg]) , 0) < 0)
		{
			puts("ricezione messaggio fallita");
			go=0;
		}
		printf("%s\n",MSG[nmsg].message);
		if(MSG[nmsg].segnale==1)
        {
            go=0;
            shutdown(sock,2);
        }
	nmsg++;
    while(go)
	{
		
		printf("Inserisci richiesta : ");
		fgets(message[nmsg],40,stdin);
		if( send(sock , message[nmsg], strlen(message[nmsg]) , 0) < 0)
		{
			puts("invio messaggio fallito");
			return 1;
		}

		if( recv(sock , &MSG[nmsg] , sizeof(MSG[nmsg]) , 0) < 0)
		{
			puts("ricezione messaggio fallita");
			break;
		}
		printf("%s\n",MSG[nmsg].message);
		if(MSG[nmsg].segnale==1)
        {
            go=0;
            shutdown(sock,2);
        }
		nmsg++;
	}
	
	close(sock);
	return 0;
}

