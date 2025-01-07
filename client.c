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

#define TAILLE_TAB_RECV 1000
#define TAILLE_TAB_SEND 1000

int client_fd;


int main(int argc,char **argv){
    char pseudo[100];memset(pseudo,0,100);
    int port_client = 4000;

    int check_error = check_error_start_client(argc,argv,pseudo,&port_client);
    if(check_error==-1)return EXIT_FAILURE;
    // printf("port client sur le main %d\n",port_client);

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


    client_fd = socket(AF_INET,SOCK_STREAM,0);perror("socket");
    if(client_fd==-1)return EXIT_FAILURE;


    check_error=bind(client_fd,(struct sockaddr*)&client_addr,sizeof client_addr);perror("bind");
    if(check_error==-1)return EXIT_FAILURE;


    check_error=connect(client_fd,(struct sockaddr*)&serveur_addr,sizeof serveur_addr);//perror("connect");
    if(check_error==-1)return EXIT_FAILURE;

    char tab_recv[TAILLE_TAB_RECV];memset(tab_recv,0,TAILLE_TAB_RECV);

    check_error = recv(client_fd,tab_recv,TAILLE_TAB_RECV,0);
    if(check_error==-1)return EXIT_FAILURE;


    printf("%s",tab_recv);

    char tab_send[TAILLE_TAB_SEND];memset(tab_send,0,TAILLE_TAB_SEND);
    
}