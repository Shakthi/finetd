//
//  service.h
//  finetd
//
//  Created by Shakthi Prasad G S on 17/07/22.
//  Copyright © 2022 Shakthi Prasad G S. All rights reserved.
//

#ifndef service_h
#define service_h

#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <errno.h>
//
#include <errno.h>
#include <sys/select.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "utils.h"


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

void
serviceFd(int fd, int index, int control[2], struct InetServicesDefintion def);
void
die(const char *message);

#endif /* service_h */
