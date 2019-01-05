#include<stdio.h>
#include<stdlib.h>
#include"funzioni.h"


int generaNumeri()
{
    return rand() % 10;
}

void func_BOOK(int client_sock,FILE *statospiaggia,Ombrellone *ombrellone)
{
    char A[2];
    int i=0,j,z=0,t=0,flag=0,conta_liberi,read_size,codnum,numero_richiesta,numero_richiesta1;
    char client_message[1000][1000];
    char BOOK[5]={0};
    char AVAILABLE[10]={0};
    char CANCEL[7]={0};
    char cod[5];
    char data_fine[20]={0};
    char data_inizio[20]={0};
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
                        	if(ombrellone[numero_richiesta].stato==0 && numero_richiesta>=-0 && numero_richiesta <=90) //if(ombrellone[numero_richiesta].stato==0)
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
                                                for(j=7;j<=16;j++)
											    {
												    data_fine[z]=client_message[i][j];
												    z++;
											    }
                                                ombrellone[numero_richiesta].stato=1;
                                                strcpy(ombrellone[numero_richiesta].datainizio,data_inizio);
                                                strcpy(ombrellone[numero_richiesta].datafine,data_fine);
                                                //generazione codice di cancellazione
                                                for(t=0;t<5;t++)
                                                {
                                                    codnum=generaNumeri();
                                                    cod[t]=codnum+'0';
                                                }
                                                strcpy(ombrellone[numero_richiesta].codice,cod);
                                                strcpy(client_message[i],"Prenotazione avvenuta con successo\n il tuo codice di cancellazione è:  ");
                                                strcat(client_message[i],cod);
                                                strcat(client_message[i],"\nFINE");
                                                //scrittura a file momentanea
                                                rewind(statospiaggia);
                                                for(t=0;t<90;t++)
                                                fprintf(statospiaggia,"%d %d %d %s %s %s\n",ombrellone[t].numero,ombrellone[t].riga,ombrellone[t].stato,ombrellone[t].codice,ombrellone[t].datainizio,ombrellone[t].datafine);
											    
											    write(client_sock ,client_message[i], strlen(client_message[i])); 
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
                                                //generazione codice
                                                for(t=0;t<5;t++)
                                                {
                                                    codnum=generaNumeri();
                                                    cod[t]=codnum+'0';
                                                }
                                                strcpy(ombrellone[numero_richiesta].codice,cod);
                                                strcpy(client_message[i],"Prenotazione avvenuta con successo\n il tuo codice di cancellazione è:  ");
                                                strcat(client_message[i],cod);
                                                strcat(client_message[i],"\nFINE");
                                                //scrittura a file momentanea
                                                rewind(statospiaggia);
                                                for(t=0;t<90;t++)
                                                fprintf(statospiaggia,"%d %d %d %s %s %s\n",ombrellone[t].numero,ombrellone[t].riga,ombrellone[t].stato,ombrellone[t].codice,ombrellone[t].datainizio,ombrellone[t].datafine);
										
		                    				    write(client_sock , client_message[i],strlen(client_message[i]));
											   

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
            
        }

}

void func_CANCEL(int client_sock,char richiesta[2000],Ombrellone *ombrellone)
{
    char A[2];
    char cod[6];
    char client_message[100][2000];
    int numero_richiesta,t,i=0,j,read_size;
    for(t=0;t<2;t++)
                {
                    A[t]= richiesta[t+7];
                    
                }
                numero_richiesta=atoi(A);
                numero_richiesta--;
                if(ombrellone[numero_richiesta].stato==0 || ombrellone[numero_richiesta].stato==2 )
                {
                    write(client_sock ,"L'ombrellone non può essere disdetto\nFINE",43);
                }
                else
                {
                    write(client_sock ,"Inserisci il codice per disdire la prenotazione\n",43);
                    if((read_size = recv(client_sock , client_message[i] , 2000 , 0))>0)
                    {
                        for(j=0;j<5;j++)
                        {
                            cod[j]=client_message[i][j];
                        }
                        if((strcmp(ombrellone[numero_richiesta].codice,cod))==0)
                        {
                            ombrellone[numero_richiesta].stato=0;
                            write(client_sock ,"CANCEL OK\nFINE",15);
                        }
                        else
                        {
                            write(client_sock ,"Codice errato\nFINE",19);
                        }
                    
                    }
                }

}


void func_AVAILABLE(int client_sock,char richiesta[2000],Ombrellone *ombrellone)
{
    int i=0,j,numero_richiesta;
    int conta_liberi=0,t;
    char A[2];
    char Liberi[100]={0};
    char client_message[100][2000];
    int ombr_disponibili[10]={0};


    if(richiesta[9]=='\n' || richiesta[10]=='\n'|| richiesta[11]==32)
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

}