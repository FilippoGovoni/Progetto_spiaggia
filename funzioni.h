#define SIZEMSG 200
typedef struct
{
    int giorno;
    int mese;
    int anno;
} Data;

struct _periodo
{
    Data datainizio;
    Data datafine;
    char codice[5];
    struct _periodo *next;
};
typedef struct _periodo Periodo;

typedef struct
{
    int numero;
    int fila;
    int stato; // se stato = 1 l'ombrellone è temporaneamente occupato
    Periodo *tempo;
} Ombrellone;

typedef struct
{
    char Data[12];
    int sock;
} parametri;

typedef struct
{
    char message[SIZEMSG];
    int segnale;
} Messaggio;

void func_BOOK(int client_sock, Ombrellone *ombrellone, char data_inizio[SIZEMSG], Periodo **ausilio, sem_t mutex);
void func_CANCEL(int client_sock, Ombrellone *ombrellone, sem_t mutex);
void func_AVAILABLE(int client_sock, char richiesta[SIZEMSG],char data_oggi[12], Periodo **ausilio, Ombrellone *ombrellone);
int CodiceData(Data data);
Data StringToData(char *calenda);