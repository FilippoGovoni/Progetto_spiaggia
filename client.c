#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>


typedef struct
{
	char message[200];
	int segnale;
} Messaggio;

int go = 1;
int sock;
Messaggio MSG;

void handler(int sig)
{
	switch(sig) {
    case SIGINT:
      printf(" CTRL-C premuto, conversazione in chiusura\n");
			close(sock);
      break;
    case SIGTSTP:
      printf(" CTRL-Z premuto, conversazione in chiusura\n");
			close(sock);
      break;
    case SIGALRM:
			printf("\nSONO PASSATI 60 SECONDI, NESSUN MESSAGGIO INVIATO, CLIENT IN CHIUSURA\n");
      close(sock);
			break;
    }
	kill(getpid(),15);
}

int main(int argc, char **argv)
{
	int s_lengt, lunghezza_messaggio;
	int j, t, flag = 0;
	struct sockaddr_in server;
	char message[200] = {0};


	//ZONA SEGNALI

	if ( signal(SIGALRM, handler) == SIG_ERR ) 
    perror("segnale non ricevuto\n");
  
  if ( signal(SIGINT,  handler) == SIG_ERR )
    perror("segnale non ricevuto\n");
    
  if ( signal(SIGTSTP, handler) == SIG_ERR )
    perror("segnale non ricevuto\n");





	if (argc == 1 || argc >= 4 )
	{
		printf("Numero di argomenti incorretto\n");
		exit(-1);
	}
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		printf("Errore creazione socket");
	}
	puts("Socket created");

	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons(58000);

	if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("Connessione fallita. Errore");
		return 1;
	}
	puts("Connesso\n");

	strcpy(message, argv[1]);

	if (argc == 3)
	{
		t = strlen(message);
		message[t] = ' ';
		strcat(message, argv[2]);
	}

	if (send(sock, message, strlen(message), 0) < 0)
	{
		puts("invio messaggio fallito");
		return 1;
	}
	if (recv(sock, &MSG, sizeof(MSG), 0) < 0)
	{
		puts("ricezione messaggio fallita");
		go = 0;
	}
	printf("%s\n", MSG.message);
	if (recv(sock, &MSG, sizeof(MSG), 0) < 0)
		{
			puts("ricezione messaggio fallita");
			go = 0;
		}
	printf("%s\n", MSG.message);
	if (MSG.segnale == 1)
	{
		go = 0;
		shutdown(sock, 2);
	}

	while (go)
	{
	
		printf("Inserisci richiesta : ");

		alarm(60);		//DA QUI SI IMPOSTANO I SECONDI DI INATTIVITÀ  DEL CLIENT
		
		fgets(message, 40, stdin);
		if (send(sock, message, strlen(message), 0) < 0)
		{
			puts("invio messaggio fallito");
			return 1;
		}
		if (recv(sock, &MSG, sizeof(MSG), 0) < 0)
		{
			puts("ricezione messaggio fallita");
			break;
		}
		printf("%s\n", MSG.message);
		if (MSG.segnale == 1)
		{
			go = 0;
			shutdown(sock, 2);
		}
		
	}

	close(sock);
	return 0;
}
