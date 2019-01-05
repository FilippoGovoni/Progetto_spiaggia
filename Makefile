progetto: main.o funzioni.o	
	gcc -g -o progetto main.o funzioni.o 
funzioni.o: funzioni.c funzioni.h	
	gcc -pthread -g -c funzioni.c        
main.o: main.c funzioni.h	
	gcc -pthread -g -c main.c 
