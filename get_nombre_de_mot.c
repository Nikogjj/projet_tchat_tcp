#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int get_nombre_de_mot(char * commande){
    if (strcmp(commande,"\n")==0)
    {
        return -1;
    }
    if (commande[0]==' ')
    {
        return -1;
    }
    
    
    int nombre=0;
    for (int i = 0; i < strlen(commande); i++)
    {
        if (commande[i]==' ' || commande[i]=='\n')
        {
            nombre++;
        }
    }
    // if (nombre>=4)
    // {
    //     return -1;
    // }
    // else{
        return nombre;
    // }
    
    // commande[strlen(commande)-1]=0;
}