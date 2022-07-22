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

#endif /* UTILS_h */
