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

#define TAILLE_TAB_RECV 1000
#define MSG_DECO "code_45421354"

void * thread_recv(void *arg){
    struct parametre_thread_snd_thread
    {
        long client_fd;
        char pseudo[100];
    }typedef t_parametre_snd_thread;
    // long client_fd = (long) arg;
    t_parametre_snd_thread *client;
    client=arg;
    
        // printf("oui");
        // fflush(stdout);
        // long client_fd=(long) arg;
    while(1){
        // printf("CLIENT FD = %ld",client_fd);
        char tab_recv[TAILLE_TAB_RECV];memset(tab_recv,0,TAILLE_TAB_RECV);
        int check_error = recv (client->client_fd,tab_recv,TAILLE_TAB_RECV,0);
        if (check_error==0 || check_error == -1) return (void*)EXIT_FAILURE;
        if (strstr(tab_recv,MSG_DECO)!=NULL)
        {
            // char*arg[30];
            // strtok(tab_recv,)
            // char msg_send[150];memset(msg_send,0,150);
            // printf("%s",tab_recv);
            send(client->client_fd,tab_recv,strlen(tab_recv),0);//perror("send deconnection ()");
        }
        else if (strcmp(tab_recv,"\nVous vous êtes deconnecté\n")==0)
        {
            // printf("owwwwwwwwwwwwwwwwwww\n");
            pthread_exit(NULL);
        }
        else
        {
            printf("%s",tab_recv);
        }
        fflush(stdout);
    
    }
    pthread_exit(NULL);
}