#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(void)
{
    int i;
    char A[30];

    printf("inserisci una stringa\n");
    fgets(A,30,stdin);
    printf("primo %s\n",A);

   strcpy(A," ");
    printf("secondo %s\n",A);


    return 0;
}