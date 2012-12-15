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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>

#include "netstring.h"

#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)

#define BUFFER_SIZE 39
char * port = 0; /* listening port */

int
main(int argc , char **argv)
{
    int status = 0; /* getaddrinfo status */
    int socketfd = 0;  
    struct addrinfo params;
    struct addrinfo *res;
    char msgBuff[BUFFER_SIZE]; /* buffer to hold coming messages from client */

    if (argc < 2)
    {
        printf("Port number must be provided.\n");
        printf("client <port_number>\n");
        exit(1);
    }

    port = argv[1]; /* setting the listening port from client command parameter */

    memset(&params, 0, sizeof params);
    params.ai_family = AF_UNSPEC;
    params.ai_socktype = SOCK_STREAM;
    params.ai_flags = AI_PASSIVE;

    /* initializing socket info */
    if ((status = getaddrinfo(NULL, port, &params, &res)) != 0)
    {
        perror("getaddrinfo");
        exit(1);
    }

    /* creating socket */
    if ((socketfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0)
    {
        perror("socket");
        exit(1);
    }

    /* connecting to server */
    if (connect(socketfd, res->ai_addr, res->ai_addrlen) < 0)
    {
        perror("connect");
        exit(1);
    }

    while(1)
    {
		char send_buf[1024];
        char *encodedMessage;
        char *decodedMessage;
		size_t send_size, enc_size, dec_size;

        /* getting message from input */
        scanf("%s", send_buf); /* FIXME: A possible buffer overflow here. */
		send_size = strlen(send_buf);

        encodedMessage = netstring_encode(send_buf,
				send_size, &enc_size); /* encoding with NetString
											STOP VIOLATING THE 80 CHARACTER WIDTH RULE */

        send(socketfd, encodedMessage, enc_size, 0); /* sending encoded message to server */
		free(encodedMessage);

        /* receiving response from server */
        recv(socketfd, msgBuff, BUFFER_SIZE, 0); /* receiving response/message from server */
        decodedMessage = netstring_decode(msgBuff, &dec_size);

		printf("Response: '%.*s'\n", (unsigned int)dec_size, decodedMessage);
		free(decodedMessage);

		/* STOP TREATING EVERYTHING AS A "STRING"... THIS IS NOT JAVA! */
        if (strncmp("end", send_buf, 3) == 0)
        {
            printf("Exiting...\n");
            break;
        }
    }

    /* cleaning resources */
    freeaddrinfo(res);
    close(socketfd);

    return 0;
}
