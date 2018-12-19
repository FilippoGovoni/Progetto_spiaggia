#include<stdio.h>	//printf
#include<string.h>	//strlen
#include<sys/socket.h>	//socket
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>


int main(int argc , char *argv[])
{
	int sock,s_lengt;
	struct sockaddr_in server;
	char message[1000] , server_reply[2000];
	
	//Create socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1)
	{
		printf("Errore creazione socket");
	}
	puts("Socket created");
	
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons( 8888 );

	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("Connessione fallita. Errore");
		return 1;
	}
	
	puts("Connesso\n");
	
	//keep communicating with server
	while(1)
	{   
        bzero(message,1000);
		printf("Enter message : ");
		fgets(message,2000,stdin);
		
		//Send some data
		if( send(sock , message , strlen(message) , 0) < 0)
		{
			puts("Send failed");
			return 1;
		}
		//Receive a reply from the server
		if( recv(sock , message , 1000 , 0) < 0)
		{
			puts("recv failed");
			break;
		}
		printf("Server:\n");
		printf("%s",message);                                                                                                                 
	}
	
	close(sock);
	return 0;
}