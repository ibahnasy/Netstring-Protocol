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
 * Netstring protocol implementation
 * 
 * char * netstring_encode(const char * str);
 * void * netstring_decode(const char *str, size_t *size)
 *
 */


#include "netstring.h"

char *
netstring_encode(const char * str, size_t size)
{
	int msg_buf_sz = (int)size + (int) strlen(str) + 2;
	char msg_buf[39];
	char * data;

	snprintf(msg_buf, msg_buf_sz, "%d:%s,", (int) size, str); /* formatting to: length:string, */

	#ifdef DEBUG
		printf("msg_buf: '%s'\n", msg_buf);
	#endif

	data = (char *) malloc(msg_buf_sz * sizeof(char));
	return memcpy(data, msg_buf, msg_buf_sz);
}


void *
netstring_decode(const char *str, size_t *size)
{
	char *data;
	char msg_buf_sz[39]; /* Enough to store a string of size more than
						  * 2**128 or aprox. 10^38.532 bytes. Way more than a Yottabyte */
	char *endptr;
	register int i = 0;

	#ifdef DEBUG
		printf("str: %s\n", str); /* Mark your debug commands if used without macros. */
	#endif

	/* Make sure msg_buf_sz does not overflow. */
	while(*str != ':' && i < (int) sizeof(msg_buf_sz))
	{
		msg_buf_sz[i] = *str++;
		++i;
	}

	++str; /* Advance `str' pointer past ':' */

	*size = strtoll(msg_buf_sz, &endptr, 10);

	/* Error occurred or no digits found */
	if ((errno == ERANGE && ((long)*size == LONG_MAX || (long)*size == LONG_MIN))
			|| (errno != 0 && *size == 0) || (msg_buf_sz == endptr)) {
		return (char *)NULL;
	}

	/* Past this point we know that we parsed a length successfuly. */
	if (str[*size] != ',') /* Check if the data field ends with ',' per standard */
		return NULL;

	#ifdef DEBUG
		printf("Message len: %d\n", (int)*size);
	#endif

	data = (char *) malloc(*size * sizeof(char));
	bzero(data, sizeof(data));
	if (!data) /* Check if malloc() was successful */
		return NULL;

	return memcpy(data, str, *size);
}
