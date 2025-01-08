#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <pthread.h>

#define TAILLE_MAX_SEND 1000

void * thread_send(void* arg){
    long client_fd = (long) arg;
    while (1)
    {
        char tab_send[TAILLE_MAX_SEND];memset(tab_send,0,TAILLE_MAX_SEND);
        fgets(tab_send,TAILLE_MAX_SEND,stdin);
        send(client_fd,tab_send,sizeof tab_send,0);
        
    }
    
}