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



int main(void)
{
	int socket_desc,client_sock,c,read_size,flag=0,numero_richiesta;
    int i=0,j,z=0,numero_richiesta1,controllo=3,conta_liberi=0,t=0;
    int ombr_disponibili[10]={0};
    char A[2];
	struct sockaddr_in server , client;
    FILE* statospiaggia;
	char client_message[100][2000];
    char BOOK[5]={0};
    char CANCEL[7]={0};
    char AVAILABLE[10]={0};
    char data_inizio[20]={0};
    char data_fine[20]={0};
    char Liberi[100]={0};
    Ombrellone ombrellone[90];

    
    //lettura dati iniziali spiggia

    if((statospiaggia=fopen("stato_spiaggia.txt","r+"))==NULL)
    {
        printf("errore apertura file\n");
        exit(-1);
    }
    while(!feof(statospiaggia))
    {
        fscanf(statospiaggia,"%d %d %d %s %s",&ombrellone[i].numero,&ombrellone[i].riga,&ombrellone[i].stato,ombrellone[i].datainizio,ombrellone[i].datafine);
        i++;
    }
    printf("Inserisci la data di oggi\n");
    fgets(data_inizio,15,stdin);
    t=strlen(data_inizio);
    data_inizio[t-1]='\0';

    //controllo stato ombrellone
    for(t=0;t<90;t++)
    {
        if(strcmp(ombrellone[t].datainizio,data_inizio)==0)
        {
            ombrellone[t].stato=1;
        }
    }

	//Creazione socket

	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if(socket_desc <0)
	{
		printf("errore creazione socket\n");
	}
	puts("Socket OK");
	
	//creazione della struttura sockaddr_in
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 8888 );
	
	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("bind fallita. Errore");
		return 1;
	}
	puts("bind OK");
	
	//Listen
	listen(socket_desc , 3);
	
	//Attesa di connessioni
	puts("In attesa di connessioni...");
	c = sizeof(struct sockaddr_in);
	
	//accept
	client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	if (client_sock < 0)
	{
		perror("accept fallita");
		return 1;
	}
	puts("Connessione OK");
	
	//Inizio conversazione
	if( (read_size = recv(client_sock , client_message[i] , 2000 , 0)) > 0 )
	{   
        //controllo sulla richiesta del client
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
        for(j=0;j<9;j++)
        {
            AVAILABLE[j]=client_message[i][j];
		}
        AVAILABLE[9]='\0';
        
        if(strcmp(BOOK,"BOOK\0")==0)
        controllo=0;
        if(strcmp(CANCEL,"CANCEL\0")==0)
        controllo=1;
        if(strcmp(AVAILABLE,"AVAILABLE\0")==0)
        controllo=2;
        }
        switch(controllo)
        {
            case 0://BOOK
            for(t=0;t<90;t++)
                {
                    if(ombrellone[t].stato==0)
                    {
                        conta_liberi++;
                    }
                }
            if(conta_liberi>0)//conta ombrelloni liberi
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
							for(t=0;t<2;t++)
                			{
                    			A[t]= client_message[i][t+5];
                    
               				}
                			numero_richiesta=atoi(A);
                            numero_richiesta--;
							strcpy(client_message[i]," ");
                        	if(ombrellone[numero_richiesta].stato==0 && numero_richiesta>0 && numero_richiesta <=90) //if(ombrellone[numero_richiesta].stato==0)
                        	{
                            	ombrellone[numero_richiesta].stato=2;//ombrellone temporaneamente occupato
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

										for(t=0;t<2;t++)
                						{
                    						A[t]= client_message[i][t+5];
                    
               							}
                						numero_richiesta1=atoi(A);
                                        numero_richiesta1--;
										strcpy(client_message[i], " ");
										if(numero_richiesta==numero_richiesta1)
										{
                                            if(client_message[i][17]=='\n' || client_message[i][18]=='\n')
                                            {
                                                for(j=8;j<=17;j++)
											    {
												    data_fine[z]=client_message[i][j];
												    z++;
											    }
                                                ombrellone[numero_richiesta].stato=1;
                                                strcpy(ombrellone[numero_richiesta].datainizio,data_inizio);
                                                strcpy(ombrellone[numero_richiesta].datafine,data_fine);
                                                
                                                //scrittura a file momentanea
                                                rewind(statospiaggia);
                                                for(t=0;t<90;t++)
                                                fprintf(statospiaggia,"%d %d %d %s %s\n",ombrellone[t].numero,ombrellone[t].riga,ombrellone[t].stato,ombrellone[t].datainizio,ombrellone[t].datafine);
											    
											    write(client_sock , "Prenotazione avvenuta con successo\nFINE", 40); 
                                            }
                                            else
                                            {
                                                for(j=8;j<=17;j++)
											    {
												    data_inizio[z]=client_message[i][j];
												    z++;
											    }
                                                z=0;
                                                for(j=19;j<=28;j++)
											    {
												    data_fine[z]=client_message[i][j];
												    z++;
											    } 
                                                ombrellone[numero_richiesta].stato=0;
                                                strcpy(ombrellone[numero_richiesta].datainizio,data_inizio);
                                                strcpy(ombrellone[numero_richiesta].datafine,data_fine);
                                                
                                                //scrittura a file momentanea
                                                rewind(statospiaggia);
                                                for(t=0;t<90;t++)
                                                fprintf(statospiaggia,"%d %d %d %s %s\n",ombrellone[t].numero,ombrellone[t].riga,ombrellone[t].stato,ombrellone[t].datainizio,ombrellone[t].datafine);
										
		                    				    write(client_sock , "Prenotazione avvenuta con successo\nFINE", 40);
											   

                                            }
											
										}
										else 
										{
											
		                    				write(client_sock ,"Hai selezionato un ombrellone diverso\nFINE",43);
                           					strcpy(client_message[i], " ");
										
										}
									}
									else if(strcmp(CANCEL,"CANCEL\0")==0)
									{
										ombrellone[numero_richiesta].stato=0; //ombrellone libero
		                    			write(client_sock ,"Prenotazione cancellata\nFINE", 29);
                           				strcpy(client_message[i], " ");
									}
                        		}
							}
                        	else
                       		{
    
		                    	write(client_sock ,"NAVAILABLE\nFINE", 16);
                           		strcpy(client_message[i], " ");
                       		}	
                    	}
						else
						{
					
		                    write(client_sock ,"Inserisci BOOK e il numero dell'ombrellone che vuoi prenotare\nFINE", 67);
                           	strcpy(client_message[i], " ");
						}
					}
					else
					{
						perror("errore ricezione messaggio\nFINE");
					}
				}
                else
                {
                   	
		            write(client_sock ,"NOK\nFINE", 9);
                    strcpy(client_message[i], "");
                    	//inserire ciclo con semaforo
                } 
        }	
        else
        {
            
		    write(client_sock ,"NAVAILABLE\nFINE",16);
            strcpy(client_message[i], "");
        }

             break;
            case 1://CANCEL



            break;
            case 2: //AVAILABLE
            if(client_message[i][9]=='\n' || client_message[i][10]=='\n'|| client_message[i][11]==32)
            {
                for(t=0;t<90;t++)
                {
                    if(ombrellone[t].stato==0)
                    {
                        conta_liberi++;
                    }
                }
                if(conta_liberi>0)
                {   	
					i++;
                    strcpy(client_message[i]," ");
                    sprintf(Liberi,"%s%d",Liberi,conta_liberi);
					strcpy(client_message[i],"AVAILABLE ");
					strcat(client_message[i],Liberi);
					strcat(client_message[i],"\nFINE");
                    write(client_sock , client_message[i], strlen(client_message[i]));
                    strcpy(client_message[i], " ");
			        i++;
                }
                else
                {
                    
		            write(client_sock ,"NAVAILABLE\nFINE",16);
                    strcpy(client_message[i], " ");
			        
                }
            }
            else
            {
                j=0;
                for(t=0;t<2;t++)
                {
                    A[t]= client_message[i][t+10];
                    
                }
                numero_richiesta=atoi(A);
                numero_richiesta--;
                if(numero_richiesta>0 && numero_richiesta<=90)
                {
                    for(t=0;t<90;t++)
                    {
                        if(ombrellone[t].riga==numero_richiesta && ombrellone[t].stato==0)
                        {
                            ombr_disponibili[j]=ombrellone[t].numero;
                            j++;
                        }
                    }
                    strcpy(Liberi," ");
                    strcpy(client_message[i]," ");
                    strcpy(client_message[i],"Ombrelloni disponibili\n");
                    for(j=0;j<10;j++)
                    {
                        sprintf(Liberi,"%s%d",Liberi,ombr_disponibili[j]);
                        t=strlen(Liberi);
                        Liberi[t]=32;
                    }
                    strcat(client_message[i],Liberi);
					strcat(client_message[i],"\nFINE");
		            write(client_sock , client_message[i], strlen(client_message[i]));
                    strcpy(client_message[i], " ");
			       
                }
                else
                {
                    
		            write(client_sock ,"La fila selezionata non esiste\nFINE",36);
                    strcpy(client_message[i], " ");
			        
                }
    
            }

            break;
            default:
            
		    write(client_sock ,"L'opzione scelta non è disponibile\nFINE",41);
            strcpy(client_message[i], " ");
			
            break;
        }
	
	if(read_size == 0)
	{
		puts("Il client si è disconnesso");
		fflush(stdout);
	}
	else if(read_size == -1)
	{
		perror("ricezione fallita");
	}
    fclose(statospiaggia);
	close(socket_desc);
	return 0;
}