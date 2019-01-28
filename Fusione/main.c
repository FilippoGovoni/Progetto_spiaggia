#include<stdio.h>
#include<stdlib.h>
#include<string.h>	
#include<sys/socket.h>
#include<arpa/inet.h>	
#include<unistd.h>	
#include<signal.h>
#include<time.h>
#include<sys/types.h>
#include<pthread.h>
#include<semaphore.h>
#include"funzioni.h"

Ombrellone ombrellone[91];

void *gestore_client(void*);
void *filewriter(void*);

int main(){
    int socket_desc,client_sock,c;
    int i=0,j,t=0,pid;
    struct sockaddr_in server , client;
    FILE* statospiaggia;
	Periodo * ausilio[91];
	parametri dato;
    


    if((statospiaggia=fopen("stato_spiaggia.txt","r+"))==NULL)
    {
        printf("errore apertura file\n");
        exit(-1);
    }
	
	fprintf(statospiaggia,"RIGA 1\n");

    while(!feof(statospiaggia)){
        ombrellone[i].tempo=(Periodo *)malloc(sizeof(Periodo));
        fscanf(statospiaggia,"%d %d %d %s %d/%d/%d %d/%d/%d",&ombrellone[i].numero,&ombrellone[i].fila,&ombrellone[i].stato,ombrellone[i].codice,&ombrellone[i].tempo->datainizio.giorno,&ombrellone[i].tempo->datainizio.mese,&ombrellone[i].tempo->datainizio.anno,&ombrellone[i].tempo->datafine.giorno,&ombrellone[i].tempo->datafine.mese,&ombrellone[i].tempo->datafine.anno);
        ausilio[i]=ombrellone[i].tempo;

        while((fgetc(statospiaggia)!='\n') && (i<91)){
            ausilio[i]->next=(Periodo *)malloc(sizeof(Periodo));
            fscanf(statospiaggia,"%d/%d/%d %d/%d/%d",&ausilio[i]->next->datainizio.giorno,&ausilio[i]->next->datainizio.mese,&ausilio[i]->next->datainizio.anno,&ausilio[i]->next->datafine.giorno,&ausilio[i]->next->datafine.mese,&ausilio[i]->next->datafine.anno);
            ausilio[i]=ausilio[i]->next;
        }
       // ausilio[i]=ombrellone[i].tempo;      AUSILIO PUNTA ALL'ULTIMO ELEMENTO INSERITO NELLA RIGA
       i++;
    }
    rewind(statospiaggia);
    fprintf(statospiaggia,"RIGA 1\n");



    // Creazione thread per la gestione dei file
    if(pthread_create(&writer,NULL,filewriter,(void*)&dato)<0)
       {
           perror("errore creazione thread");
           return 2;
    }

    //Creazione socket
    socket_desc=socket(AF_INET , SOCK_STREAM , 0);
    if(socket_desc <0)
	{
		puts("errore creazione socket\n");
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
	listen(socket_desc ,8);
	
	//Attesa di connessioni
	puts("In attesa di connessioni...");
	c = sizeof(struct sockaddr_in);
	fclose(statospiaggia);
	//accept
	while(client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c))
    {
       printf("Connessione aperta con %d\n",client_sock);
       dato.sock=client_sock;
	   if(pthread_create(&cliente,NULL,gestore_client,(void*)&dato)<0)
       {
           perror("errore creazione thread");
           return -1;
       }
    }
    close(socket_desc);
    return 0;  
}

void *gestore_client(void *dato){
    parametri dati= *(parametri*)dato;
    int lenght,controllo;
	char client_message[15];

	//Inizio conversazione
	if( (read_size = recv(dati.sock,client_message,15,0))>0){
		lenght=strlen(client_message);
        client_message[lenght]='\0';


        if(strncmp(client_message,"BOOK",4)==0)
            controllo=0;
        if(strncmp(client_message,"CANCEL",6)==0)
            controllo=1;
        if(strncmp(client_message,"AVAILABLE",9)==0)
            controllo=2;
        switch(controllo){
            case 0://BOOK
            func_BOOK(dati.sock,ombrellone,dati.Data);
            break;
            case 1://CANCEL
            func_CANCEL(dati.sock,client_message,ombrellone);
            break;
            case 2: //AVAILABLE
            func_AVAILABLE(dati.sock,client_message,ombrellone);
            break;

            default:
		    write(dati.sock ,"L'opzione scelta non è disponibile\nFINE",36);
			
            break;
        }

	}
	if(read_size == 0){
		puts("Il client si è disconnesso");
		fflush(stdout);
	}
	else if(read_size == -1){
		perror("ricezione fallita");
	}
	
	if(close(dati.sock)<0)
    perror("errore chiusura socket\n");

    pthread_exit(NULL);

}




void *filewriter(void *dato){	//gli si potrebbe passare statospiaggia come parametro ed ottimizzare un poco il codice
    int i,t;
    FILE* statospiaggia;
	FILE* modifiche;
	Ombrellone Supporto;
	
	
	
	if((statospiaggia=fopen("stato_spiaggia.txt","r+"))==NULL)
    {
        printf("errore apertura file\n");
        exit(-1);
    }

	while(1){
		sleep(30);
        if((modifiche=fopen("aggiornamenti.txt","r"))==NULL)
        {
            printf("errore apertura file\n");
            exit(-1);
        }



		for(i=1;i<91;i++){
        fprintf(statospiaggia,"%d %d %d %s",ombrellone[i].numero,ombrellone[i].fila,ombrellone[i].stato,ombrellone[i].codice);
        while(ombrellone[i].tempo!=NULL){
            fprintf(statospiaggia,"\t%d/%d/%d %d/%d/%d",ombrellone[i].tempo->datainizio.giorno,ombrellone[i].tempo->datainizio.mese,ombrellone[i].tempo->datainizio.anno,ombrellone[i].tempo->datafine.giorno,ombrellone[i].tempo->datafine.mese,ombrellone[i].tempo->datafine.anno);
            ombrellone[i].tempo=ombrellone[i].tempo->next;
        }
        fprintf(statospiaggia,"\n");
	}


}