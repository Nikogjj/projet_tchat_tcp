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
#include "port.h"
#include "parametre/check_error_start_client.c"
#include "thread_client/thread_recv.c"
#include "thread_client/thread_send.c"

#define TAILLE_TAB_RECV 1000
#define TAILLE_TAB_SEND 1000
#define TAILLE_MAX_COMMANDE 50
#define DANS_LE_MENU 0;
// long client_fd;



int main(int argc,char **argv){
    struct parametre_thread_snd
    {
        long client_fd;
        char pseudo[100];
    }typedef t_parametre_snd;

    int port_client = 4000;


    t_parametre_snd client;
    t_parametre_snd* client_p=malloc(sizeof client);
    strcpy(client_p->pseudo,argv[2]);

    int check_error = check_error_start_client(argc,argv,client_p->pseudo,&port_client);
    if(check_error==-1)return EXIT_FAILURE;


    struct sockaddr_in client_addr={
        .sin_addr.s_addr=INADDR_ANY,
        .sin_family=AF_INET,
        .sin_port=htons(port_client)
    };

    struct sockaddr_in serveur_addr={
        .sin_addr.s_addr=INADDR_ANY,
        .sin_family=AF_INET,
        .sin_port=htons(PORT_SERVER)
    };

    socklen_t len;

    client_p->client_fd= socket(AF_INET,SOCK_STREAM,0);//perror("socket");
    if(client_p->client_fd==-1)return EXIT_FAILURE;

    check_error=bind(client_p->client_fd,(struct sockaddr*)&client_addr,sizeof client_addr);//perror("bind");
    if(check_error==-1)return EXIT_FAILURE;


    check_error=connect(client_p->client_fd,(struct sockaddr*)&serveur_addr,sizeof serveur_addr);//perror("connect");
    if(check_error==-1)return EXIT_FAILURE;

    char tab_recv[TAILLE_TAB_RECV];memset(tab_recv,0,TAILLE_TAB_RECV);

    send(client_p->client_fd,client_p->pseudo,sizeof client_p->pseudo,0);//perror("send_pseudo()");

    check_error = recv(client_p->client_fd,tab_recv,TAILLE_TAB_RECV,0);
    if(check_error==-1)return EXIT_FAILURE;

    printf("%s",tab_recv);

    pthread_t thread_snd;
    pthread_t thread_rcv;

    pthread_create(&thread_rcv,NULL,thread_recv,(void*) client_p);
    pthread_create(&thread_snd,NULL,thread_send,(void*) client_p);

    int wait_end_of_thread_recv = pthread_join(thread_rcv,NULL);
    if (wait_end_of_thread_recv==0)
    {
        pthread_cancel(thread_snd);
    }
    return 0;
}