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
#include "../get_nombre_de_mot.c"
#include "../check_commande.c"

#define NB_CLIENT_MAX 20
#define NB_SALON_MAX 50
#define DANS_LE_MENU 0
#define DANS_UN_SALON 1
#define DANS_UN_SALON_PRIVE 2
#define CREATE 1
#define LIST 2
#define JOIN 3
#define EXIT 4
#define INVITE 5
#define NOT_INVITED 0
#define INVITED 1


int nombre_de_client=0;
int nombre_salon_en_ligne=0;

char list_salon[50][NB_SALON_MAX];
char list_salon_prive[50][NB_SALON_MAX];

typedef struct client_body
{
    long client_fd;
    char pseudo[500];
    char nom_salon[50];
    int check_menu_or_salon;// 0 = dans le menu et 1 = dans un salon
    // int check_if_client_is_invited;
    int nombre_dinvitation_prive;
    char list_invite_private[10][100];
    int ID_client;
}t_client_body;



typedef struct thread_recv_info
{
    int ID_client;
    t_client_body* clients_body;

}t_thread_recv_info;


void * recv_thread(void* parametre_thread ){
    printf("entrer dans le thread recv\n");
    t_thread_recv_info* parametre_thread_recv;
    
    parametre_thread_recv = parametre_thread;

    long client_fd = parametre_thread_recv->clients_body[parametre_thread_recv->ID_client].client_fd;
    printf("client_fd dans le recv : %ld\n",client_fd);
    
    recv(client_fd,parametre_thread_recv->clients_body[parametre_thread_recv->ID_client].pseudo,100000,0);perror("recv du pseudo()");

    printf("%s a rejoint le serveur\n",parametre_thread_recv->clients_body[parametre_thread_recv->ID_client].pseudo);

    int real_ID_client = parametre_thread_recv->clients_body[parametre_thread_recv->ID_client].ID_client;
    while (1)
    {
        // int real_ID_client = parametre_thread_recv->clients_body[real_ID_client].ID_client;
        char tab_recv[1000];memset(tab_recv,0,1000);
        int check_error = recv(client_fd,tab_recv,1000,0);perror("recv_thread()");
        if (check_error == -1 || check_error == 0)
        {
            // send(client_fd,"code_45421354",14,0);perror("send deconnection ()");
            for (int i = 0; i < nombre_de_client; i++)
                {
                    if (client_fd==parametre_thread_recv->clients_body[i].client_fd)
                    {
                    
                    }
                    else
                    {
                        send(parametre_thread_recv->clients_body[i].client_fd,"code_45421354",14,0);perror("send deconnection()");
                    }
                }
            for (int i = 0; i < nombre_de_client; i++)
            {
                if (parametre_thread_recv->clients_body[i].client_fd==client_fd)
                {
                    parametre_thread_recv->clients_body[i].client_fd=0;
                }
            }
            for (int i = 0; i < nombre_de_client; i++)
            {
                if (parametre_thread_recv->clients_body[i].client_fd ==0)
                {
                    parametre_thread_recv->clients_body[i].ID_client=parametre_thread_recv->clients_body[i+1].ID_client--;

                    // parametre_thread_recv->clients_body[i].check_if_client_is_invited=parametre_thread_recv->clients_body[i+1].check_if_client_is_invited;
                    parametre_thread_recv->clients_body[i].check_menu_or_salon=parametre_thread_recv->clients_body[i+1].check_menu_or_salon;
                    parametre_thread_recv->clients_body[i].client_fd=parametre_thread_recv->clients_body[i+1].client_fd;
                    strcpy(parametre_thread_recv->clients_body[i].nom_salon,parametre_thread_recv->clients_body[i+1].nom_salon);
                    strcpy(parametre_thread_recv->clients_body[i].pseudo,parametre_thread_recv->clients_body[i+1].pseudo);
                    parametre_thread_recv->clients_body[i+1].client_fd=0;
                }   
            }
            close(client_fd);
            nombre_de_client--;
            pthread_exit(NULL);           
        }

        if (strcmp(tab_recv,"code_45421354")==0)
        {
            real_ID_client--;
        }
        
        if (parametre_thread_recv->clients_body[real_ID_client].check_menu_or_salon==DANS_UN_SALON)
        {
            printf("client %s dans le salon\n",parametre_thread_recv->clients_body[real_ID_client].pseudo);
        }
        else if (parametre_thread_recv->clients_body[real_ID_client].check_menu_or_salon==DANS_LE_MENU)
        {
            printf("client %s dans le menu\n",parametre_thread_recv->clients_body[real_ID_client].pseudo);
        }
        
        
        if (parametre_thread_recv->clients_body[real_ID_client].check_menu_or_salon==DANS_LE_MENU)
        {
            printf("dans le if\n");
            int nombre_de_mot = get_nombre_de_mot(tab_recv);
            char* arg_commande[nombre_de_mot];
            switch (check_commande(tab_recv,arg_commande,nombre_de_mot))
            {
            case CREATE:
                strcpy(list_salon[nombre_salon_en_ligne],arg_commande[2]);
                nombre_salon_en_ligne++;
                char msg_reaction_serveur[255];memset(msg_reaction_serveur,0,255);
                sprintf(msg_reaction_serveur,"Salon %s créé ! Vous pouvez dès à présent le rejoindre.\n",arg_commande[2]);
                send(client_fd,msg_reaction_serveur,sizeof msg_reaction_serveur,0);
                break;

            case LIST:
                if (strcmp(arg_commande[1],"users")==0)
                {
                    for (int i = 0; i < nombre_de_client; i++)
                    {
                        printf("%s\n",parametre_thread_recv->clients_body[i].pseudo);
                        send(client_fd,parametre_thread_recv->clients_body[i].pseudo,1000,0);perror("send list_users()");// probleme avec la taille du recv(client) exemple : si la taille send du serveur =100 et que le recv client=1000 si ya plusieurs send je ne recois pas tout 
                    }
                    
                }
                if (strcmp(arg_commande[1],"salons")==0)
                {
                    for (int i = 0; i < nombre_salon_en_ligne; i++)
                    {
                        send(client_fd,list_salon[i],1000,0);perror("send list_users()");// probleme avec la taille du recv(client) exemple : si la taille send du serveur =100 et que le recv client=1000 si ya plusieurs send je ne recois pas tout 
                    }
                }

                break;

            case JOIN:
                if (strcmp(arg_commande[1],"user")==0)
                {
                    if (parametre_thread_recv->clients_body[real_ID_client].nombre_dinvitation_prive==0)
                    {
                        send(client_fd,"Vous n'avez aucune invitation pour un salon privé actuellement\n",66,0);
                    }
                    else if (parametre_thread_recv->clients_body[real_ID_client].nombre_dinvitation_prive!=0)
                    {
                        int check_if_client_exist_in_list=0;
                        char pseudo_client_to_join[50];memset(pseudo_client_to_join,0,50);
                        int index_client_to_join;
                        for (int i = 0; i < parametre_thread_recv->clients_body[real_ID_client].nombre_dinvitation_prive; i++)
                        {
                            if (strcmp(parametre_thread_recv->clients_body[real_ID_client].list_invite_private[i],arg_commande[2])==0)
                            {
                                check_if_client_exist_in_list++;
                                // strcpy(pseudo_client_to_join,parametre_thread_recv->clients_body[i].pseudo);
                                // index_client_to_join=i;
                            }
                        }
                        if (check_if_client_exist_in_list==1)
                        {
                            strcpy(parametre_thread_recv->clients_body[real_ID_client].nom_salon,arg_commande[2]);
                            parametre_thread_recv->clients_body[real_ID_client].check_menu_or_salon=DANS_UN_SALON;
                            for (int i = 0; i < parametre_thread_recv->clients_body[real_ID_client].nombre_dinvitation_prive; i++)
                            {
                                if (strcmp(arg_commande[2],parametre_thread_recv->clients_body[real_ID_client].list_invite_private[i])==0)
                                {
                                    strcpy(parametre_thread_recv->clients_body[real_ID_client].list_invite_private[i],parametre_thread_recv->clients_body[real_ID_client].list_invite_private[i+1]);   
                                }
                                if (i==parametre_thread_recv->clients_body[real_ID_client].nombre_dinvitation_prive-1)
                                {
                                    memset(parametre_thread_recv->clients_body[real_ID_client].list_invite_private[i],0,500);
                                }
                                if (strcmp(parametre_thread_recv->clients_body[real_ID_client].list_invite_private[i],parametre_thread_recv->clients_body[real_ID_client].list_invite_private[i-1])==0)
                                {
                                    strcpy(parametre_thread_recv->clients_body[real_ID_client].list_invite_private[i],parametre_thread_recv->clients_body[real_ID_client].list_invite_private[i+1]);   
                                }
                            }
                            
                            parametre_thread_recv->clients_body[real_ID_client].nombre_dinvitation_prive--;

                        }
                        else
                        {
                            send(client_fd,"Le pseudo de cette utilisateur n'existe pas ou n'est plus en ligne\n",68,0);
                        }
                    } 
                }
                if(strcmp(arg_commande[1],"salon")==0)
                {
                    int check_salon_exist=0;
                    if (parametre_thread_recv->clients_body[real_ID_client].check_menu_or_salon==DANS_UN_SALON)
                    {
                        send(client_fd,"\nVous êtes déjà dans un salon.\nVeuillez quitter ce salon pour en rejoindre un autre\n\n",89,0);
                    }
                    else{
                        for (int i = 0; i < nombre_salon_en_ligne; i++)
                        {
                            if (strcmp(arg_commande[2],list_salon[i])==0)
                            {
                                check_salon_exist++;
                                strcpy(parametre_thread_recv->clients_body[real_ID_client].nom_salon,arg_commande[2]);
                                parametre_thread_recv->clients_body[real_ID_client].check_menu_or_salon=DANS_UN_SALON;
                            }

                        }
                        if (check_salon_exist==0)
                        {
                            send(client_fd,"Le salon n'existe pas\n",23,0);
                        }
                    }
                    
                }
                else if (strcmp(arg_commande[1],"world")==0)
                {
                    strcpy(parametre_thread_recv->clients_body[real_ID_client].nom_salon,"world");
                    parametre_thread_recv->clients_body[real_ID_client].check_menu_or_salon=DANS_UN_SALON;
                }
                break;
            case INVITE:
                if (strcmp(arg_commande[1],"user")==0)
                {
                    int check_client_exist=0;
                    // char pseudo_client_to_join[50];memset(pseudo_client_to_join,0,50);
                    int index_client_to_join;
                    for (int i = 0; i < nombre_de_client; i++)
                    {
                        if (strcmp(parametre_thread_recv->clients_body[i].pseudo,arg_commande[2])==0)
                        {
                            check_client_exist++;   
                            // strcpy(pseudo_client_to_join,parametre_thread_recv->clients_body[i].pseudo);
                            index_client_to_join=i;
                        }
                    }
                    if (check_client_exist==1)
                    {
                        if (parametre_thread_recv->clients_body[index_client_to_join].check_menu_or_salon==DANS_LE_MENU)
                        {
                            int index_invitation_prive = parametre_thread_recv->clients_body[index_client_to_join].nombre_dinvitation_prive;
                            parametre_thread_recv->clients_body[index_client_to_join].nombre_dinvitation_prive++;
                            // parametre_thread_recv->clients_body[index_client_to_join].check_if_client_is_invited=INVITED;
                            strcpy(parametre_thread_recv->clients_body[index_client_to_join].list_invite_private[index_invitation_prive],parametre_thread_recv->clients_body[real_ID_client].pseudo);
                            printf("creation d'invitation : \"%s\" à %s\n",parametre_thread_recv->clients_body[index_client_to_join].list_invite_private[index_invitation_prive],parametre_thread_recv->clients_body[index_client_to_join].pseudo);

                            parametre_thread_recv->clients_body[real_ID_client].check_menu_or_salon=DANS_UN_SALON;
                            strcpy(parametre_thread_recv->clients_body[real_ID_client].nom_salon,parametre_thread_recv->clients_body[real_ID_client].pseudo);
                            
                            char send_invite_private[2000];memset(send_invite_private,0,2000);
                            sprintf(send_invite_private,"%s vous invite à rejoindre son salon privé tapez la commande : join user %s\n",parametre_thread_recv->clients_body[real_ID_client].pseudo,parametre_thread_recv->clients_body[real_ID_client].pseudo);
                            send(parametre_thread_recv->clients_body[index_client_to_join].client_fd,send_invite_private,strlen(send_invite_private),0);

                            char send_notification_private[2000];memset(send_invite_private,0,2000);
                            sprintf(send_invite_private,"Vous êtes maintenant dans votre salon privé en attente de %s. Tapez exit pour sortir de votre salon privé et en rejoindre un autre",parametre_thread_recv->clients_body[index_client_to_join].pseudo);
                            send(client_fd,send_invite_private,strlen(send_invite_private),0);
                        }
                        else if (parametre_thread_recv->clients_body[index_client_to_join].check_menu_or_salon==DANS_UN_SALON)
                        {
                            int index_invitation_prive = parametre_thread_recv->clients_body[index_client_to_join].nombre_dinvitation_prive;
                            parametre_thread_recv->clients_body[index_client_to_join].nombre_dinvitation_prive++;
                            // parametre_thread_recv->clients_body[index_client_to_join].check_if_client_is_invited=INVITED;
                            strcpy(parametre_thread_recv->clients_body[index_client_to_join].list_invite_private[index_invitation_prive],parametre_thread_recv->clients_body[real_ID_client].pseudo);
                            printf("creation d'invitation : \"%s\" à %s\n",parametre_thread_recv->clients_body[index_client_to_join].list_invite_private[index_invitation_prive],parametre_thread_recv->clients_body[index_client_to_join].pseudo);
                            parametre_thread_recv->clients_body[real_ID_client].check_menu_or_salon=DANS_UN_SALON;
                            strcpy(parametre_thread_recv->clients_body[real_ID_client].nom_salon,parametre_thread_recv->clients_body[real_ID_client].pseudo);
                        
                            char send_invite_private[2000];memset(send_invite_private,0,2000);
                            sprintf(send_invite_private,"%s vous invite à rejoindre son salon privé mais vous êtes déjà dans un salon. Si vous voulez le rejoindre quittez votre salon en tapant : \"exit\" puis \"join user %s\"\n",parametre_thread_recv->clients_body[real_ID_client].pseudo,parametre_thread_recv->clients_body[real_ID_client].pseudo);
                            send(parametre_thread_recv->clients_body[index_client_to_join].client_fd,send_invite_private,strlen(send_invite_private),0);

                            char send_notification_private[2000];memset(send_invite_private,0,2000);
                            sprintf(send_invite_private,"Vous êtes maintenant dans votre salon privé en attente de %s. Tapez exit pour sortir de votre salon privé et en rejoindre un autre",parametre_thread_recv->clients_body[index_client_to_join].pseudo);
                            send(client_fd,send_invite_private,strlen(send_invite_private),0);
                        }
                        
                        
                        
                    }
                    else
                    {
                        send(client_fd,"Le pseudo de cette utilisateur n'existe pas ou n'est plus en ligne\n",68,0);
                    }
                    
                    
                }
                else
                {
                    send(client_fd,"Mauvaise commande. Entrez la commande suivante : invite user *user_name*\n",74,0);
                }

                break;
            case EXIT:
                send(client_fd,"\nVous vous êtes deconnecté\n",30,0);
                close(client_fd);
                for (int i = 0; i < nombre_de_client; i++)
                {
                    if (client_fd==parametre_thread_recv->clients_body[i].client_fd)
                    {
                    
                    }
                    else
                    {
                        send(parametre_thread_recv->clients_body[i].client_fd,"code_45421354",14,0);perror("send deconnection()");
                    }
                }

                for (int i = 0; i < nombre_de_client; i++)
                {
                    if (parametre_thread_recv->clients_body[i].client_fd==client_fd)
                    {
                        parametre_thread_recv->clients_body[i].client_fd=0;
                    }
                }
                for (int i = 0; i < nombre_de_client; i++)
                {
                    if (parametre_thread_recv->clients_body[i].client_fd ==0)
                    {
                        parametre_thread_recv->clients_body[i].ID_client=parametre_thread_recv->clients_body[i+1].ID_client--;

                        // parametre_thread_recv->clients_body[i].check_if_client_is_invited=parametre_thread_recv->clients_body[i+1].check_if_client_is_invited;
                        parametre_thread_recv->clients_body[i].check_menu_or_salon=parametre_thread_recv->clients_body[i+1].check_menu_or_salon;
                        parametre_thread_recv->clients_body[i].client_fd=parametre_thread_recv->clients_body[i+1].client_fd;
                        strcpy(parametre_thread_recv->clients_body[i].nom_salon,parametre_thread_recv->clients_body[i+1].nom_salon);
                        strcpy(parametre_thread_recv->clients_body[i].pseudo,parametre_thread_recv->clients_body[i+1].pseudo);
                        parametre_thread_recv->clients_body[i+1].client_fd=0;
                    }   
                }
                close(client_fd);
                nombre_de_client--;
                pthread_exit(NULL);           
                    
                break;
            default:
                break;
            }
        }
        else if(parametre_thread_recv->clients_body[real_ID_client].check_menu_or_salon==DANS_UN_SALON || parametre_thread_recv->clients_body[real_ID_client].check_menu_or_salon==DANS_UN_SALON_PRIVE)
        {
            if (strcmp(tab_recv,"exit\n")==0)
            {
                parametre_thread_recv->clients_body[real_ID_client].check_menu_or_salon=DANS_LE_MENU;
                memset(parametre_thread_recv->clients_body[real_ID_client].nom_salon,0,50);
            }
            else{

                tab_recv[strlen(tab_recv)-1]=0;
                for (int i = 0; i < nombre_de_client; i++)
                {
                    if (strcmp(parametre_thread_recv->clients_body[real_ID_client].nom_salon,parametre_thread_recv->clients_body[i].nom_salon)==0)
                    {
                        if (client_fd==parametre_thread_recv->clients_body[i].client_fd)
                        {
                        
                        }
                        else
                        {
                            send(parametre_thread_recv->clients_body[i].client_fd,tab_recv,strlen(tab_recv),0);perror("send msg()");
                        }
                    }   
                }
            }
            
        }
            
        // else if(parametre_thread_recv->clients_body[real_ID_client].check_menu_or_salon==DANS_UN_SALON_PRIVE)
        // {
            
        // }
    }
}

void * thread_accept(void* arg){
    long server_fd=(long)arg;


    t_client_body* tab_client_info=malloc((sizeof(t_client_body))*NB_CLIENT_MAX);
    
    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t len;
        int copie_nombre_de_client=nombre_de_client;
        long client_fd = accept(server_fd,(struct sockaddr*)&client_addr,&len);perror("accept");
        if (client_fd==-1) pthread_exit(NULL);

        t_thread_recv_info* parametre_thread = malloc(sizeof(t_thread_recv_info));
        t_client_body client_info;

        client_info.client_fd=client_fd;
        client_info.check_menu_or_salon=DANS_LE_MENU;
        // client_info.check_if_client_is_invited=NOT_INVITED;
        client_info.ID_client=nombre_de_client;
        client_info.nombre_dinvitation_prive=0;

        tab_client_info[nombre_de_client]=client_info;

        parametre_thread->ID_client=nombre_de_client;
        parametre_thread->clients_body=tab_client_info;



        printf("Nombre de client : %d\n",nombre_de_client);
        
        FILE* message = fopen("fichier_text/msg_bienvenue.txt","r");
        fseek(message,0,SEEK_END);
        int taille_fichier = ftell(message);
        fseek(message,0,SEEK_SET);
        char msg_acces_au_serv[taille_fichier];memset(msg_acces_au_serv,0,taille_fichier);
        fread(msg_acces_au_serv,1,taille_fichier,message);
        send(client_fd,msg_acces_au_serv,taille_fichier,0);perror("send msg accès au serv");
        fseek(message,0,SEEK_SET);
        fclose(message);
        
        pthread_t thread_receive;
        
        
        pthread_create(&thread_receive,NULL,recv_thread,(void*)parametre_thread); 
        
        // pthread_create(&thread_receive,NULL,recv_thread,(void*)); 
        nombre_de_client++;
        printf("nombre de client connecter : %d\n",nombre_de_client);
    }
    
    free(tab_client_info);
}
