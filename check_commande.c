#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


int check_commande(char* commande,char**arg_commande,int nombre_de_mot){
    // int nombre_de_mot=get_nombre_de_mot(commande);

    commande[strlen(commande)-1]=0;
    printf("COMMANDE : \"%s\"\n",commande);
    printf("NOMBRE DE MOT : %d\n",nombre_de_mot);
    if (strcmp(commande,"code_45421354")==0)
    {
        printf("ouiouioiuoiuoiuoiuoi\n");
        return -2;
    }
    
    
    arg_commande[0]=strtok(commande," ");
    for (int i = 1; i < nombre_de_mot; i++)
    {
        arg_commande[i]=strtok(NULL," ");
    }
    
    if (strcmp(arg_commande[0],"create")==0)
    {
        return 1;

    }
    else if (strcmp(arg_commande[0],"list")==0)
    {
        return 2;
    }
    else if (strcmp(arg_commande[0],"join")==0)
    {
        return 3;
    }
    else if (strcmp(arg_commande[0],"exit")==0)
    {
        
        return 4;
    }
    else if (strcmp(arg_commande[0],"invite")==0)
    {
        return 5;
    }
    else if (strcmp(arg_commande[0],"command")==0)
    {
        return 6;
    }
    
    else
    {
        return -1;
    }
    
}

