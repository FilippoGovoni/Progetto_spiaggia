#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

int main(void)
{
    int i=0;
    char c[5]={0};
    
    i=rand()%10;
    printf("numero %d\n",i);

    c[0]=i+'0';
    printf("carattere %s\n",c);
    return 0;
}