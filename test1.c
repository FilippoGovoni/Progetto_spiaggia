#include<stdio.h>
#include<string.h>	//strlen
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write
#include<pthread.h>

pthread_t cliente;

typedef struct {
    char message[2000];
    int signal;
}Messaggio;

Messaggio MSG;

void *gestore_client(void*);

int main(int argc , char *argv[])
{
	int socket_desc , client_sock , c , read_size,P;
	struct sockaddr_in server , client;
	char client_message[2000];
	P=fork();

	if(P<0)
	{
		perror("fork fallita\n");
		exit(-1);
	}
	if(P==0)
	{
		close(getppid());
		//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");
	
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 57000 );
	
	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		//print the error message
		perror("bind failed. Error");
		return 1;
	}
	puts("bind done");
	
	//Listen
	listen(socket_desc , 3);
	
	//Accept and incoming connection
	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);
	
	//accept connection from an incoming client
	while(client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c))
    {
        if (client_sock < 0)
	{
		perror("accept failed");
		return 1;
	}
	puts("Connection accepted");

    if(pthread_create(&cliente,NULL,gestore_client,(void*)&client_sock)<0)
       {
           perror("errore creazione thread");
           return -1;
       }


    }
	}

	
	
	
	return 0;
}

void *gestore_client(void *client_sock)
{
    int sock= *(int*)client_sock;
    int read_size;
    int i=0;

    //Receive a message from client
	while( (read_size = recv(sock , &MSG , sizeof(MSG) , 0)) > 0 )
	{
		//Send the message back to client
        if(strcmp(MSG.message,"FINE")==0)
        {
            MSG.signal=1;
            strcpy(MSG.message,"fine conversazione");
            write(sock , &MSG , sizeof(MSG));
            shutdown(sock,2);
            break;
        }
        else
        {
            MSG.signal=0;
            write(sock , &MSG , sizeof(MSG));
        }
        
		
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


}