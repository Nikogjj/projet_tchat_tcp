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
    struct parametre_thread_snd_thread
    {
        long client_fd;
        char pseudo[100];
    }typedef t_parametre_snd_thread;
    // long client_fd = (long) arg;
    t_parametre_snd_thread *client;
    client=arg;

    while (1)
    {
        char tab_send[TAILLE_MAX_SEND];memset(tab_send,0,TAILLE_MAX_SEND);
        fflush(stdout);
        fgets(tab_send,TAILLE_MAX_SEND,stdin);
        // printf("FGETS : \"%s\"",tab_send);
        send(client->client_fd,tab_send,strlen(tab_send),0);
    }
    
}