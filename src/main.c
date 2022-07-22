//
//main.c
// finetd
//
//Created by Shakthi Prasad G S on 13 / 07 / 22.
// Copyright � �����2022 Shakthi Prasad G S.All rights reserved.
//

#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/select.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>




#define ArraySize(x) (sizeof (x)/sizeof(x[0]))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y)


#include "service.h"
#include "argprocess.h"



int
listenAtPort(int port)
{
	/* Create IPv4 socket */
	struct sockaddr_in serv_addr4;
	int sockfd_v4, option;
	int BACKLOG = 20;

    LOG_DEBUG("Asked to listen at port %d", port);


	if ((sockfd_v4 = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		die("socket");
		return 1;
	}
	setsockopt(sockfd_v4, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int));

	serv_addr4.sin_family = AF_INET;
	serv_addr4.sin_addr.s_addr = inet_addr("0.0.0.0");
	serv_addr4.sin_port = htons(port);

	if (bind(sockfd_v4, (struct sockaddr *)&serv_addr4, sizeof(serv_addr4)) < 0) {
		die("bind");
		return -1;
	}
	if (listen(sockfd_v4, BACKLOG) < 0) {
		die("listen");
		return -1;
	}
	LOG_INFO("listening at port %d", port);
	return sockfd_v4;
}

enum {
    serviceIdle=1,
    serviceBusy=2
    
};




int LOG_ENABLED = 10;
int
main(int argc,  char *argv[])
{


    struct InetServicesDefintion * allServices;
    int totalServices=0;
    processArgs(argc,argv,&allServices,&totalServices,&LOG_ENABLED);
    
    

	struct InetServicesRecord inetServicesRecord[20] = {{0}};

	/* Pipe to signal death of child */
	int controlPipe[2];

	if (pipe(controlPipe) == -1)
		die("cannot create pipe");


    LOG_INFO("Starting server");

	fd_set readFds;
	int maxFd = 0;
	maxFd = MAX(maxFd, controlPipe[0]);

	for (int i = 0; i < totalServices; i++) {
        LOG_DEBUG("new service %d",allServices[i].sourcePort);
		inetServicesRecord[i].masterSocket = listenAtPort(allServices[i].sourcePort);
		inetServicesRecord[i].status = serviceIdle;
		maxFd = MAX(maxFd, inetServicesRecord[i].masterSocket);
	}

    LOG_INFO("Entering main loop with %d services",totalServices);

	while (1) {
		FD_ZERO(&readFds);
		FD_SET(controlPipe[0], &readFds);

		for (int i = 0; i < ArraySize(inetServicesRecord); i++) {
			if (inetServicesRecord[i].status == serviceIdle)
				FD_SET(inetServicesRecord[i].masterSocket, &readFds);
		}


		int activity = select(maxFd + 1, &readFds, NULL, NULL, NULL);
		if ((activity < 0) ) {
			die("select error");
		}
		for (int i = 0; i < ArraySize(inetServicesRecord); i++) {
			if (inetServicesRecord[i].status == serviceIdle) {
				if (FD_ISSET(inetServicesRecord[i].masterSocket, &readFds)) {
					serviceFd(inetServicesRecord[i].masterSocket, i, controlPipe, allServices[i]);
					inetServicesRecord[i].status = serviceBusy;
				}
			}
		}


        /* We listen for message service exit message at the pipe*/
		if (FD_ISSET(controlPipe[0], &readFds)) {
            /*Message template*/
			char message[] = "stopping listen 00000";
			read(controlPipe[0], message, sizeof(message));

            /* signature of closure message*/
			char stopping[250];
			char listen[250];
			int serviceIndex;

			sscanf(message, "%s %s %d", stopping, listen, &serviceIndex);

			if (strcmp(stopping, "stopping") == 0 && strcmp(listen, "listen") == 0) {
				inetServicesRecord[serviceIndex].status = serviceIdle;
			}
		}
	}






	//insert code here...
		LOG_DEBUG("Hello, World!");
	return 0;
}
