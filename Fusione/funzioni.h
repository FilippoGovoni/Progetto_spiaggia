#define SIZEMSG 60
typedef struct{
    int giorno;
    int mese;
    int anno;
}Data;


struct _periodo{
    Data datainizio;
    Data datafine;
    struct _periodo * next;
};
typedef struct _periodo Periodo;

typedef struct{
    int numero;
    int fila;
    int stato;
    char codice[6];
    Periodo * tempo;
}Ombrellone;

typedef struct
{
    char Data[20];      //DA MODIFICAREEEEE
    int sock;
}parametri;

void func_BOOK(int client_sock,Ombrellone *ombrellone,char data_inizio[SIZEMSG],Periodo ** ausilio);
void func_CANCEL(int client_sock,char richiesta[SIZEMSG],Ombrellone *ombrellone);
void func_AVAILABLE(int client_sock,char richiesta[SIZEMSG],Ombrellone *ombrellone);

