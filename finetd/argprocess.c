//
//argprocess.c
// finetd
//
//Created by Shakthi Prasad G S on 17 / 07 / 22.
// Copyright Â ©2022 Shakthi Prasad G S.All rights reserved.
//

#include <string.h>
#include "argprocess.h"
#include "service.h"
#include "utils.h"

void 
processArgs(int argc, const char *argv[], struct InetServicesDefintion **allService, int *totalServices)
{

	
	struct InetServicesDefintion servies[20];

	if (1 || argc == 3 && strcmp(argv[1], "-c") == 0) {
		*totalServices = 0;
		FILE *file = fopen("/Volumes/Development/Projects/finetd/finetd/service.sample.conf", "r");
		while (!feof(file)) {
			int source, destination;
			char command[1024];
			if(fscanf(file, "%d %d", &source, &destination)==0)
                break;


			if(!fgets(command, 1024, file))
                break;
            command[strcspn(command, "\n")] = 0;

			servies[*totalServices].sourcePort = source;
			servies[*totalServices].destinationPort = destination;

            char * rest = command;
            if(*rest == ' ')
                rest++;
            servies[*totalServices].startCommand = strdup(strtok_r(rest, ";", &rest));
            
            if(rest)
                servies[*totalServices].stopCommand = strdup(strtok_r(rest, ";", &rest));

            
			(*totalServices)++;
			if (*totalServices >= 20) {
				die("Exeeds maxsupported services");
			}
		}
		fclose(file);

	}
	*allService = malloc(sizeof(servies));
	memcpy(*allService, servies, sizeof(servies));

    slogf("total servces descovered %d\n",*totalServices);

    /*


     const struct InetServicesDefintion * allServices[] = {
         {
             2014,
             4028,
             "/usr/local/bin/node /usr/local/bin/serve -l 4028",
         },
         
         {
                 2020,
                 4040,
                 "/usr/local/bin/node /usr/local/bin/serve -l 4040",
         }
     };
     */



}
