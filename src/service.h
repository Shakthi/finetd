//
//  service.h
//  finetd
//
//  Created by Shakthi Prasad G S on 17/07/22.
//  Copyright © 2022 Shakthi Prasad G S. All rights reserved.
//

#ifndef service_h
#define service_h

#include "utils.h"
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

struct InetServicesDefintion {
  int sourcePort;
  int destinationPort;
  const char *startCommand;
  const char *stopCommand;
};

struct InetServicesRecord {
  int status;
  pid_t pid;
  int masterSocket;
};

void serviceFd(int fd, int index, int control[2],
               struct InetServicesDefintion def,int serviceTimeOut);
int listenAtPort(int port);

#endif /* service_h */
