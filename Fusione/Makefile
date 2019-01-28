progetto: main.o funzioni.o	
	gcc -g -o progetto main.o funzioni.o -lpthread 
funzioni.o: funzioni.c funzioni.h	
	gcc -g -c funzioni.c         
main.o: main.c funzioni.h	
	gcc -g -c main.c 
