#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <semaphore.h>
#include <pthread.h>
#include "funzioni.h"

int confrontaDate(Data, Data, Data, Data);
int CodiceData(Data);
int ControlloDate(Data);
Data StringToData(char *);

int generaNumeri()
{
    return rand() % 10;
}

Messaggio MSG;

void func_BOOK(int client_sock, Ombrellone *ombrellone, char data_inizio[SIZEMSG], Periodo **ausilio, sem_t mutex)
{
    srand(time(NULL));
    int i, j,t=0, limit, flag = 2, confronto, codnum;
    int read_size, num_ombrellone;
    char client_message[SIZEMSG], data_fine[16] = {0};
    char A[3] = {0};
    FILE *modifiche;
    Data DATAIN, DATAF,DATAOGGI;
    Periodo *prenotazione;

    strcpy(MSG.message, "Inizio Trattativa \n\nInserire BOOK e la data (GG/MM/AAAA) oppure (GG-MM-AAAA)"); //MSG cambiato!!!
    MSG.segnale = 0;
    write(client_sock, &MSG, sizeof(MSG));

    if ((read_size = recv(client_sock, client_message, SIZEMSG, 0)) > 0)
    {
        //IL PRIMO MESSAGGIO DEL CLIENT È IL COMANDO + LA DATA.
        //SE VENGONO INSERITE DUE DATE, IL PERIODO CONSIDERATO È L'INTERVALLO TRA LE DUE
        //SE VIENE INSERITA UNA DATA, IL PERIODO È TRA OGGI E LA DATA SCRITTA

        if (strncmp(client_message, "BOOK", 4) == 0 || strncmp(client_message, "book", 4) ==0)
        {
            prenotazione = (Periodo *)malloc(sizeof(Periodo));
            for (j = 4; j < 20; j++)
            {
                if (client_message[j] == '\n')
                {
                    limit = 1;
                    break;
                }
            }

            //DECODIFICA DEL MESSAGGIO DEL CLIENT, VEDE QUANTE DATE SONO STATE INSERITE
            //E SCRIVE NELLE STRINGHE data_inizio E data_fine
            DATAOGGI=StringToData(data_inizio);

            if (limit) //CASO DI INSERIMENTO DI UNA SOLA DATA
            {
                strtok(client_message, " ");
                strncpy(data_fine, strtok(NULL, " "), 10);
                DATAIN = DATAOGGI;
            }
            else
            {
                strtok(client_message, " ");
                strncpy(data_inizio, strtok(NULL, " "), 10);
                strncpy(data_fine, strtok(NULL, " "), 10);
                DATAIN = StringToData(data_inizio);
            }

            //CONVERTE LE STRINGHE PRECEDENTEMENTE AGGIORNATE IN STRUTTURE Data
            
            DATAF = StringToData(data_fine);
            if (ControlloDate(DATAIN) == 0 && ControlloDate(DATAF) == 0 && CodiceData(DATAIN) <= CodiceData(DATAF) && CodiceData(DATAOGGI)<=CodiceData(DATAF))
            {

                if(CodiceData(DATAIN)<CodiceData(DATAOGGI)){
                    strcpy(MSG.message, "Prima data inserita non accettabile, verrà contata come data di oggi");
                    write(client_sock, &MSG, sizeof(MSG));
                    DATAIN=DATAOGGI;
                }
                prenotazione->datainizio = DATAIN;
                prenotazione->datafine = DATAF;

                //SE IN QUEL PERIODO OGNI OMBRELLONE È OCCUPATO SCRIVO NAVAILABLE AL CLIENT
                for (i = 1; i < 91; i++)
                {
                    if (confrontaDate(ombrellone[i].tempo->datainizio, ombrellone[i].tempo->datafine, DATAIN, DATAF) != 0)
                        break;
                }
                if (i == 90)
                { //Da cambiare, non conta se l'ultimo ombrellone è libero
                    strcpy(MSG.message, "NAVAILABLE");
                    MSG.segnale = 1;
                    write(client_sock, &MSG, sizeof(MSG));
                    return;
                }

                //AVVIO DELLE CONDIZIONI
                strcpy(MSG.message, "\nInserisci:\nBOOK e il numero dell'ombrellone per terminare la prenotazione\nCANCEL per cancellare la prenotazione\n");
                MSG.segnale = 0;
                write(client_sock, &MSG, sizeof(MSG));

                if ((read_size = recv(client_sock, client_message, SIZEMSG, 0)) > 0)
                {

                    if (strncmp(client_message, "CANCEL", 6) == 0 || strncmp(client_message, "cancel", 6) == 0)
                    {
                        strcpy(MSG.message, "Prenotazione annullata");
                        MSG.segnale = 1;
                        write(client_sock, &MSG, sizeof(MSG));
                        free(prenotazione);
                        return;
                    }

                    //SE NON VIENE RIMESSO BOOK
                    if (strncmp(client_message, "BOOK", 4) != 0 && strncmp(client_message, "book", 4) != 0)
                    {
                        strcpy(MSG.message, "Richiesta inserita non valida"); //MSG cambiato!!!
                        MSG.segnale = 1;
                        write(client_sock, &MSG, sizeof(MSG));
                        return;
                    }

                    //SINTASSI PER IL RICEVIMENTO DEL MESSAGGIO
                    strtok(client_message, " ");
                    strcpy(A, strtok(NULL, " "));
                    num_ombrellone = atoi(A);
                    if (ombrellone[num_ombrellone].stato == 1)
                    {
                        strcpy(MSG.message, "TEMPORANEAMENTE OCCUPATO"); //MSG cambiato!!!
                        MSG.segnale = 1;
                        write(client_sock, &MSG, sizeof(MSG));
                        return;
                    }
                    ombrellone[num_ombrellone].stato = 1;

                    if (CodiceData(ombrellone[num_ombrellone].tempo->datainizio) != 0)
                    { //SE È GIÀ STATA EFFETTUATA ALMENO UNA PRENOTAZIONE SU QUELL'OMBRELLONE...
                        confronto = confrontaDate(prenotazione->datainizio, prenotazione->datafine, ombrellone[num_ombrellone].tempo->datainizio, ombrellone[num_ombrellone].tempo->datafine);

                        if (confronto == -1)
                        {
                            flag = 1;
                        }
                        if (confronto == 0)
                        {
                            flag = 0;
                        }
                        while (ausilio[num_ombrellone]->next != NULL && confronto == 1)
                        {
                            confronto = confrontaDate(prenotazione->datainizio, prenotazione->datafine, ausilio[num_ombrellone]->next->datainizio, ausilio[num_ombrellone]->next->datafine);
                            if (confronto == -1)
                            {
                                break;
                            }
                            if (confronto == 0)
                            {
                                flag = 0;
                                break;
                            }
                            ausilio[num_ombrellone] = ausilio[num_ombrellone]->next;
                            flag++;
                        }
                    }
                    else
                    {
                        flag = -1;
                    }

                    // pthread_mutex_lock(&mutex);
                    sem_wait(&mutex);

                    if ((modifiche = fopen("aggiornamenti.txt", "a")) == NULL)
                    {
                        printf("errore apertura file\n");
                        exit(-1);
                    }

                    switch (flag)
                    {
                    case -1: //VIENE EFFETTUATA LA PRIMA PRENOTAZIONE DI QUELL'OMBRELLONE
                        prenotazione->codice[0] = '0' + (ombrellone[num_ombrellone].numero / 10);
                        prenotazione->codice[1] = '0' + (ombrellone[num_ombrellone].numero % 10);
                        for (i = 2; i < 5; i++)
                        {
                            codnum = generaNumeri();
                            prenotazione->codice[i] = codnum + '0';
                        }
                        strcpy(client_message, "Prenotazione eseguita\nID di prenotazione: ");
                        strcat(client_message, prenotazione->codice);
                        strcpy(MSG.message, client_message);
                        MSG.segnale = 1;
                        write(client_sock, &MSG, sizeof(MSG));

                        fprintf(modifiche, "%d %d", ombrellone[num_ombrellone].numero, ombrellone[num_ombrellone].fila);
                        fprintf(modifiche, "\t%s %d/%d/%d %d/%d/%d", prenotazione->codice, prenotazione->datainizio.giorno, prenotazione->datainizio.mese, prenotazione->datainizio.anno, prenotazione->datafine.giorno, prenotazione->datafine.mese, prenotazione->datafine.anno);
                        fprintf(modifiche, "\n");
                        break;

                    case 0: //L'OMBRELLONE SCELTO È GIA STATO PRESO PER QUEL PERIODO
                        strcpy(MSG.message, "Ombrellone non disponibile, già occupato per quel periodo");
                        MSG.segnale = 1;
                        write(client_sock, &MSG, sizeof(MSG));
                        ombrellone[num_ombrellone].stato == 0;
                        free(prenotazione);
                        break;

                    default: //TUTTI GLI ALTRI CASI
                        ausilio[num_ombrellone] = ombrellone[num_ombrellone].tempo;
                        prenotazione->codice[0] = '0' + (ombrellone[num_ombrellone].numero / 10);
                        prenotazione->codice[1] = '0' + (ombrellone[num_ombrellone].numero % 10);
                        for (i = 2; i < 5; i++)
                        {
                            codnum = generaNumeri();
                            prenotazione->codice[i] = codnum + '0';
                        }
                        strncpy(prenotazione->codice, prenotazione->codice, 5);
                        strcpy(client_message, "Prenotazione eseguita\nID di prenotazione: ");
                        strcat(client_message, prenotazione->codice);
                        strcpy(MSG.message, client_message);
                        MSG.segnale = 1;
                        write(client_sock, &MSG, sizeof(MSG));
                        fprintf(modifiche, "%d %d", ombrellone[num_ombrellone].numero, ombrellone[num_ombrellone].fila);
                        for (i = 1; i < flag; i++)
                        {
                            fprintf(modifiche, "\t%s %d/%d/%d %d/%d/%d", ausilio[num_ombrellone]->codice, ausilio[num_ombrellone]->datainizio.giorno, ausilio[num_ombrellone]->datainizio.mese, ausilio[num_ombrellone]->datainizio.anno, ausilio[num_ombrellone]->datafine.giorno, ausilio[num_ombrellone]->datafine.mese, ausilio[num_ombrellone]->datafine.anno);
                            ausilio[num_ombrellone] = ausilio[num_ombrellone]->next;
                        }
                        fprintf(modifiche, "\t%s %d/%d/%d %d/%d/%d", prenotazione->codice, prenotazione->datainizio.giorno, prenotazione->datainizio.mese, prenotazione->datainizio.anno, prenotazione->datafine.giorno, prenotazione->datafine.mese, prenotazione->datafine.anno);
                        while (ausilio[num_ombrellone] != NULL)
                        {
                            fprintf(modifiche, "\t%s %d/%d/%d %d/%d/%d", ausilio[num_ombrellone]->codice, ausilio[num_ombrellone]->datainizio.giorno, ausilio[num_ombrellone]->datainizio.mese, ausilio[num_ombrellone]->datainizio.anno, ausilio[num_ombrellone]->datafine.giorno, ausilio[num_ombrellone]->datafine.mese, ausilio[num_ombrellone]->datafine.anno);
                            ausilio[num_ombrellone] = ausilio[num_ombrellone]->next;
                        }
                        fprintf(modifiche, "\n");
                        ausilio[num_ombrellone] = ombrellone[num_ombrellone].tempo;

                        break;
                    }
                    fclose(modifiche);
                    //pthread_mutex_unlock(&mutex);
                    sem_post(&mutex);
                }
                else
                {
                    strcpy(MSG.message, "Messaggio non ricevuto");
                    write(client_sock, &MSG, sizeof(MSG));
                }
            }
            else
            {
                strcpy(MSG.message, "Le date inserite non sono corrette");
                MSG.segnale = 1;
                write(client_sock, &MSG, sizeof(MSG));
            }
        }

        else if (strncmp(client_message, "CANCEL", 6) == 0 || strncmp(client_message, "cancel", 6) == 0)
        {
            strcpy(MSG.message, "Prenotazione annullata");
            MSG.segnale = 1;
            write(client_sock, &MSG, sizeof(MSG));
            return;
        }
        else
        {
            strcpy(MSG.message, "Richiesta inserita non valida"); //MSG cambiato!!!
            MSG.segnale = 1;
            write(client_sock, &MSG, sizeof(MSG));
        }
    }
    else
    {
        strcpy(MSG.message, "Messaggio non ricevuto");
        write(client_sock, &MSG, sizeof(MSG));
    }
}

void func_CANCEL(int client_sock, Ombrellone *ombrellone, sem_t mutex)
{

    FILE *modifiche;
    char client_message[SIZEMSG];
    int num_ombrellone, read_size;
    int i;
    char cod[5], A[3], ID[5];
    Periodo *aus;
    Periodo *aus2;

    strcpy(MSG.message, "Inizio Trattativa\nInserire CANCEL e l'ID di prenotazione"); //MSG cambiato!!!
    MSG.segnale = 0;
    write(client_sock, &MSG, sizeof(MSG));

    if ((read_size = recv(client_sock, client_message, SIZEMSG, 0)) > 0)
    {

        if (strncmp(client_message, "CANCEL", 6) == 0 || strncmp(client_message, "cancel", 6) == 0)
        {
            for (i = 0; i < 2; i++)
                A[i] = client_message[i + 7];
            A[3] = '\0';
            num_ombrellone = atoi(A);
            //printf("OMBRELLONE NUM.%s\n", A);

            for (i = 0; i < 5; i++)
                cod[i] = client_message[i + 7];
            cod[5] = '\0';
            //printf("CODICE SCRITTO:%s\n", cod);

            aus = ombrellone[num_ombrellone].tempo;
            while (aus != NULL && strncmp(aus->codice, cod, 5) != 0)
            {
                aus = aus->next;
            }
            if (aus == NULL)
            {
                strcpy(MSG.message, "CODICE INSERITO ERRATO, PRENOTAZIONE INESISTENTE");
                MSG.segnale = 1;
                write(client_sock, &MSG, sizeof(MSG));
            }
            else
            {
                if (ombrellone[num_ombrellone].stato == 1)
                    {
                        strcpy(MSG.message, "Ombrellone in prenotazione da un altro cliente. Riprova più tardi"); //MSG cambiato!!!
                        MSG.segnale = 1;
                        write(client_sock, &MSG, sizeof(MSG));
                        return;
                    }

                strcpy(MSG.message, "CODICE INSERITO ACCETTATO, CANCELLAZIONE IN CORSO");
                MSG.segnale = 1;
                write(client_sock, &MSG, sizeof(MSG));

                //  pthread_mutex_lock(&mutex);
                sem_wait(&mutex);

                if ((modifiche = fopen("aggiornamenti.txt", "a")) == NULL)
                {
                    printf("errore apertura file\n");
                    exit(-1);
                }

                aus2 = ombrellone[num_ombrellone].tempo;
                fprintf(modifiche, "%d %d", ombrellone[num_ombrellone].numero, ombrellone[num_ombrellone].fila);
                if (aus2->next == NULL)
                {                                              //VIENE CANCELLATA L'UNICA PRENOTAZIONE EFFETTUATA SU QUELL'OMBRELLONE
                    fprintf(modifiche, "\t00000 0/0/0 0/0/0"); //SCRIVO LO STATO DI DEFAULT
                }
                else
                {
                    while (aus2 != NULL)
                    { //VIENE CANCELLATA UNA PRENOTAZIONE TRA LE VARIE
                        if (aus2 != aus)
                        {
                            fprintf(modifiche, "\t%s %d/%d/%d %d/%d/%d", aus2->codice, aus2->datainizio.giorno, aus2->datainizio.mese, aus2->datainizio.anno, aus2->datafine.giorno, aus2->datafine.mese, aus2->datafine.anno);
                            aus2 = aus2->next;
                        }
                        else
                            aus2 = aus2->next;
                    }
                }
                fprintf(modifiche, "\n");

                fclose(modifiche);

                sem_post(&mutex);
                //pthread_mutex_unlock(&mutex);
            }
        }
        else
        {
            strcpy(MSG.message, "Richiesta inserita non valida (CANCEL non inserito)");
            MSG.segnale = 1;
            write(client_sock, &MSG, sizeof(MSG));
        }
    }
    else
    {
        strcpy(MSG.message, "Messaggio non ricevuto");
        write(client_sock, &MSG, sizeof(MSG));
    }
}


void func_AVAILABLE(int client_sock, char richiesta[SIZEMSG], char data_oggi[12], Periodo **ausilio, Ombrellone *ombrellone)
{
    int i, j = 0, t = 0,p=0, limit = 0, z = 0, lenght, cifra = 0, read_size, flag = 0;
    char client_message[SIZEMSG];
    Messaggio Msg;
    int n_ombr_disponibili = 0, fila_ombr;
    int ombr_disponibili[10] = {0};
    char Liberi[30] = {0};
    char data_inizio[12] = {0};
    char data_fine[12] = {0};
    Data data_in, data_fin, today;
    char ombr[2];
    char A[2] = {0};
    Periodo *prenotazione;
    prenotazione = (Periodo *)malloc(sizeof(Periodo));
    p=strlen(richiesta);

    if(p != 9)
    {
        strtok(richiesta," ");
        strcpy(A, strtok(NULL, " "));
        fila_ombr = atoi(A);
    }
    

    strcpy(MSG.message, "Inserisci AVAILABLE e il periodo in cui visualizzare gli ombrelloni disponibili");
    MSG.segnale = 0;
    write(client_sock, &MSG, sizeof(MSG));

    //Inserimento data
    if (read_size = recv(client_sock, client_message, SIZEMSG, 0) < 0)
    {
        strcpy(MSG.message, "Messaggio non ricevuto");
        MSG.segnale = 1;
        write(client_sock, &MSG, sizeof(MSG));
        return;
    }
    if (strncmp(client_message, "AVAILABLE", 9) != 0 && strncmp(client_message, "available", 9) != 0)
    {
        strcpy(MSG.message, "Compilazione della richiesta errata(AVAILABLE non inserito)");
        MSG.segnale = 1;
        write(client_sock, &MSG, sizeof(MSG));
        return;
    }
    for (j = 9; j < 24; j++)
    {
        if (client_message[j] == '\n')
        {
            limit = 1;
            break;
        }
    }
    if (limit)
    {
        strtok(client_message, " ");
        strncpy(data_fine, strtok(NULL, " "), 10);
        strncpy(data_inizio,data_oggi,10);
        t = strlen(data_fine);
        data_fine[t] = '\0';
    }
    else
    {
        strtok(client_message, " ");
        strncpy(data_inizio, strtok(NULL, " "), 10);
        strncpy(data_fine, strtok(NULL, " "), 10);
        t = strlen(data_inizio);
        data_inizio[t] = '\0';
        t = strlen(data_fine);
        data_fine[t] = '\0';
        
    }
    today = StringToData(data_oggi);
    data_in = StringToData(data_inizio);
    data_fin = StringToData(data_fine);
    if (ControlloDate(data_in) != 0 || ControlloDate(data_fin) != 0 || CodiceData(data_in) > CodiceData(data_fin) || CodiceData(today) > CodiceData(data_in))
    {
        strcpy(MSG.message, "La data inserita non è corretta");
        MSG.segnale = 1;
        write(client_sock, &MSG, sizeof(MSG));
        return;
    }

    if (p==9) //SE È STATO INSERITO SOLO AVAILABLE SENZA SPECIFICHE
    {
        prenotazione->datainizio = data_in;
        prenotazione->datafine = data_fin;
        for (i = 1; i < 91; i++)
        {
            flag = 0;
            while (ausilio[i] != NULL)
            {
                if (confrontaDate(prenotazione->datainizio, prenotazione->datafine, ausilio[i]->datainizio, ausilio[i]->datafine) == 0)
                    flag++;
                ausilio[i] = ausilio[i]->next;
            }
            if (flag == 0)
                n_ombr_disponibili++;
            ausilio[i] = ombrellone[i].tempo;
        }

        if (n_ombr_disponibili > 0)
        { //SE CI SONO OMBRELLONI DISPONIBILI
            strcpy(MSG.message, "Liberi: ");
            sprintf(ombr, "%d", n_ombr_disponibili);
            strcat(MSG.message, ombr);
            MSG.segnale = 1;
            write(client_sock, &MSG, sizeof(MSG));
        }
        else
        { //SE NON CE NE SONO
            strcpy(MSG.message, "NAVAILABLE");
            MSG.segnale = 1;
            write(client_sock, &MSG, sizeof(MSG));
        }
    }
    else
    { //SE È SPECIFICATA QUALE FILA DI OMBRELLONI CERCARE

        j = 0;
        prenotazione->datainizio = data_in;
        prenotazione->datafine = data_fin;
        if (fila_ombr > 0 && fila_ombr < 10)
        {
            for (t = 1; t < 91; t++)
            {
                if (ombrellone[t].fila == fila_ombr)
                {
                    while (ausilio[t] != NULL)
                    {
                        if (confrontaDate(prenotazione->datainizio, prenotazione->datafine, ausilio[t]->datainizio, ausilio[t]->datafine) != 0)
                        {
                            ombr_disponibili[j] = ombrellone[t].numero;
                            j++;
                        }
                        ausilio[t] = ausilio[t]->next;
                    }
                    ausilio[t] = ombrellone[t].tempo;
                }
            }
            strcpy(Liberi, " ");
            strcpy(client_message, " ");
            strcpy(client_message, "Ombrelloni disponibili\n");
            for (z = 0; z < j; z++)
            {
                sprintf(Liberi, "%s%d", Liberi, ombr_disponibili[z]);
                t = strlen(Liberi);
                Liberi[t] = 32;
            }
            strcat(client_message, Liberi);
            strcpy(MSG.message, client_message);
            MSG.segnale = 1;
            write(client_sock, &MSG, sizeof(MSG));
        }
        else
        {
            strcpy(MSG.message, "La fila selezionata non esiste");
            MSG.segnale = 1;
            write(client_sock, &MSG, sizeof(MSG));
        }
    }
}


int confrontaDate(Data inizio, Data fine, Data occupatoi, Data occupatof)
{
    if (CodiceData(inizio) > CodiceData(occupatof)) //se la prenotazione richiesta è DOPO di quella effettuata, restituisco 1
        return 1;
    if (CodiceData(fine) < CodiceData(occupatoi)) //se la prenotazione richiesta è PRIMA di quella effettuata, restituisco -1
        return -1;
    return 0;
}

int CodiceData(Data data)
{
    return data.anno * 100000 + data.mese * 1000 + data.giorno;
}

Data StringToData(char *calenda)
{
    char A[6] = {0};
    Data DATA;
    if (strlen(calenda) < 8)
    {
        DATA.giorno = 0;
        DATA.mese = 0;
        DATA.anno = 0;
        return DATA;
    }
    if (calenda[1] == '/' || calenda[2] == '/')
    {
        strcpy(A, strtok(calenda, "/"));
        //printf("giorno %s\n", A);
        DATA.giorno = atoi(A);

        strcpy(A, strtok(NULL, "/"));
        //printf("mese %s\n", A);
        DATA.mese = atoi(A);

        strcpy(A, strtok(NULL, "/"));
        //printf("anno %s\n", A);
        DATA.anno = atoi(A);
    }
    else if (calenda[1] == '-' || calenda[2] == '-')
    {
        strcpy(A, strtok(calenda, "-"));
        //printf("giorno %s\n", A);
        DATA.giorno = atoi(A);

        strcpy(A, strtok(NULL, "-"));
        //printf("mese %s\n", A);
        DATA.mese = atoi(A);

        strcpy(A, strtok(NULL, "-"));
        //printf("anno %s\n", A);
        DATA.anno = atoi(A);
    }
    else
    {
        DATA.giorno = 0;
        DATA.mese = 0;
        DATA.anno = 0;
        return DATA;
    }

    return DATA;
}

int ControlloDate(Data data)
{ // se data è OK restituisce 0 altrimenti restituisce 1;
    if (data.mese <= 0 || data.mese > 12)
        return 1;

    if (data.mese == 1 || data.mese == 3 || data.mese == 5 || data.mese == 7 || data.mese == 8 || data.mese == 10 || data.mese == 12)
        if (data.giorno <= 0 || data.giorno > 31)
            return 1;
    if (data.mese == 4 || data.mese == 6 || data.mese == 9 || data.mese == 11)
        if (data.giorno <= 0 || data.giorno > 30)
            return 1;
    if (data.mese == 2) // verifica anni bisestili
    {
        if ((data.anno % 4 == 0 && data.anno % 100 != 0) || (data.anno % 100 == 0 && data.anno % 400 == 0))
        {
            if (data.giorno <= 0 || data.giorno > 29)
                return 1;
        }
        else
        {
            if (data.giorno <= 0 || data.giorno > 28)
                return 1;
        }
    }

    return 0;
}
