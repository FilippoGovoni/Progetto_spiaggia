#include<stdio.h>
#include<stdlib.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write
#include<time.h>
#include"funzioni.h"


#define Numero_Ombrelloni 90
#define Numero_righe 9
#define Numero_colonne 10


int confrontaDate(char datain1[],char datafin1[],char datain2[],char datafin2[])
{
    int i=0,z=0;
    periodo A,B;
    int ANNO,MESE,GIORNO;
    for(i=0;i<2;i++)
    {
        A.giornoinizio[z]=datain1[i];
        A.giornofine[z]=datafin1[i];
        B.giornoinizio[z]=datain2[i];
        B.giornofine[z]=datafin2[i];
        z++;
    }
    z=0;
    for(i=3;i<5;i++)
    {
        A.meseinizio[z]=datain1[i];
        A.mesefine[z]=datafin1[i];
        B.meseinizio[z]=datain2[i];
        B.mesefine[z]=datafin2[i];
        z++;
    }
    z=0;
    for(i=7;i<=10;i++)
    {
        A.annoinizio[z]=datain1[i];
        A.annofine[z]=datafin1[i];
        B.annoinizio[z]=datain2[i];
        B.annofine[z]=datafin2[i];
        z++;
    }
    A.ai=atoi(A.annoinizio);
    A.af=atoi(A.annofine);
    B.ai=atoi(B.annoinizio);
    B.af=atoi(B.annofine);
    A.mi=atoi(A.meseinizio);
    A.mf=atoi(A.mesefine);
    B.mi=atoi(B.meseinizio);
    B.mf=atoi(B.mesefine);
    A.gi=atoi(A.giornoinizio);
    A.gf=atoi(A.giornofine);
    B.gi=atoi(B.giornoinizio);
    B.gf=atoi(B.giornofine);

    if((B.ai>=A.ai && B.ai <= A.af) && (B.af>=A.ai && B.af<=A.af))
    {
        if((B.mi>=A.mi && B.mi<=A.mf) && (B.mf <=A.mf && B.mf>= A.mi))
        {
            if((B.gi>=A.gi && B.gi<=A.gf) && (B.gf<=A.gf && B.gf >=A.gi))
            {
                return 0;
            }
            else
            return 1;
        }
        else
        return 1;
    }
    else
    return 1;
    
}

int main(void)
{
	int socket_desc,client_sock,c,read_size;
    int i=0,j,controllo=3,t=0;
	struct sockaddr_in server , client;
    FILE* statospiaggia;
	char client_message[100][2000];
    char BOOK[5]={0};
    char CANCEL[7]={0};
    char AVAILABLE[10]={0};
    char data_inizio[20]={0};
    Ombrellone ombrellone[90];
    time_t temp;

    srand((unsigned) time(&temp));

    //lettura dati iniziali spiggia

    if((statospiaggia=fopen("stato_spiaggia.txt","r+"))==NULL)
    {
        printf("errore apertura file\n");
        exit(-1);
    }
    while(!feof(statospiaggia))
    {
        fscanf(statospiaggia,"%d %d %d %s %s %s",&ombrellone[i].numero,&ombrellone[i].riga,&ombrellone[i].stato,ombrellone[i].codice,ombrellone[i].datainizio,ombrellone[i].datafine);
        i++;
    }
    printf("Inserisci la data di oggi\n");
    fgets(data_inizio,15,stdin);
    t=strlen(data_inizio);
    data_inizio[t-1]='\0';

    //controllo stato ombrellone inizio
    for(t=0;t<90;t++)
    {
        if(strcmp(ombrellone[t].datainizio,data_inizio)==0)
        {
            ombrellone[t].stato=1;
        }
    }
     //controllo stato ombrellone fine
    for(t=0;t<90;t++)
    {
        if(strcmp(ombrellone[t].datafine,data_inizio)==0)
        {
            ombrellone[t].stato=0;
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
            func_BOOK(client_sock,statospiaggia,ombrellone);
            break;
            case 1://CANCEL
            func_CANCEL(client_sock,client_message[i],ombrellone);
            break;
            case 2: //AVAILABLE
            func_AVAILABLE(client_sock,client_message[i],ombrellone);
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