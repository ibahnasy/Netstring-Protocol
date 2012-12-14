/*
 *
 * Copyright (C) 2012
 * Author: Islam Bahnasy <islam.bahnasy@gmail.com>
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 */

/*
 * Netstring protocol header file
 * 
 * char * netstring_encode(const char * str);
 * void * netstring_decode(const char *str, size_t *size)
 *
 */

#ifndef NETSTRING_H
#define NETSTRING_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h> /* errno */
#include <limits.h> /* LONG_MAX LONG_MIN */

#define BUFFER_SIZE 39

char *
netstring_encode(const char * str, size_t size);

void *
netstring_decode(const char *str, size_t *size);


#endif