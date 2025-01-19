#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int check_commande_deconnexion(char* command){
    printf("LA COMMANDE : %s\n\n",command);
    char* arg_commande[50];
    printf("debut\n");
    arg_commande[0]=strtok(command," ");
    printf("commande 1 :%s\n",arg_commande[0]);
    arg_commande[1]=strtok(NULL," ");
    printf("commande 2 :%s\n",arg_commande[1]);
    // printf("commande 2 : %s\n",arg_commande[1]);
    // int ID_client;
    // sscanf(command,"code_45421354 %d/",&ID_client);
    // printf("ID_client = %d\n",ID_client);
    // printf("ID CLIENT DANS CHECK 10 : %c/\n",command[10]);
    // printf("ID CLIENT DANS CHECK 11 : %c/\n",command[11]);
    // printf("ID CLIENT DANS CHECK 12 : %c/\n",command[12]);
    // printf("ID CLIENT DANS CHECK 13 : %c/\n",command[13]);
    // printf("ID CLIENT DANS CHECK 14 : %c/\n",command[14]);
    // printf("ID CLIENT DANS CHECK 15 : %c/\n",command[15]);
    // printf("ID CLIENT DANS CHECK : %d\n",ID_client);
    return atoi(arg_commande[1]);
}