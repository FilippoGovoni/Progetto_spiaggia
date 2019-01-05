progetto: main.o
        gcc -o progetto main.o funzioni.o
main.o:main.c funzioni.h
gcc -c -g main.c

funzioni.o:funzioni.c funzioni.h
        gcc -c -g funzioni.c