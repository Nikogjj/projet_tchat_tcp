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
#include "../check_command_deconnexion.c"

#define NB_CLIENT_MAX 20
#define NB_SALON_MAX 50
#define DANS_LE_MENU 0
#define DANS_UN_SALON 1
#define DANS_UN_SALON_PRIVE 2
#define DECONNECTION -2
#define ERROR -1
#define CREATE 1
#define LIST 2
#define JOIN 3
#define EXIT 4
#define INVITE 5
#define COMMAND 6
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
                        if (parametre_thread_recv->clients_body[real_ID_client].check_menu_or_salon==DANS_UN_SALON_PRIVE)
                        {
                            if (strcmp(parametre_thread_recv->clients_body[real_ID_client].nom_salon,parametre_thread_recv->clients_body[i].nom_salon)==0)
                            {
                                char msg_send[2000];memset(msg_send,0,2000);
                                sprintf(msg_send,"<SERVER> : %s a quitté le salon %s.\n",parametre_thread_recv->clients_body[real_ID_client].pseudo,parametre_thread_recv->clients_body[real_ID_client].nom_salon);
                                send(parametre_thread_recv->clients_body[i].client_fd,msg_send,strlen(msg_send),0);
                            }
                            
                        }
                        char msg_send[2000];memset(msg_send,0,2000);// FAIRE LE INTEGER TO ASCII
                        sprintf(msg_send,"code_45421354 %d ",real_ID_client);
                        printf("MSG SEND : %s",msg_send);
                        send(parametre_thread_recv->clients_body[i].client_fd,msg_send,strlen(msg_send),0);perror("send deconnection()");
                        
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

        if (strstr(tab_recv,"code_45421354")!=NULL)
        {
            printf("le if a marché\n");
            
            int ID_client_disconnected = check_commande_deconnexion(tab_recv);
            printf("ID client disconnected dans le main %d\n",ID_client_disconnected);
            if (real_ID_client>ID_client_disconnected)
            {
                real_ID_client--;
            }
            printf("REAL ID CLIENT de %s = %d\n",parametre_thread_recv->clients_body[real_ID_client].pseudo,real_ID_client);
            continue;
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
            if (nombre_de_mot==ERROR)
            {
                send(client_fd,"<SERVER> : Veuillez entrer une commande valide.\n",49,0);
                continue;
            }
            
            char* arg_commande[nombre_de_mot];
            switch (check_commande(tab_recv,arg_commande,nombre_de_mot))
            {
            case CREATE:
                if (nombre_de_mot!=3)
                {
                    send(client_fd,"<SERVER> : Mauvaise commande. Entrez la commande suivante : \"create salon *salon_id*\"\n",87,0);
                    continue;
                }
                if (strcmp(arg_commande[1],"salon")==0)
                {
                    strcpy(list_salon[nombre_salon_en_ligne],arg_commande[2]);
                    nombre_salon_en_ligne++;
                    char msg_reaction_serveur[255];memset(msg_reaction_serveur,0,255);
                    sprintf(msg_reaction_serveur,"\n<SERVER> : Salon %s créé ! Vous pouvez dès à présent le rejoindre.\n\n",arg_commande[2]);
                    send(client_fd,msg_reaction_serveur,strlen(msg_reaction_serveur),0);
                    break;
                }
                else{
                    send(client_fd,"<SERVER> : Mauvaise commande. Entrez la commande suivante : \"create salon *salon_id*\"\n",87,0);
                    continue;
                }
            case LIST:
                if (nombre_de_mot==1)
                {
                    send(client_fd,"<SERVER> : Mauvaise commande. Entrez la commande suivante : \"list users\" ou \"list salons\"\n",91,0);
                    continue;
                }
                if (nombre_de_mot>2)
                {
                    send(client_fd,"<SERVER> : Mauvaise commande. Entrez la commande suivante : \"list users\" ou \"list salons\"\n",91,0);
                    continue;
                }
                
                if (strcmp(arg_commande[1],"users")==0)
                {
                    char text_list_users[510] = "\nListe des utilisateurs connectés :\n";
                    FILE* list_des_pseudo = fopen("fichier_text/list_users.txt","w+");
                    fwrite(text_list_users,strlen(text_list_users),1,list_des_pseudo);
                    fclose(list_des_pseudo);
                    char pseudo_list[550];memset(pseudo_list,0,550);
                    FILE * list_des_pseudos = fopen("fichier_text/list_users.txt","a+");
                    for (int i = 0; i < nombre_de_client; i++)
                    {

                        sprintf(text_list_users,"-%s\n",parametre_thread_recv->clients_body[i].pseudo);
                        fwrite(text_list_users,strlen(text_list_users),1,list_des_pseudo);
                        // strcat(text_list_users,"\n");
                        printf("%s\n",parametre_thread_recv->clients_body[i].pseudo);
                    // fflush(stdout);
                    }
                    fseek(list_des_pseudos,0,SEEK_SET);
                    fread(pseudo_list,sizeof pseudo_list,1,list_des_pseudos);
                    // strcpy(text_list_users,"<");
                    strcat(pseudo_list,"\n");
                    // strcat(pseudo_list,parametre_thread_recv->clients_body[real_ID_client].pseudo);
                    // strcpy(text_list_users,"> : ");
                    // strcat(pseudo_list,text_list_users);
                    // pseudo_list[strlen(pseudo_list)-1]=0;
                    send(client_fd,pseudo_list,1000,0);perror("send list_users()");// probleme avec la taille du recv(client) exemple : si la taille send du serveur =100 et que le recv client=1000 si ya plusieurs send je ne recois pas tout 
                    printf("PSEUDO LIST : %s",pseudo_list);
                    // printf("PSEUDO LIST");
                    
                }
                else if (strcmp(arg_commande[1],"salons")==0)
                {
                    char text_list_salons[510] = "\nListe des salons existants :\n-world\n";
                    FILE* list_des_salon = fopen("fichier_text/list_salons.txt","w+");
                    fwrite(text_list_salons,strlen(text_list_salons),1,list_des_salon);
                    fclose(list_des_salon);
                    char salons_list[550];memset(salons_list,0,550);
                    list_des_salon = fopen("fichier_text/list_salons.txt","a+");
                    for (int i = 0; i < nombre_salon_en_ligne; i++)
                    {

                        sprintf(text_list_salons,"-%s\n",list_salon[i]);
                        fwrite(text_list_salons,strlen(text_list_salons),1,list_des_salon);
                        // strcat(text_list_users,"\n");
                        printf("%s\n",list_salon[i]);
                        // fflush(stdout);
                    }
                    fseek(list_des_salon,0,SEEK_SET);
                    fread(salons_list,sizeof salons_list,1,list_des_salon);
                    // strcpy(text_list_users,"<");
                    strcat(salons_list,"\n");
                    // strcat(pseudo_list,parametre_thread_recv->clients_body[real_ID_client].pseudo);
                    // strcpy(text_list_users,"> : ");
                    // strcat(pseudo_list,text_list_users);
                    // pseudo_list[strlen(pseudo_list)-1]=0;
                    send(client_fd,salons_list,1000,0);perror("send list_users()");// probleme avec la taille du recv(client) exemple : si la taille send du serveur =100 et que le recv client=1000 si ya plusieurs send je ne recois pas tout 
                    printf("SALON LIST : %s",salons_list);
                    // printf("PSEUDO LIST");
                }
                else{
                    send(client_fd,"<SERVER> : Mauvaise commande. Entrez la commande suivante : \"list users\" ou \"list salons\"\n",91,0);
                }

                break;

            case JOIN:
                if (nombre_de_mot>3)
                {
                    send(client_fd,"<SERVER> : Mauvaise commande. Entrez la commande suivante : \"join user *user_id\", \"join salon *salon_id*\" ou \"join world\" \"\n",125,0);
                }
                
                if (strcmp(arg_commande[1],"user")==0)
                {
                    if (parametre_thread_recv->clients_body[real_ID_client].nombre_dinvitation_prive==0)
                    {
                        send(client_fd,"<SERVER> : Vous n'avez aucune invitation pour un salon privé actuellement\n",76,0);
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
                            send(client_fd,"<SERVER> : Le pseudo de cette utilisateur n'existe pas ou n'est plus en ligne\n",79,0);
                        }
                    } 
                }
                else if(strcmp(arg_commande[1],"salon")==0)
                {
                    int check_salon_exist=0;
                    if (parametre_thread_recv->clients_body[real_ID_client].check_menu_or_salon==DANS_UN_SALON)
                    {
                        send(client_fd,"\n\n<SERVER> : Vous êtes déjà dans un salon.\nVeuillez quitter ce salon pour en rejoindre un autre\n\n",101,0);
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
                            send(client_fd,"<SERVER> : Le salon n'existe pas\n",34,0);
                        }
                        else if (check_salon_exist==1)
                        {
                            char msg_send[2000];memset(msg_send,0,2000);
                            sprintf(msg_send,"\n<SERVER> : Vous avez rejoins le salon %s. Tapez \"exit\" pour quitter ce salon et revenir au menu\n\n",arg_commande[2]);
                            send(client_fd,msg_send,strlen(msg_send),0);
                            memset(msg_send,0,2000);
                            sprintf(msg_send,"<SERVER> : %s a rejoin le salon %s.\n",parametre_thread_recv->clients_body[real_ID_client].pseudo,arg_commande[2]);
                            for (int i = 0; i < nombre_de_client; i++)
                            {
                                if (strcmp(arg_commande[2],parametre_thread_recv->clients_body[i].nom_salon)==0 && strcmp(parametre_thread_recv->clients_body[i].pseudo,parametre_thread_recv->clients_body[real_ID_client].pseudo)!=0)
                                {
                                    send(parametre_thread_recv->clients_body[i].client_fd,msg_send,strlen(msg_send),0);
                                }
                            }
                        }
                        
                    }   
                }
                else if (strcmp(arg_commande[1],"world")==0)
                {
                    strcpy(parametre_thread_recv->clients_body[real_ID_client].nom_salon,"world");
                    parametre_thread_recv->clients_body[real_ID_client].check_menu_or_salon=DANS_UN_SALON;
                    char msg_send[2000];memset(msg_send,0,2000);
                    sprintf(msg_send,"\n<SERVER> : Vous avez rejoins le salon %s. Tapez \"exit\" pour quitter ce salon et revenir au menu\n\n",arg_commande[1]);
                    send(client_fd,msg_send,strlen(msg_send),0);
                    memset(msg_send,0,2000);
                    sprintf(msg_send,"<SERVER> : %s a rejoin le salon %s.\n",parametre_thread_recv->clients_body[real_ID_client].pseudo,arg_commande[1]);
                    for (int i = 0; i < nombre_de_client; i++)
                    {
                        if (strcmp(arg_commande[1],parametre_thread_recv->clients_body[i].nom_salon)==0 && strcmp(parametre_thread_recv->clients_body[i].pseudo,parametre_thread_recv->clients_body[real_ID_client].pseudo)!=0)
                        {
                            send(parametre_thread_recv->clients_body[i].client_fd,msg_send,strlen(msg_send),0);
                        }
                    }
                }
                else{
                    send(client_fd,"<SERVER> : Mauvaise commande. Entrez la commande suivante : \"join user *user_id\", \"join salon *salon_id*\" ou \"join world\" \"\n",125,0);
                }
                break;
            case INVITE:
                if (nombre_de_mot>3)
                {
                    send(client_fd,"<SERVER> : Mauvaise commande. Entrez la commande suivante : \"invite user *user_id*\"\n",85,0);
                    continue;
                }
                if (nombre_de_mot==1)
                {
                    send(client_fd,"<SERVER> : Mauvaise commande. Entrez la commande suivante : \"invite user *user_id*\"\n",85,0);
                    continue;
                }
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

                            parametre_thread_recv->clients_body[real_ID_client].check_menu_or_salon=DANS_UN_SALON_PRIVE;
                            strcpy(parametre_thread_recv->clients_body[real_ID_client].nom_salon,parametre_thread_recv->clients_body[real_ID_client].pseudo);
                            
                            char send_invite_private[2000];memset(send_invite_private,0,2000);
                            sprintf(send_invite_private,"<SERVER> : %s vous invite à rejoindre son salon privé tapez la commande : join user %s\n",parametre_thread_recv->clients_body[real_ID_client].pseudo,parametre_thread_recv->clients_body[real_ID_client].pseudo);
                            send(parametre_thread_recv->clients_body[index_client_to_join].client_fd,send_invite_private,strlen(send_invite_private),0);

                            char send_notification_private[2000];memset(send_invite_private,0,2000);
                            sprintf(send_invite_private,"<SERVER> : Vous êtes maintenant dans votre salon privé en attente de %s. Tapez exit pour sortir de votre salon privé et en rejoindre un autre\n",parametre_thread_recv->clients_body[index_client_to_join].pseudo);
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
                            sprintf(send_invite_private,"<SERVER> : %s vous invite à rejoindre son salon privé mais vous êtes déjà dans un salon. Si vous voulez le rejoindre quittez votre salon en tapant : \"exit\" puis \"join user %s\"\n",parametre_thread_recv->clients_body[real_ID_client].pseudo,parametre_thread_recv->clients_body[real_ID_client].pseudo);
                            send(parametre_thread_recv->clients_body[index_client_to_join].client_fd,send_invite_private,strlen(send_invite_private),0);

                            char send_notification_private[2000];memset(send_invite_private,0,2000);
                            sprintf(send_invite_private,"<SERVER> : Vous êtes maintenant dans votre salon privé en attente de %s. Tapez exit pour sortir de votre salon privé et en rejoindre un autre\n",parametre_thread_recv->clients_body[index_client_to_join].pseudo);
                            send(client_fd,send_invite_private,strlen(send_invite_private),0);
                        }   
                    }
                    else
                    {
                        send(client_fd,"<SERVER> : Le pseudo de cette utilisateur n'existe pas ou n'est plus en ligne\n",79,0);
                    }
                }
                else
                {
                    send(client_fd,"<SERVER> : Mauvaise commande. Entrez la commande suivante : invite user *user_name*\n",85,0);
                }
                break;
            case EXIT:
                send(client_fd,"\n<SERVER> : Vous vous êtes deconnecté\n",41,0);
                close(client_fd);
                for (int i = 0; i < nombre_de_client; i++)
                {
                    if (client_fd==parametre_thread_recv->clients_body[i].client_fd)
                    {
                    
                    }
                    else
                    {
                        char msg_send[2000];memset(msg_send,0,2000);// FAIRE LE INTEGER TO ASCII
                        sprintf(msg_send,"code_45421354 %d ",real_ID_client);
                        send(parametre_thread_recv->clients_body[i].client_fd,msg_send,strlen(msg_send),0);perror("send deconnection()");
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
            case COMMAND:
            if (nombre_de_mot>1)
            {
                send(client_fd,"<SERVER> : Mauvaise commande. Entrez la commande suivante : \"command\"\n",71,0);
            }
            else{

                FILE * list_command=fopen("fichier_text/list_commands.txt","r+");perror("fopen");
                char commands[2000];memset(commands,0,2000);
                fseek(list_command,0,SEEK_END);
                int taille_commands = ftell(list_command);
                printf("TAILLE DU FTELL = %d\n",taille_commands);
                fseek(list_command,0,SEEK_SET);
                fread(commands,1,taille_commands,list_command);perror("fread");
                send(client_fd,commands,strlen(commands),0);
                fclose(list_command);
            }
            
                break;
            case ERROR:
                send(client_fd,"<SERVER> : Veuillez entrer une commande valide\n",48,0);
                break;            
            case DECONNECTION:
                
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
                char msg_send[2000];memset(msg_send,0,2000);
                sprintf(msg_send,"<SERVER> : %s a quitté le salon %s.\n",parametre_thread_recv->clients_body[real_ID_client].pseudo,parametre_thread_recv->clients_body[real_ID_client].nom_salon);
                for (int i = 0; i < nombre_de_client; i++)
                {
                    if (strcmp(parametre_thread_recv->clients_body[real_ID_client].nom_salon,parametre_thread_recv->clients_body[i].nom_salon)==0 && strcmp(parametre_thread_recv->clients_body[i].pseudo,parametre_thread_recv->clients_body[real_ID_client].pseudo)!=0)
                    {
                        send(parametre_thread_recv->clients_body[i].client_fd,msg_send,strlen(msg_send),0);
                    }
                    
                }
                memset(parametre_thread_recv->clients_body[real_ID_client].nom_salon,0,50);
                
            }
            else{
                char msg_send[2000];memset(msg_send,0,2000);
                // tab_recv[strlen(tab_recv)-1]=0;
                sprintf(msg_send,"<%s> : %s",parametre_thread_recv->clients_body[real_ID_client].pseudo,tab_recv);
                // strcat(msg_send,parametre_thread_recv->clients_body[real_ID_client].pseudo);
                // strcat(msg_send,tab_recv);
                for (int i = 0; i < nombre_de_client; i++)
                {
                    if (strcmp(parametre_thread_recv->clients_body[real_ID_client].nom_salon,parametre_thread_recv->clients_body[i].nom_salon)==0)
                    {
                        if (client_fd==parametre_thread_recv->clients_body[i].client_fd)
                        {
                        
                        }
                        else
                        {
                            send(parametre_thread_recv->clients_body[i].client_fd,msg_send,strlen(msg_send),0);perror("send msg()");
                        }
                    }   
                }
            }
        }
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
