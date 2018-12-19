#include<stdio.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write

int main(int argc , char *argv[])
{	
	int socket_desc,client_sock,c,read_size,numero=1,numero_richiesta,stato=0,numero_richiesta1;
	int i=0,j,z=0;
    int flag=0,flagg=1;
	struct sockaddr_in server , client;
	char A;
	char client_message[2000][100];
	char BOOK[5];
	char CANCEL[7];
	char Data[15];

	
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
	server.sin_port = htons( 8888 );
	
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
	client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	if (client_sock < 0)
	{
		perror("accept failed");
		return 1;
	}
	puts("Connection accepted");
	
	//Receive a message from client
	if( (read_size = recv(client_sock , client_message[i] , 2000 , 0)) > 0 )
	{
		// ciclo for per il calcolo di ombrelloni disponibili
		if(numero>0)//conta_ombrelloni>0
            {
               if(flag==0)//inserire semaforo per la sincronizzazione dei thread
                {
                    flag=1;
				    strcpy(client_message[i],"Prenotazione disponibile\0");
		            write(client_sock , client_message[i], strlen(client_message[i]));
					i++;
                    
					if((read_size = recv(client_sock , client_message[i] , 2000 , 0))>0)
                    {
						for(j=0;j<4;j++)
						{
							BOOK[j]=client_message[i][j];
						}
						BOOK[4]='\0';
						if(strcmp(BOOK,"BOOK\0")==0)
						{
							A= client_message[i][5];
                        	numero_richiesta=A-'0';
							strcpy(client_message[i]," ");
                        	if(stato==0) //if(ombrellone[numero_richiesta].stato==0)
                        	{
                            	stato=2;//ombrellone.stato=2
                            	strcpy(client_message[i],"AVAILABLE\0");
		                    	write(client_sock , client_message[i], strlen(client_message[i]));
								i++;	
                            	if((read_size = recv(client_sock , client_message[i] , 2000 , 0))>0)
								{
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
									if(strcmp(BOOK,"BOOK\0")==0)
									{

										A= client_message[i][5];
                        				numero_richiesta1=A-'0';
										strcpy(client_message[i]," ");
										if(numero_richiesta==numero_richiesta1)
										{
											for(j=7;j<=17;j++)
											{
												Data[z]=client_message[i][j];
												z++;
											}
											Data[z]='\0';
											printf("PRENOTAZIONE:\n\nnumero ombrellone: %d\ndata di fine prenotazione: %s\n",numero_richiesta1,Data);
											//ombrellone[numero_richiesta].stato=1; strcpy(ombrellone[numero_richiesta].data,Data);
											strcpy(client_message[i], " ");
                            				strcpy(client_message[i],"Prenotazione avvenuta con successo\0");
		                    				write(client_sock , client_message[i], strlen(client_message[i]));
                           					strcpy(client_message[i], " ");
											i++;
										}
										else //controllare stato di ombrellone[numero_richiesta1]
										{
											strcpy(client_message[i], " ");
                            				strcpy(client_message[i],"Hai selezionato un ombrellone diverso\0");
		                    				write(client_sock , client_message[i], strlen(client_message[i]));
                           					strcpy(client_message[i], " ");
											i++;
										}
									}
									else if(strcmp(CANCEL,"CANCEL\0")==0)
									{
										stato=0; //ombrellone[numero_richiesta].stato=0;
										strcpy(client_message[i], " ");
                            			strcpy(client_message[i],"Prenotazione cancellata\0");
		                    			write(client_sock , client_message[i], strlen(client_message[i]));
                           				strcpy(client_message[i], " ");
									}
                        		}
							}
                        	else
                       		{
                            	strcpy(client_message[i], " ");
                            	strcpy(client_message[i],"NAVAILABLE\0");
		                    	write(client_sock , client_message[i], strlen(client_message[i]));
                           		strcpy(client_message[i], " ");
                       		}	
                    	}
						else
						{
							strcpy(client_message[i], " ");
                            strcpy(client_message[i],"Inserisci BOOK e il numero di ombrellone che vuoi prenotare\0");
		                    write(client_sock , client_message[i], strlen(client_message[i]));
                           	strcpy(client_message[i], " ");
						}
					}
					else
					{
						perror("errore ricezione messaggio\n");
					}
				}
                else
                {
                   	strcpy(client_message[i], "");
                    strcpy(client_message[i],"NOK\0");
		            write(client_sock , client_message[i], strlen(client_message[i]));
                    strcpy(client_message[i], "");
                    	//inserire ciclo con semaforo
                } 
        }	
        else
        {
            strcpy(client_message[i], "");
            strcpy(client_message[i],"NAVAILABLE");
		    write(client_sock , client_message[i], strlen(client_message[i]));
            strcpy(client_message[i], "");
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
	
	return 0;
}