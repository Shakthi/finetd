//
//  service.c
//  finetd
//
//  Created by Shakthi Prasad G S on 17/07/22.
//  Copyright © 2022 Shakthi Prasad G S. All rights reserved.
//

#include "service.h"

pid_t execute(const char * cmd){
    pid_t pid = fork();
    if (pid == 0) {
        char * args[strlen(cmd)];
        char * rest = strdup(cmd);
        char * token = NULL;
        LOG_DEBUG("args %s",cmd);

        int i = 0;
        while ((token = strtok_r(rest, " ", &rest)))
        {
            args[i] = token;
            LOG_DEBUG("args %s",args[i]);
            i++;
        }
        
        
        args[i] = NULL;
        execvp(args[0], args);
        die("Failed exec");
        free(rest);//Not needed but still

    }
    return  pid;
}

void endService(int control[2],int index,pid_t pid,struct InetServicesDefintion def){
    char retcmd[] = "stoping listen 12345";
    sprintf(retcmd, "stoping listen %5d", index);
    write(control[1], retcmd, sizeof(retcmd));
    char killcmd[] = "kill xxxxxx";
    sprintf(killcmd, "kill %d",pid);
    
    int ret = execute(def.stopCommand?def.stopCommand:killcmd);
    if (ret == -1)
        die("kill");
    
    exit(0);
}

void
serviceFd(int fd, int index, int control[2], struct InetServicesDefintion def)
{

    pid_t pid = fork();

    if (pid == 0) {
        close(control[0]);


        char str[20];
        struct sockaddr_storage client_saddr;
        socklen_t addrlen = sizeof(struct sockaddr_storage);
        int fd_new;

        if ((fd_new = accept(fd, (struct sockaddr *)&client_saddr, &addrlen)) == -1)
            die("accept");



        struct sockaddr_in *ptr = (struct sockaddr_in *)&client_saddr;

        inet_ntop(AF_INET, &(ptr->sin_addr), str, sizeof(str));

        LOG_DEBUG("Request from %s", str);

        char cmd[1024];
        sprintf(cmd, def.startCommand, def.destinationPort);


        LOG_INFO("starting command:%s",cmd);
        pid_t pid = execute(cmd);
        
        int clients[20] = {fd_new};
        int remotes[20] = {0};

        int maxClients = 1;







        for (int i = 0; i < 10; i++) {

            struct sockaddr_in remote_addr;
            memset((void *)&remote_addr, 0, (size_t) sizeof(remote_addr));

            remote_addr.sin_family = AF_INET;
            remote_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
            remote_addr.sin_port = htons(def.destinationPort);


            remotes[0] = socket(AF_INET, SOCK_STREAM, 0);
            if (remotes[0] < 0)
                die("destination socket creation");

            if (connect(remotes[0], (struct sockaddr *)&remote_addr, sizeof(remote_addr)) == 0) {
                LOG_DEBUG("Conected remote for client 0");
                break;
            } else {
                LOG_DEBUG("trying to start");

                sleep(1);
            }
        }




        fd_set readfds;
        char buffer[4096];


        while (1) {
            FD_ZERO(&readfds);
            FD_SET(fd, &readfds);
            int maxFd = fd;

            for (int i = 0, clientCount = 0; (i < 20 && clientCount < maxClients); i++) {

                if (clients[i]) {
                    FD_SET(clients[i], &readfds);
                    FD_SET(remotes[i], &readfds);


                    maxFd = maxFd > clients[i] ? maxFd : clients[i];
                    maxFd = maxFd > remotes[i] ? maxFd : remotes[i];
                    clientCount++;
                }
            }



            struct timeval tv = {20, 0};    /* sleep for ten minutes */


            int ret = select(maxFd + 1, &readfds, NULL, NULL, &tv);
            if (ret < 0) {
                die("server select");
                break;
            } else if (ret == 0) {
                LOG_DEBUG("select timeout ");
                endService(control,index,pid,def);
            }


            if (FD_ISSET(fd, &readfds)) {
                //New client


                struct sockaddr_storage client_saddr;
                socklen_t addrlen = sizeof(struct sockaddr_storage);
                int fd_new;
                if ((fd_new = accept(fd, (struct sockaddr *)&client_saddr, &addrlen)) == -1) {
                    die("read new client");
                }


                for (int i = 0; i < 20; i++) {
                    if (clients[i] == 0) {
                        clients[i] = fd_new;
                        struct sockaddr_in remote_addr;
                        remote_addr.sin_family = AF_INET;
                        remote_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
                        remote_addr.sin_port = htons(def.destinationPort);



                        remotes[i] = socket(AF_INET, SOCK_STREAM, 0);
                        if (connect(remotes[i], (struct sockaddr *)&remote_addr, sizeof(remote_addr)) != 0)
                        {
                            LOG_ERROR("Failed connection with to remote %s",def.startCommand);
                        }
                        else
                            LOG_DEBUG("new connected ");
                        maxClients++;
                        break;
                    }else{
                        LOG_ERROR("reached max supported client limit");
                        close(fd_new);
                        
                    }
                }

            }
            for (int i = 0, clientCount = 0; (i < 20 && clientCount < maxClients); i++) {
                if (clients[i] && FD_ISSET(clients[i], &readfds)) {

                    size_t count = recv(clients[i], buffer, sizeof(buffer), 0);
                    LOG_DEBUG("Recevied %d bytes from client",count);
                    
                    if (count < 0) {
                        die("recv");
                        close(clients[i]);
                        close(remotes[i]);
                        clients[i] = 0;
                        remotes[i] = 0;
                        maxClients--;
                        LOG_DEBUG("-1");
                    }
                    if (count == 0) {
                        
                        close(clients[i]);
                        close(remotes[i]);

                        clients[i] = 0;
                        remotes[i] = 0;
                        maxClients--;
                        LOG_DEBUG("Closed event from client");



                    } else
                        send(remotes[i], buffer, count, 0);
                    clientCount++;
                }
                if (remotes[i] && FD_ISSET(remotes[i], &readfds)) {
                    size_t count = recv(remotes[i], buffer, sizeof(buffer), 0);
                    LOG_DEBUG("Recevied %d bytes from remote",count);
                    
                    if (count < 0) {
                        die("recieve error");
                        close(remotes[i]);
                        close(clients[i]);
                        clients[i] = 0;
                        remotes[i] = 0;
                        maxClients--;

                    }
                    if (count == 0) {
                        close(remotes[i]);
                        close(clients[i]);
                        clients[i] = 0;
                        remotes[i] = 0;
                        maxClients--;
                        LOG_DEBUG("Closed event from remote");
                    } else
                        send(clients[i], buffer, count, 0);

                }
            }


        }












    }
}

