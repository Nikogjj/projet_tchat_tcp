#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int check_error_start_client(int argc,char **argv,char pseudo[1000],int* port_client){
    if (argc==1 || argc==2)
    {
        FILE* error=fopen("error_start_client","r");
        char error_start[1000];memset(error_start,0,1000);
        fread(error_start,1,1000,error);
        printf("%s\n",error_start);
        fclose(error); 
        return -1;
    }

    if (strcmp(argv[1],"connect")!=0 || argc>=5)
    {
        FILE* error=fopen("error_start_client","r");
        char error_start[1000];memset(error_start,0,1000);
        fread(error_start,1,1000,error);
        printf("%s\n",error_start);
        fclose(error); 
        return -1;
    }
    
    else if (argc==3)
    {
        strcpy(pseudo,argv[2]);
        if (strlen(pseudo)>20)
        {
            printf("Le peudo contient trop de caractère\n");
        }
        printf("Votre pseudo : %s\n",pseudo);
    }
    else if (argc==4)
    {
        strcpy(pseudo,argv[2]);
        if (strlen(pseudo)>20)
        {
            printf("Le peudo contient trop de caractère\n");
        }
        printf("Votre pseudo : %s\n",pseudo);
        if (argc>2)
        {
            int port;
            port=atoi(argv[3]);
            *port_client=port;
        }
    }
    return 0;
}