//
//  untils.c
//  finetd
//
//  Created by Shakthi Prasad G S on 18/07/22.
//  Copyright © 2022 Shakthi Prasad G S. All rights reserved.
//
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "utils.h"


extern int LOG_ENABLED;



int LOG_DEBUG (
        __const char *__restrict __format, ...)
{
    va_list args;
 
    va_start(args,__format);
    char dest[strlen(__format)+1];
    
    strcpy(dest,__format);
    strcat(dest, "\n");

 
    int return_status = 0;
    if(LOG_ENABLED<=10)
        return_status = vprintf( dest, args);
    
 
    va_end(args);
    return return_status;
}



int LOG_WARNING (
        __const char *__restrict __format, ...)
{
    va_list args;
 
    va_start(args,__format);
    char dest[strlen(__format)+1];
    
    strcpy(dest,__format);
    strcat(dest, "\n");

 
    int return_status = 0;
    if(LOG_ENABLED<=30)
        return_status = vprintf( dest, args);
    
 
    va_end(args);
    return return_status;
}

int LOG_ERROR (
        __const char *__restrict __format, ...)
{
    va_list args;
 
    va_start(args,__format);
    char dest[strlen(__format)+1];
    
    strcpy(dest,__format);
    strcat(dest, "\n");

 
    int return_status = 0;
    if(LOG_ENABLED<=40)
        return_status = vprintf( dest, args);
    
 
    va_end(args);
    return return_status;
}


int LOG_INFO (
        __const char *__restrict __format, ...)
{
    va_list args;
 
    va_start(args,__format);
    char dest[strlen(__format)+1];
    
    strcpy(dest,__format);
    strcat(dest, "\n");

 
    int return_status = 0;
    if(LOG_ENABLED<=20)
        return_status = vprintf( dest, args);
    
 
    va_end(args);
    return return_status;
}

void
die(__const char *__restrict __format, ...)
{
   va_list args;
    
   va_start(args,__format);
   char dest[strlen(__format)+1];
   
   strcpy(dest,__format);
   strcat(dest, "\n");


   int return_status = 0;
   if(LOG_ENABLED<=50)
       
       return_status = vprintf( dest, args);
   

   va_end(args);

}
