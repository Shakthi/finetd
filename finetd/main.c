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


int main(int argc, const char * argv[]) {
    
    /* Create IPv4 socket */
    struct sockaddr_in serv_addr4, cli_addr4;
    int addrlen4 = sizeof(cli_addr4);
    int sockfd_v4,option;
    int BACKLOG=20;
    

    if ((sockfd_v4 = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return 1;
    }

    setsockopt(sockfd_v4, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int));

    serv_addr4.sin_family = AF_INET;
    serv_addr4.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr4.sin_port = htons(3000);

    if (bind(sockfd_v4, (struct sockaddr *)&serv_addr4, sizeof(serv_addr4)) < 0) {
        perror("bind");
        return 1;
    }

    if (listen(sockfd_v4, BACKLOG) < 0) {
        perror("listen");
        return 1;
    }
    
    
    
    // insert code here...
    printf("Hello, World!\n");
    return 0;
}
