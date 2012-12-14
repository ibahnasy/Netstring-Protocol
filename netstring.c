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

#include <stdio.h> /* snprintf */
#include <stdlib.h> /* strtoll */
#include <string.h> /* memcpy */
#include <errno.h> /* errno */
#include <limits.h> /* LONG_MAX, LONG_MIN */

#include "netstring.h"

#define SIZE_BUFFER_SZ 39

char *
netstring_encode(const void *data, size_t size, size_t *res_size)
{
	int rc;
	char *res;
	char size_buf[SIZE_BUFFER_SZ];
	size_t msg_buf_sz = size + 3; /* 3 bytes extra for ':', ',', and '\x00' */

	rc = snprintf(size_buf, sizeof(size_buf), "%u", (unsigned int)size);

	if (rc < 0) /* We encountered an error */
		return NULL;
	else if (rc >= (int)sizeof(size_buf)) /* Size of data is dangerously more than
											 what we can handle. ABORT! */
		return NULL;
	else /* We successfully converted the size into a char array */
		msg_buf_sz += rc;

	res = (char *) malloc(msg_buf_sz);
	*res_size = snprintf(res, msg_buf_sz, "%s:%s,", size_buf,
			(const char *)data);

	/* Note that res_size will not include '\x00' */
	if (*res_size + 1 != msg_buf_sz) /* Something aweful happened */
		return NULL;

	return res;
}

void *
netstring_decode(const char *data, size_t *size)
{
	char *res;
	char msg_buf_sz[SIZE_BUFFER_SZ]; /* Enough to store a string of size more than
										2^128 or aprox. 10^38.532 bytes. Way more than a Yottabyte */
	char *endptr;
	register unsigned int i = 0;

	/* Make sure msg_buf_sz does not overflow. */
	while(*data != ':' && i < sizeof(msg_buf_sz))
	{
		msg_buf_sz[i] = *data++;
		++i;
	}

	++data; /* Advance `data' pointer past ':' */

	*size = strtoll(msg_buf_sz, &endptr, 10);

	/* Error occurred or no digits found */
	if ((errno == ERANGE && ((long)*size == LONG_MAX || (long)*size == LONG_MIN))
			|| (errno != 0 && *size == 0) || (msg_buf_sz == endptr)) {
		return (char *)NULL;
	}

	/* Past this point we know that we parsed a length successfuly. */
	if (data[*size] != ',') /* Check if the data field ends with ',' per standard */
		return NULL;

	res = (char *) malloc(*size);
	if (!res) /* Check if malloc() was successful */
		return NULL;

	return memcpy(res, data, *size);
}
