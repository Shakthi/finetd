//
//  untils.h
//  finetd
//
//  Created by Shakthi Prasad G S on 18/07/22.
//  Copyright © 2022 Shakthi Prasad G S. All rights reserved.
//

#ifndef UTILS_h
#define UTILS_h

int LOG_DEBUG(__const char *__restrict __format, ...);
int LOG_INFO(__const char *__restrict __format, ...);

int LOG_WARNING(__const char *__restrict __format, ...);

int LOG_ERROR(__const char *__restrict __format, ...);

void die(__const char *__restrict __format, ...);

void str_replace_inplace(const char *orig, const char *rep, const char *with,char * destination);

#define ArraySize(x) (sizeof(x) / sizeof(x[0]))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))


#endif /* UTILS_h */
