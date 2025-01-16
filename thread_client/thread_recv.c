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

void * thread_recv(void *arg){
    while(1){
        // printf("oui");
        // fflush(stdout);
        long client_fd=(long) arg;
        char tab_recv[TAILLE_TAB_RECV];memset(tab_recv,0,TAILLE_TAB_RECV);
        int check_error = recv (client_fd,tab_recv,TAILLE_TAB_RECV,0);
        if (check_error==0 || check_error == -1) return (void*)EXIT_FAILURE;
        if (strcmp(tab_recv,"code_45421354")==0)
        {
            send(client_fd,"code_45421354",14,0);perror("send deconnection ()");
        }
        else
        {
            printf("%s\n",tab_recv);
        }
        
    
    }
    pthread_exit(NULL);
}