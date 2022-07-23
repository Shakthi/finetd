//
// main.c
// finetd
//
// Created by Shakthi Prasad G S on 13 / 07 / 22.
// Copyright � �����2022 Shakthi Prasad G S.All rights reserved.
//

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>


#include "argprocess.h"
#include "service.h"

enum {
  serviceIdle = 1,
  serviceBusy = 2

};

int LOG_ENABLED = 10;
int main(int argc, char *argv[]) {

  struct InetServicesRecord inetServicesRecord[20] = {};
  struct InetServicesDefintion allServices[20];

  int totalServices = 0;
  int serviceTimeOut = 3600;

    
  int controlPipe[2]; // Pipe to signal death of child

  processArgs(argc, argv, allServices, &totalServices,
              &LOG_ENABLED,&serviceTimeOut);

  if (pipe(controlPipe) == -1)
    die("cannot create pipe");

  LOG_INFO("Starting server");

  fd_set readFds;
  int maxFd = 0;
  maxFd = MAX(maxFd, controlPipe[0]);

  for (int i = 0; i < totalServices; i++) {
    LOG_DEBUG("new service %d", allServices[i].sourcePort);
    inetServicesRecord[i].masterSocket =
        listenAtPort(allServices[i].sourcePort);
    inetServicesRecord[i].status = serviceIdle;
    maxFd = MAX(maxFd, inetServicesRecord[i].masterSocket);
  }

  LOG_INFO("Entering main loop with %d services", totalServices);

  while (1) {
    FD_ZERO(&readFds);
    FD_SET(controlPipe[0], &readFds);

    for (int i = 0; i < ArraySize(inetServicesRecord); i++) {
      if (inetServicesRecord[i].status == serviceIdle)
        FD_SET(inetServicesRecord[i].masterSocket, &readFds);
    }

    int activity = select(maxFd + 1, &readFds, NULL, NULL, NULL);
    if ((activity < 0)) {
      die("select error");
    }
    for (int i = 0; i < ArraySize(inetServicesRecord); i++) {
      if (inetServicesRecord[i].status == serviceIdle) {
        if (FD_ISSET(inetServicesRecord[i].masterSocket, &readFds)) {
          serviceFd(inetServicesRecord[i].masterSocket, i, controlPipe,
                    allServices[i],serviceTimeOut);
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

  return 0;
}
