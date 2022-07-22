//
//  untils.c
//  finetd
//
//  Created by Shakthi Prasad G S on 18/07/22.
//  Copyright © 2022 Shakthi Prasad G S. All rights reserved.
//
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "utils.h"

#include <errno.h>

extern int LOG_ENABLED;



int 
LOG_DEBUG(
	  __const char *__restrict __format,...)
{
	va_list args;

	va_start(args, __format);
	char dest[strlen(__format) + 1];

	strcpy(dest, __format);
	strcat(dest, "\n");


	int return_status = 0;
	if (LOG_ENABLED <= 10)
		return_status = vprintf(dest, args);


	va_end(args);
	return return_status;
}



int 
LOG_WARNING(
	    __const char *__restrict __format,...)
{
	va_list args;

	va_start(args, __format);
	char dest[strlen(__format) + 1];

	strcpy(dest, __format);
	strcat(dest, "\n");


	int return_status = 0;
	if (LOG_ENABLED <= 30)
		return_status = vprintf(dest, args);


	va_end(args);
	return return_status;
}

int 
LOG_ERROR(
	  __const char *__restrict __format,...)
{
	va_list args;

	va_start(args, __format);
	char dest[strlen(__format) + 1];

	strcpy(dest, __format);
	strcat(dest, "\n");


	int return_status = 0;
	if (LOG_ENABLED <= 40)
		return_status = vprintf(dest, args);


	va_end(args);
	return return_status;
}


int 
LOG_INFO(
	 __const char *__restrict __format,...)
{
	va_list args;

	va_start(args, __format);
	char dest[strlen(__format) + 1];

	strcpy(dest, __format);
	strcat(dest, "\n");


	int return_status = 0;
	if (LOG_ENABLED <= 20)
		return_status = vprintf(dest, args);


	va_end(args);
	return return_status;
}

void
die(__const char *__restrict __format,...)
{
	va_list args;

	va_start(args, __format);
	char destination[strlen(__format) + 1024];



	if (LOG_ENABLED <= 50) {
		vsnprintf(destination, sizeof(destination), __format, args);
        if (!errno)
            strcat(destination, "\n");
	}
	va_end(args);

	va_end(args);

	if (errno)
		perror(destination);
	else
		printf("%s", destination);
	exit(1);

}


//  https://stackoverflow.com/questions/779875/what-function-is-to-replace-a-substring-from-a-string-in-c
char *str_replace(const char *orig,const  char *rep, const char *with) {
    char *result; // the return string
    const char *ins;    // the next insert point
    char *tmp;    // varies
    size_t len_rep;  // length of rep (the string to remove)
    size_t len_with; // length of with (the string to replace rep with)
    size_t len_front; // distance between rep and end of last rep
    int count;    // number of replacements

    // sanity checks and initialization
    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return NULL; // empty rep causes infinite loop during count
    if (!with)
        with = "";
    len_with = strlen(with);

    // count the number of replacements needed
    ins = orig;
    for (count = 0; (tmp = strstr(ins, rep)); ++count) {
        ins = tmp + len_rep;
    }

    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}


void str_replace_inplace(const char *orig, const char *rep, const char *with,char * destination) {

    char * out =  str_replace(orig,rep,with);
    strcpy(destination, out);
    if(out)
        free(out);
}
