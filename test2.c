#include<stdio.h>	//printf
#include<string.h>	//strlen
#include<sys/socket.h>	//socket
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>

typedef struct {
    char message[2000];
    int signal;
}Messaggio;

int go =1;
Messaggio MSG;

int main(int argc , char *argv[])
{
	int sock;
	struct sockaddr_in server;
	char message[1000];
	
	//Create socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");
	
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons( 57000 );

	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("connect failed. Error");
		return 1;
	}
	
	puts("Connected\n");
	
	//keep communicating with server
	while(go)
	{
		printf("Enter message : ");
		scanf("%s" , message);
		strcpy(MSG.message,message);
		//Send some data
		if( send(sock , &MSG , sizeof(MSG) , 0) < 0)
		{
			puts("Send failed");
			return 1;
		}
		
		//Receive a reply from the server
		if( recv(sock , &MSG , sizeof(MSG) , 0) < 0)
		{
			puts("recv failed");
			break;
		}
        printf("segnale %d \n",MSG.signal);
		if(MSG.signal==0)
        {
            printf("%s\n",MSG.message);
        }
        else
        {
            printf("%s\n",MSG.message);
            go=0;
            shutdown(sock,2);
        }
			
	}
	
	close(sock);
	return 0;
}
