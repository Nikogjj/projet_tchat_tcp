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

int set_server(int port_server,int server_fd){
    struct sockaddr_in server={
            .sin_addr.s_addr=INADDR_ANY,
            .sin_family=AF_INET,
            .sin_port=htons(port_server)   
        };
    
    server_fd = socket(AF_INET,SOCK_STREAM,0);perror("socket()");
    
    int check_error = bind(server_fd,(struct sockaddr*)&server,sizeof server);perror("bind()");
    if (check_error==-1)return -1;
    
    check_error = listen(server_fd,1000);perror("listen()");
    if (check_error==-1)return -1;

    return server_fd;

}
