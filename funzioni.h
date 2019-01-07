typedef struct
{
    char giornoinizio[2];
    int gi;
    char meseinizio[2];
    int mi;
    char annoinizio[4];
    int ai;
    char giornofine[2];
    int gf;
    char mesefine[2];
    int mf;
    char annofine[4];
    int af;

}periodo;

typedef struct
{
    int numero;
    int riga;
    int colonna;
    int stato;   //0:libero 1:occupato 2:temporaneamente occupato
    char codice[6];
    char datainizio[20]; 
    char datafine[20];
}Ombrellone;

typedef struct
{
    char Data[20];
    int sock;
    Ombrellone ombrellone[90];
}parametri;

void func_BOOK(int client_sock,Ombrellone *ombrellone,char data_inizio[20]);
void func_CANCEL(int client_sock,char richiesta[2000],Ombrellone *ombrellone);
void func_AVAILABLE(int client_sock,char richiesta[2000],Ombrellone *ombrellone);

