progetto: server client.o
server: server.o funzioni.o thpool.o	
	gcc -g -o progetto server.o funzioni.o thpool.o -lrt -lpthread -o server
funzioni.o: funzioni.c funzioni.h	
	gcc -g -c funzioni.c         
server.o: server.c funzioni.h thpool.h	
	gcc -g -c server.c 
thpool.o: thpool.c thpool.h
	gcc -g -c thpool.c
client.o: client.c
	gcc -g -c client.c
