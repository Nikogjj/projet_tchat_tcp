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
#include "recv_thread.c"
#define NB_CLIENT_MAX 20

long tab_client[NB_CLIENT_MAX];
int nombre_de_client=0;

void* thread_recv(void* arg){
    long ID_client = (long)arg;
    while (1)
    {
        char reponse_buf[255];memset(reponse_buf,0,255);
        int check_error=recv(ID_client,reponse_buf,sizeof reponse_buf,0);
        if (check_error==0||check_error==-1){
            printf("client[%ld] has left the chat\n",ID_client);
            close(ID_client);
            for (int i = 0; i < nombre_de_client; i++)
            {
                if (tab_client[i]==ID_client)
                {
                    tab_client[i]=0;
                }
                
            }
            for (int i = 0; i < NB_CLIENT_MAX; i++)
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
        else{
            printf("Nombre de clients: %d\n",nombre_de_client);
            for (int i = 0; i < nombre_de_client; i++)
            {
                if (tab_client[i]==ID_client)
                {
                    send(tab_client[i],"[OK]\n",6,0);perror("send a l'envoyeur du msg");
                }  
                else
                {
                    send(tab_client[i],reponse_buf,strlen(reponse_buf),0);perror("send");
                    printf("envoi effectué à client[%ld]\n",tab_client[i]);
                }  
            }
        }
    }
}

void * thread_accept(void* arg){
    long server_fd=(long)arg;

    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t len;
        int client_fd = accept(server_fd,(struct sockaddr*)&client_addr,&len);perror("accept");
        if (client_fd==-1) pthread_exit(NULL);
        
        FILE* message = fopen("oui.txt","r");
        fseek(message,0,SEEK_END);
        int taille_fichier = ftell(message);
        fseek(message,0,SEEK_SET);
        char msg_acces_au_serv[taille_fichier];memset(msg_acces_au_serv,0,taille_fichier);
        fread(msg_acces_au_serv,1,taille_fichier,message);
        send(client_fd,msg_acces_au_serv,taille_fichier,0);perror("send msg accès au serv");
        fseek(message,0,SEEK_SET);
        fclose(message);

        nombre_de_client++;
        printf("nombre de client connecter : %d\n",nombre_de_client);
        sleep(3);
        pthread_t thread_receive;
        pthread_create(&thread_receive,NULL,thread_recv,(void*)tab_client[nombre_de_client]); 

    }
    


}

