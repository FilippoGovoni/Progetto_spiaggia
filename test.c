//-------------------------
  // File:  test.c
  // Comp.: gcc test.c -otest
  // Run:   test
  //-------------------------
  #include <stdio.h>
  #include<stdlib.h>  
  #include <syslog.h>

  int main()
  {
    FILE *f;
    //Apro la connessione al sistema di logging nella modalita' voluta
    openlog("test", LOG_CONS || LOG_PID, LOG_DAEMON);
    //Scrivo il messaggio
    syslog(LOG_DAEMON || LOG_WARNING,"Ciao questa e' una prova di Gabriele");
    //Chiudo la connessione
    closelog();
  }