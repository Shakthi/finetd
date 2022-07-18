//
//  untils.c
//  finetd
//
//  Created by Shakthi Prasad G S on 18/07/22.
//  Copyright © 2022 Shakthi Prasad G S. All rights reserved.
//

#include <stdarg.h>
#include "utils.h"
#include <stdlib.h>

extern int LOG_ENABLED;

int slogf (
        __const char *__restrict __format, ...)
{
    va_list args;
 
    va_start(args,__format);
 
    int return_status = 0;
    if(LOG_ENABLED)
        return_status = vprintf( __format, args);
    
 
    va_end(args);
    return return_status;
}


void
die(const char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}
