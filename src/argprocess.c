//
// argprocess.c
// finetd
//
// Created by Shakthi Prasad G S on 17 / 07 / 22.
// Copyright � �2022 Shakthi Prasad G S.All rights reserved.
//

#include <getopt.h>
#include <libgen.h>
#include <string.h>

#include "argprocess.h"
#include "service.h"
#include "utils.h"

/** Program for ondemand loading of service by listening in their ports
 */
void print_usage() {
  printf("Usage: finetd --config config_path --loglevel loglevelNumber -t "
         "timeOutSeconds\n");
}

void processArgs(int argc, char *argv[],
                 struct InetServicesDefintion *allService, int *totalServices,
                 int *loglevel, int *timeOut) {

  struct InetServicesDefintion *services = allService;

  int c;
  char *configFilePath = NULL;
  int option_index = 0;

  while (1) {
    struct option long_options[] = {{"config", required_argument, 0, 'c'},
                                    {"loglevel", required_argument, NULL, 'l'},
                                    {"help", required_argument, NULL, 'h'},
                                    {"timeout", required_argument, NULL, 't'},
                                    {"testcase", 0, NULL, 'T'},
                                    {0, 0, 0, 0}};

    c = getopt_long(argc, argv, "c:l:t:hT", long_options, &option_index);
    if (c == -1)
      break;

    switch (c) {

    case 'c':
      configFilePath = strdup(optarg);
      LOG_DEBUG("configFilePath %s", configFilePath);
      break;

    case 'l':
      *loglevel = atoi(optarg);
      LOG_DEBUG("loglevel %d", *loglevel);
      break;

    case 't':
      *timeOut = atoi(optarg);
      LOG_DEBUG("timeOut %d", *timeOut);
      break;

    case 'T':
      /* test case for debug*/
      {
        char conffile[1024] = "";
        strcat(conffile, dirname(__FILE__));
        strcat(conffile, "/service.sample.conf");
        configFilePath = strdup(conffile);
        *timeOut = 15;
        LOG_DEBUG("timeOut %d", *timeOut);
        LOG_DEBUG("configFilePath %s", configFilePath);
      }
      break;

    case 'h':

    default:
      print_usage();
    }
  }

  if (optind < argc) {
    LOG_WARNING("Ignoring extra %d arguments", argc - optind);
  }

  if (!configFilePath) {
    print_usage();
    die("config file not provided");
  }

  /*Load the services*/
  *totalServices = 0;
  FILE *file = fopen(configFilePath, "r");
  if (!file) {
    die("Unable to open config file %s", configFilePath);
  }
  while (!feof(file)) {
    int source, destination;
    char command[1024];
    if (fscanf(file, "%d %d", &source, &destination) == 0)
      break;

    if (!fgets(command, 1024, file))
      break;
    command[strcspn(command, "\n")] = 0;

    services[*totalServices].sourcePort = source;
    services[*totalServices].destinationPort = destination;

    char *rest = command;
    if (*rest == ' ')
      rest++;
    services[*totalServices].startCommand = strdup(strtok_r(rest, ";", &rest));

    if (rest)
      services[*totalServices].stopCommand = strdup(strtok_r(rest, ";", &rest));

    (*totalServices)++;
    if (*totalServices >= 20) {
      die("Exceeds max supported services");
    }
  }
  fclose(file);

  LOG_INFO("total services discovered %d", *totalServices);

  /*
   * const struct InetServicesDefintion * allServices[] = { { 2014, 4028,
   * "/usr/local/bin/node /usr/local/bin/serve -l 4028", },
   *
   * { 2020, 4040, "/usr/local/bin/node /usr/local/bin/serve -l 4040", }
   * }; */
}
