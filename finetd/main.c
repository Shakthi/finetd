//
//  main.c
//  finetd
//
//  Created by Shakthi Prasad G S on 13/07/22.
//  Copyright © 2022 Shakthi Prasad G S. All rights reserved.
//

#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <errno.h>
//#include <errno.h>
#include <sys/select.h>
#include <unistd.h>
#include <stdlib.h>


int listenAtPort(int port){
    /* Create IPv4 socket */
       struct sockaddr_in serv_addr4;
       int sockfd_v4,option;
       int BACKLOG=20;
       

       if ((sockfd_v4 = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
           perror("socket");
           return 1;
       }

       setsockopt(sockfd_v4, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int));

       serv_addr4.sin_family = AF_INET;
       serv_addr4.sin_addr.s_addr = inet_addr("0.0.0.0");
       serv_addr4.sin_port = htons(port);

       if (bind(sockfd_v4, (struct sockaddr *)&serv_addr4, sizeof(serv_addr4)) < 0) {
           perror("bind");
           return -1;
       }

       if (listen(sockfd_v4, BACKLOG) < 0) {
           perror("listen");
           return -1;
       }
    return sockfd_v4;
}



void serviceFd(int fd){
    pid_t pid = fork();
    if(pid == 0){
        char str[20];
            struct sockaddr_storage client_saddr;
            socklen_t addrlen = sizeof (struct sockaddr_storage);
            int fd_new;
            if ((fd_new = accept (fd, (struct sockaddr *) &client_saddr, &addrlen)) == -1)
             printf ("accept");
        struct sockaddr_in * ptr = (struct sockaddr_in *) &client_saddr;

         inet_ntop (AF_INET, &(ptr-> sin_addr), str, sizeof (str));
        
        printf("Request from %s\n",str);

        pid_t pid = fork();
        if(pid==0){
            exit(system("serve -l 8080"));
        }
        
        
        
        struct sockaddr_in remote_addr;
        int remote_socket;
        remote_addr.sin_family = AF_INET;
        remote_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        remote_addr.sin_port = htons(8080);

     

        remote_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (remote_socket < 0)
            perror("serviceFd: socket()");

        for (int i=0; i<10; i++) {
            if (connect(remote_socket, (struct sockaddr *) &remote_addr, sizeof(remote_addr)) ==0)
            {
                break;
            }else{
                sleep(1);
            }
        }
        
        
        fd_set readfds;
        fd_set writefds;


        
        
        
        
        
        

        
    }
}



struct inetServicesDefintion {
    int sourcePort;
    int destinationPort;
    const char * startCommand;
    const char **startCommandArgs;
    const char * stopCommand;
    const char **stopCommandArgs;
};


struct inetServicesRecord {
    int status;
    pid_t pid;
    const char * command;
    const char **args;
    int masterSocket;
};



int main(int argc, const char * argv[]) {
    
    int  ports[] = {3002};
    fd_set readfds;
    FD_ZERO(&readfds);
    
    int busyPorts[2]={0,0};

    
    int sockets[2];
    int maxfd=0;
    for (int i=0; i<2; i++) {
        sockets[i] = listenAtPort(ports[i]);
        maxfd = maxfd>sockets[i]?maxfd:sockets[i];
      //add master socket to set
    }
    
    while (1) {
        for (int i=0; i<2; i++) {
             if(!busyPorts[i])
                 FD_SET(sockets[i], &readfds);
         }
        
        int activity = select( maxfd + 1 , &readfds , NULL , NULL , NULL);

        if ((activity < 0) && (errno!=EINTR))
        {
            printf("select error");
        }
        for (int fd=0,i = 0; fd < (maxfd + 1); fd++) {
              if (FD_ISSET (fd, &readfds)) {  // fd is ready for reading
                  busyPorts[i]=1;
                  serviceFd(fd);
                  i++;
              }
        }
                          

        
    }
    
    
    
    
    
    
    // insert code here...
    printf("Hello, World!\n");
    return 0;
}
