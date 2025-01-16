#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int get_nombre_de_mot(char * commande){
    int nombre=0;
    for (int i = 0; i < strlen(commande); i++)
    {
        if (commande[i]==' ' || commande[i]=='\n')
        {
            nombre++;
        }
    }
    commande[strlen(commande)-1]=0;
    return nombre;
}