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
#include <errno.h>

#define NB_CLIENT_MAX 20

int nombre_de_client=0;
long tab_client[NB_CLIENT_MAX];


void * recv_thread(void* argument){
    int id_client=(int)argument;
    while (1)
    {
        // printf("id client = %d\n",id_client);
        char tab_recv[1000];memset(tab_recv,0,1000);
        int check_error = recv(id_client,tab_recv,1000,0);perror("recv_thread()");

        if (check_error == -1 || check_error == 0)
        {
            close(id_client);
            for (int i = 0; i < nombre_de_client; i++)
            {
                if (tab_client[i]==id_client)
                {
                    tab_client[i]=0;
                }
            }
            for (int i = 0; i < nombre_de_client; i++)
            {
                if (tab_client[i]==0)
                {
                    tab_client[i]=tab_client[i+1];
                    tab_client[i+1]=0;
                }   
            }

            nombre_de_client--;
            pthread_exit(NULL);           
        }
        else
        {
            int client_fd_send=4;
            for (int i = 0; i < nombre_de_client; i++)
            {
                if (id_client==tab_client[i])
                {
                    
                }
                else
                {
                    send(tab_client[i],tab_recv,sizeof tab_recv,0);
                }
                
            }   
        }
    }
}

void * thread_accept(void* arg){
    long server_fd=(long)arg;

    while (1)
    {
        char pseudo[50];memset(pseudo,0,50);
        printf("nombre de client au début:%d\n",nombre_de_client);
        struct sockaddr_in client_addr;
        socklen_t len;
        int client_fd = accept(server_fd,(struct sockaddr*)&client_addr,&len);perror("accept");
        if (client_fd==-1) pthread_exit(NULL);

        tab_client[nombre_de_client]=client_fd;
        
        FILE* message = fopen("fichier_text/msg_bienvenue.txt","r");
        fseek(message,0,SEEK_END);
        int taille_fichier = ftell(message);
        fseek(message,0,SEEK_SET);
        char msg_acces_au_serv[taille_fichier];memset(msg_acces_au_serv,0,taille_fichier);
        fread(msg_acces_au_serv,1,taille_fichier,message);
        send(client_fd,msg_acces_au_serv,taille_fichier,0);perror("send msg accès au serv");
        fseek(message,0,SEEK_SET);
        fclose(message);

        recv(client_fd,pseudo,sizeof pseudo,0);

        printf("client[%d] has joined the chat\n",client_fd);
        
        char*argument[2];
        argument[0]=tab_client[nombre_de_client];
        argument[1]=pseudo;
        pthread_t thread_receive;
        pthread_create(&thread_receive,NULL,recv_thread,(void*)argument); 
        nombre_de_client++;
        printf("nombre de client connecter : %d\n",nombre_de_client);
    }
    


}
