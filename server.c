#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>
#include "thpool.h"
#include "funzioni.h"

#define MAX_CLIENT 24

pthread_t writer;
sem_t mutex;
threadpool thpool;

Ombrellone ombrellone[91];
Periodo *ausilio[91];
Messaggio MSG;

void gestore_client(void *);
void filewriter();
char *convertitore_mese(char *);


int main()
{
    time_t ora;
    ora = time(NULL);
    char data[30] = {0};
    char giorno[3] = {0}, mese[4] = {0}, anno[5] = {0};
    int socket_desc, client_sock, c;
    int i = 1, j, t = 0;
    pid_t pid;
    struct sockaddr_in server, client;

    FILE *statospiaggia;

    parametri dato;
    MSG.segnale = 0;

    pid = fork();
    if (pid < 0)
        exit(EXIT_FAILURE);
    if (pid > 0)
    {
        usleep(500);
        exit(EXIT_SUCCESS);
    }
        

    thpool = thpool_init(MAX_CLIENT);

    sem_init(&mutex, 0, 1);

    if ((statospiaggia = fopen("stato_spiaggia.txt", "r+")) == NULL)
    {
        printf("errore apertura file\n");
        exit(-1);
    }

    fprintf(statospiaggia, "TABELLA OMBRELLONI: n°ombrellone-fila-codicepren-periodo\n");

    while (!feof(statospiaggia))
    {
        ombrellone[i].tempo = (Periodo *)malloc(sizeof(Periodo));
        fscanf(statospiaggia, "%d %d\t%s %d/%d/%d %d/%d/%d", &ombrellone[i].numero, &ombrellone[i].fila, ombrellone[i].tempo->codice, &ombrellone[i].tempo->datainizio.giorno, &ombrellone[i].tempo->datainizio.mese, &ombrellone[i].tempo->datainizio.anno, &ombrellone[i].tempo->datafine.giorno, &ombrellone[i].tempo->datafine.mese, &ombrellone[i].tempo->datafine.anno);
        ausilio[i] = ombrellone[i].tempo;
        while ((fgetc(statospiaggia) != '\n') && (i < 91))
        {
            ausilio[i]->next = (Periodo *)malloc(sizeof(Periodo));
            fscanf(statospiaggia, "\t%s %d/%d/%d %d/%d/%d", ausilio[i]->next->codice, &ausilio[i]->next->datainizio.giorno, &ausilio[i]->next->datainizio.mese, &ausilio[i]->next->datainizio.anno, &ausilio[i]->next->datafine.giorno, &ausilio[i]->next->datafine.mese, &ausilio[i]->next->datafine.anno);
            ausilio[i] = ausilio[i]->next;
        }
        ausilio[i] = ombrellone[i].tempo; // AUSILIO PUNTA ALL'ULTIMO ELEMENTO INSERITO NELLA RIGA
        i++;
    }
    fclose(statospiaggia); //SI CHIUDE SUBITO IL FILE APERTO, PER POI APRIRLO NUOVAMENTE NEL FILEWRITER
    strcpy(dato.Data, " ");
    printf("Data di oggi: \n");
    strtok(asctime(localtime(&ora)), " ");
    strcpy(mese, strtok(NULL, " "));
    strcpy(mese, convertitore_mese(mese));
    strcpy(giorno, strtok(NULL, " "));
    strtok(NULL, " ");
    strcpy(anno, strtok(NULL, " "));
    t = strlen(anno);
    anno[t] = '\0';

    strcpy(dato.Data, giorno);
    strcat(dato.Data, mese);
    printf("%s\n", dato.Data);
    // Creazione thread per la gestione dei file
    if (pthread_create(&writer, NULL, (void *)filewriter, NULL) < 0)
    {
        perror("errore creazione thread");
        return 2;
    }

    //Creazione socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc < 0)
    {
        puts("errore creazione socket\n");
    }
    puts("Socket OK");

    //creazione della struttura
    server.sin_family = AF_INET; //collegamento ipv 4
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(58000);

    //Bind
    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("bind fallita. Errore");
        return 1;
    }
    puts("bind OK");

    //Listen
    listen(socket_desc, MAX_CLIENT);

    //Attesa di connessioni
    puts("In attesa di connessioni...");
    c = sizeof(struct sockaddr_in);
    //accept
    while (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c))
    {
        send(client_sock, "ATTENDERE...", 13, 0);
        //printf("Connessione aperta con %d\n", client_sock);
        dato.sock = client_sock;

        while (thpool_num_threads_working(thpool) >= MAX_CLIENT)
        {
        }

        thpool_add_work(thpool, (void *)gestore_client, (void *)&dato);

        /*else
        {
            strcpy(MSG.message, "\nConnessione non disponibile ");
            MSG.segnale = 1;
            write(client_sock, &MSG, sizeof(MSG));
        }*/
    }
    thpool_destroy(thpool);
    close(socket_desc);
    return 0;
}


void gestore_client(void *dato)
{
    parametri dati = *(parametri *)dato;
    char Message[200]={0};
    int lenght, controllo = 3, read_size, nmsg = 0;

    //Inizio conversazione

    if ((read_size = recv(dati.sock,Message, 200, 0)) > 0)
    {
        
        lenght = strlen(Message);
        Message[lenght] = '\0';

        if (strncmp(Message, "BOOK", 4) == 0 || strncmp(Message, "book", 4) == 0)
            controllo = 0;
        if (strncmp(Message, "CANCEL", 6) == 0 || strncmp(Message, "cancel", 6) == 0)
            controllo = 1;
        if (strncmp(Message, "AVAILABLE", 9) == 0 || strncmp(Message, "available", 9) == 0)
            controllo = 2;
        switch (controllo)
        {
        case 0: //BOOK
            func_BOOK(dati.sock, ombrellone, dati.Data, ausilio, mutex);

            break;

        case 1: //CANCEL

            func_CANCEL(dati.sock, ombrellone, mutex);

            break;

        case 2: //AVAILABLE
            func_AVAILABLE(dati.sock, Message, dati.Data, ausilio, ombrellone);
            break;

        default:
            MSG.segnale = 1;
            strcpy(MSG.message, "L'opzione scelta non è disponibile");
            write(dati.sock, &MSG, sizeof(MSG));

            break;
        }
    }
    else if (read_size == -1)
    {
        perror("ricezione fallita");
    }

    if (close(dati.sock) < 0)
        perror("errore chiusura socket\n");
}
void filewriter()
{
    int i, end_line, g;
    FILE *statospiaggia;
    FILE *modifiche;
    Ombrellone supporto;
    Periodo *aus;
    while (1)
    {
        sem_wait(&mutex);

        if ((modifiche = fopen("aggiornamenti.txt", "r")) == NULL)
        {
            printf("errore apertura file\n");
            exit(-1);
        }
        while (!feof(modifiche))
        {
            if (fscanf(modifiche, "%d %d", &supporto.numero, &supporto.fila) < 0)
            {
                //printf("File aggiornamenti vuoto (fscanf=%d)\n", g);
                break; //IL FILE È VUOTO
            }
            supporto.tempo = (Periodo *)malloc(sizeof(Periodo));
            fscanf(modifiche, "\t%s %d/%d/%d %d/%d/%d", supporto.tempo->codice, &supporto.tempo->datainizio.giorno, &supporto.tempo->datainizio.mese, &supporto.tempo->datainizio.anno, &supporto.tempo->datafine.giorno, &supporto.tempo->datafine.mese, &supporto.tempo->datafine.anno);
            aus = supporto.tempo;
            end_line = fgetc(modifiche);

            //printf("supporto ombr n.%d ha cambiato prenotazione:\n", supporto.numero);
            //printf("%d/%d/%d %d/%d/%d\n", supporto.tempo->datainizio.giorno, supporto.tempo->datainizio.mese, supporto.tempo->datainizio.anno, supporto.tempo->datafine.giorno, supporto.tempo->datafine.mese, supporto.tempo->datafine.anno);

            while (end_line != '\n' && end_line != EOF)
            {
                //printf("cicloinf \n");
                aus->next = (Periodo *)malloc(sizeof(Periodo));
                fscanf(modifiche, "\t%s %d/%d/%d %d/%d/%d", aus->next->codice, &aus->next->datainizio.giorno, &aus->next->datainizio.mese, &aus->next->datainizio.anno, &aus->next->datafine.giorno, &aus->next->datafine.mese, &aus->next->datafine.anno);
                aus = aus->next;
                end_line = fgetc(modifiche);
            }

            for (i = 1; i < 91; i++)
            {
                if (supporto.numero == ombrellone[i].numero)
                {
                    ombrellone[i] = supporto;
                    ombrellone[i].tempo = supporto.tempo;
                    ausilio[i] = supporto.tempo;
                    break;
                }
            }
            //supporto.numero=0;
            //printf("Ombrellone n.%d ha cambiato prenotazione:\t", ombrellone[i].numero);
            //printf("%d/%d/%d %d/%d/%d", ombrellone[i].tempo->datainizio.giorno, ombrellone[i].tempo->datainizio.mese, ombrellone[i].tempo->datainizio.anno, ombrellone[i].tempo->datafine.giorno, ombrellone[i].tempo->datafine.mese, ombrellone[i].tempo->datafine.anno);
            //printf("\t%d/%d/%d %d/%d/%d", ausilio[i]->datainizio.giorno, ausilio[i]->datainizio.mese, ausilio[i]->datainizio.anno, ausilio[i]->datafine.giorno, ausilio[i]->datafine.mese, ausilio[i]->datafine.anno);
            //if (ombrellone[i].tempo->next != NULL)
            //    printf("%d/%d/%d %d/%d/%d", ombrellone[i].tempo->next->datainizio.giorno, ombrellone[i].tempo->next->datainizio.mese, ombrellone[i].tempo->next->datainizio.anno, ombrellone[i].tempo->next->datafine.giorno, ombrellone[i].tempo->next->datafine.mese, ombrellone[i].tempo->next->datafine.anno);
            //printf("\n");
        }
        fclose(modifiche);

        sem_post(&mutex);

        //sleep(5);
        if ((modifiche = fopen("aggiornamenti.txt", "w")) == NULL)
        {
            printf("errore apertura file\n");
            exit(-1);
        }
        fclose(modifiche);

        if ((statospiaggia = fopen("stato_spiaggia.txt", "w")) == NULL)
        {
            printf("errore apertura file\n");
            exit(-1);
        }
        fprintf(statospiaggia, "TABELLA OMBRELLONI: n°ombrellone-fila-codicepren-periodo\n");
        for (i = 1; i < 91; i++)
        {
            ombrellone[i].stato = 0;
            fprintf(statospiaggia, "%d %d", ombrellone[i].numero, ombrellone[i].fila);
            while (ausilio[i] != NULL)
            {
                fprintf(statospiaggia, "\t%s %d/%d/%d %d/%d/%d", ausilio[i]->codice, ausilio[i]->datainizio.giorno, ausilio[i]->datainizio.mese, ausilio[i]->datainizio.anno, ausilio[i]->datafine.giorno, ausilio[i]->datafine.mese, ausilio[i]->datafine.anno);
                ausilio[i] = ausilio[i]->next;
            }
            ausilio[i] = ombrellone[i].tempo;
            fprintf(statospiaggia, "\n");
        }
        fclose(statospiaggia);

        //printf("AGGIORNATO STATOSPIAGGIA\n");
        sleep(30);
    }
    pthread_exit(NULL);
}
char *convertitore_mese(char *mese)
{
    if (strcmp(mese, "Gen") == 0)
        strcpy(mese, "-01-");
    if (strcmp(mese, "Feb") == 0)
        strcpy(mese, "-02-");
    if (strcmp(mese, "Mar") == 0)
        strcpy(mese, "-03-");
    if (strcmp(mese, "Apr") == 0)
        strcpy(mese, "-04-");
    if (strcmp(mese, "May") == 0)
        strcpy(mese, "-05-");
    if (strcmp(mese, "Jun") == 0)
        strcpy(mese, "-06-");
    if (strcmp(mese, "Jul") == 0)
        strcpy(mese, "-07-");
    if (strcmp(mese, "Aug") == 0)
        strcpy(mese, "-08-");
    if (strcmp(mese, "Sep") == 0)
        strcpy(mese, "-09-");
    if (strcmp(mese, "Oct") == 0)
        strcpy(mese, "-10-");
    if (strcmp(mese, "Nov") == 0)
        strcpy(mese, "-11-");
    if (strcmp(mese, "Dec") == 0)
        strcpy(mese, "-12-");

    return mese;
}