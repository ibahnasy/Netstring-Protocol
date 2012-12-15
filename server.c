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
 *  NetString Server
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h> /* errno */
#include <limits.h> /* LONG_MAX LONG_MIN */

#include "netstring.h"

#define BUFFER_SIZE 39
#define BACKLOG 5
char * port = "1100"; /* listening port */

int 
main()
{
    int status = 0; /* getaddrinfo status */
    int socketfd = 0; 
    int newSocketfd = 0; 
    int msgLen = 0;
    int optval = 1;
    struct addrinfo params;
    struct addrinfo *res;
    struct sockaddr_storage their_addr;
    socklen_t adrr_len;
    char msgBuff[BUFFER_SIZE]; /* buffer to hold coming messages from client */

    memset(&params, 0, sizeof params);
    params.ai_family = AF_UNSPEC;
    params.ai_socktype = SOCK_STREAM;
    params.ai_flags = AI_PASSIVE;

    /* initializing socket info */
    if ((status = getaddrinfo(NULL, port, &params, &res)) != 0)
    {
        perror("getaddrinfo: ");
        exit(1);
    }

    /* creating socket */
    if ((socketfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0)
    {
        perror("socket");
        exit(1);
    }

    /* allowing the reuse of existing sockets, if any */
    if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1) 
    {
        perror("setsockopt");
        exit(1);
    }

    /* binding socket to port */
    if ((bind(socketfd, res->ai_addr, res->ai_addrlen)) < 0)
    {
        perror("bind");
        exit(1);
    }

    /* start listening on port for connections */
    if((listen(socketfd, BACKLOG)) < 0)
    {
        perror("listen");
        exit(1);
    }

    /* on coming connecting, accept it */
    adrr_len = sizeof(their_addr);
    if ((newSocketfd = accept(socketfd, (struct sockaddr*)&their_addr, &adrr_len)) < 0)
    {
        perror("accept");
        exit(1);
    }

    while(1)
    {
        char * msg = "OK";
        char * encodedMessage;
        char * decodedMessage;
		size_t enc_size, dec_size;

        /* receiving data from connected client */
        if ((msgLen = recv(newSocketfd, msgBuff, BUFFER_SIZE, 0)) > 0)
        {
            decodedMessage = netstring_decode(msgBuff, &dec_size);

            if (strncmp(decodedMessage, "end", 3) == 0) /* exit condition */
            {
                printf("Exiting...\n");
				free(decodedMessage);
                break;
            }

            printf("Message Received: '%.*s'\n", (unsigned int)dec_size, decodedMessage);
			free(decodedMessage);

            /* sending reponse to client */
            encodedMessage = netstring_encode(msg, strlen(msg), &enc_size);

            send(newSocketfd, encodedMessage, enc_size, 0);
			free(encodedMessage);
        }
    }

    /* cleaning resources */
    freeaddrinfo(res);
    close(newSocketfd);
    close(socketfd);

    return 0;
}
