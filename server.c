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
#include "set_server.c"
#include "thread_accept.c"

int main(int argc, char** argv){

    // int port_server=atoi(argv[1]);

    long server_fd;
    server_fd = set_server(PORT_SERVER,server_fd);
    if(server_fd == -1)return EXIT_FAILURE;

    pthread_t thread;
    pthread_create(&thread,NULL,thread_accept,(void*)server_fd);

    while (1)
    {   
        char close_server[50];memset(close_server,0,50);
        fgets(close_server,50,stdin);
        close_server[strlen(close_server)-1]=0;
        if (strcmp(close_server,"close")==0)
        {
            break;
        }
        else{
            printf("Commande invalide. Entrez la commande \"close\" pour fermer le serveur.\n");
        }
    }

    close(server_fd);
    
}