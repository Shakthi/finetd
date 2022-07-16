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
    
        printf("listenning at port %d",port);
    return sockfd_v4;
}



void serviceFd(int fd,int port){
    
    pid_t pid = fork();
//    if(pid>0)
//        close(fd);
    if(pid == 0){
        

        char str[20];
            struct sockaddr_storage client_saddr;
            socklen_t addrlen = sizeof (struct sockaddr_storage);
            int fd_new;

            if ((fd_new = accept (fd, (struct sockaddr *) &client_saddr, &addrlen)) == -1)
             perror ("accept");
        
        int remoteport = port*2;
        
        
        struct sockaddr_in * ptr = (struct sockaddr_in *) &client_saddr;

         inet_ntop (AF_INET, &(ptr-> sin_addr), str, sizeof (str));
        
        printf("Request from %s\n",str);

        char cmd[1024];
        sprintf(cmd, "/usr/local/bin/node /usr/local/bin/serve -l %d 1>/dev/null",remoteport);
        printf("cmd %s",cmd);

        pid_t pid = fork();
        if(pid==0){
            exit(system(cmd));
        }
        
        int clients[20]={fd_new};
        int remotes[20]={0};

        int maxClients=1;
        
       

     

        

        for (int i=0; i<10; i++) {
            
           struct sockaddr_in remote_addr;
           memset((void*)&remote_addr, 0,(size_t) sizeof(remote_addr));

           remote_addr.sin_family = AF_INET;
           remote_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
           remote_addr.sin_port = htons(remoteport);
            

            remotes[0] = socket(AF_INET, SOCK_STREAM, 0);
            if (remotes[0]<0)
                perror("serviceFd: socket()");
            
            if (connect(remotes[0], (struct sockaddr *) &remote_addr, sizeof(remote_addr)) == 0)
            {
                printf("Conected");
                
                
                break;
            }else{
                perror("\ntrying to start");

                sleep(1);
            }
        }
        
        
        
        
        fd_set readfds;
        char buffer [4096];
        
        while (1) {
            FD_ZERO(&readfds);
            FD_SET(fd, &readfds);
            int maxFd=0;
            for (int i=0,clientCount=0; (i<20 && clientCount<maxClients); i++) {
                printf("came inside");

                if (clients[i]) {
                    FD_SET(clients[i], &readfds);
                    FD_SET(remotes[i], &readfds);
                    printf("fd set");


                    maxFd = maxFd>clients[i]?maxFd:clients[i];
                    maxFd = maxFd>remotes[i]?maxFd:remotes[i];

                    clientCount++;
                }
            }
            
            
            printf("before service select");

            
            if (select(maxFd+1, &readfds, NULL, NULL, NULL) < 0)
            {
                perror("use_tunnel: select()");
                break;
            }
            
            printf("after service select");
            
            for (int i=0,clientCount=0; (i<20 && clientCount<maxClients); i++) {
                   if (clients[i]&&FD_ISSET(clients[i], &readfds)) {
                       
                       int count = recv(clients[i], buffer, sizeof(buffer), 0);
                       if (count < 0)
                       {
                           perror("use_tunnel: recv(rc.client_socket)");
                           close(clients[i]);
                           close(remotes[i]);
                           clients[i]=0;
                           maxClients--;
                       }

                       if (count == 0)
                       {
                           close(clients[i]);
                           close(remotes[i]);
                           
                           clients[i]=0;
                           maxClients--;
                       }

                       send(remotes[i], buffer, count, 0);
                       clientCount++;
                   }
                
                
                    if (FD_ISSET(remotes[i], &readfds))
                    {
                        int count = recv(remotes[i], buffer, sizeof(buffer), 0);
                        if (count < 0)
                        {
                            perror("use_tunnel: recv(rc.remote_socket)");
                            close(remotes[i]);
                            close(clients[i]);
                        }

                        if (count == 0)
                        {
                            perror("use_tunnel: recv(rc.remote_socket)");
                            close(remotes[i]);
                            close(clients[i]);

                        }

                        send(clients[i], buffer, count, 0);

                    }
                
                    if (FD_ISSET(fd, &readfds)) {//New client
                        
                        struct sockaddr_storage client_saddr;
                        socklen_t addrlen = sizeof (struct sockaddr_storage);
                        int fd_new;
                        if ((fd_new = accept (fd, (struct sockaddr *) &client_saddr, &addrlen)) == -1){
                            perror("read new client");
                        }
                        
                        for (int i=0; i<20; i++) {
                            if(clients[i]==0){
                                clients[i]=fd_new;
                                struct sockaddr_in remote_addr;
                                       remote_addr.sin_family = AF_INET;
                                       remote_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
                                       remote_addr.sin_port = htons(remoteport);

                                    

                                       remotes[i] = socket(AF_INET, SOCK_STREAM, 0);
                                if (connect(remotes[i], (struct sockaddr *) &remote_addr, sizeof(remote_addr)) != 0)
                                    perror("new connect");

                                break;
                            }
                        }
                    
                    }

                
                
                
            }
            
       
        }
        
        


        
        
        
        
        
        

        
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
    
    int  ports[] = {3056};
    fd_set readfds;
    FD_ZERO(&readfds);
    
    int busyPorts[2]={0,0};

    
    int sockets[2]={};
    int maxfd=0;
    for (int i=0; i<1; i++) {
        sockets[i] = listenAtPort(ports[i]);
        maxfd = maxfd>sockets[i]?maxfd:sockets[i];
      //add master socket to set
    }
    
    while (1) {
        FD_ZERO(&readfds);
        if(!busyPorts[0])
            FD_SET(sockets[0], &readfds);
         
        
        int activity = select( maxfd + 1 , &readfds , NULL , NULL , NULL);

        if ((activity < 0) && (errno!=EINTR))
        {
            printf("select error");
        }
        for (int fd=0,i = 0; fd < (maxfd + 1); fd++) {
              if (FD_ISSET (fd, &readfds)) {  // fd is ready for reading
                  busyPorts[0]=1;
                  serviceFd(fd,ports[0]);
                  i++;
              }
        }
                          

        
    }
    
    
    
    
    
    
    // insert code here...
    printf("Hello, World!\n");
    return 0;
}
