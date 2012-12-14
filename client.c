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
 *  NetString Client
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
main(int argc , char *argv[])
{
    int status = 0; /* getaddrinfo status */
    int socketfd = 0;  
    int msgLen = 0;
    struct addrinfo params;
    struct addrinfo *res;
    char msgBuff[BUFFER_SIZE]; /* buffer to hold coming messages from client */
    char sendMsgBuff[BUFFER_SIZE]; /* buffer to hold message for sending */

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
        size_t size;
        char * encodedMessage;
        char * decodedMessage;
        /* getting message from input */
        scanf("%s", sendMsgBuff);
        encodedMessage = (char *) malloc(BUFFER_SIZE);

        encodedMessage = netstring_encode(sendMsgBuff, strlen(sendMsgBuff)); /* encoding with NetString */
        
        #ifdef DEBUG
            printf("Formated Message: %s\n", encodedMessage);
        #endif

        send(socketfd, encodedMessage, strlen(encodedMessage), 0); /* sending encoded message to server */
        
        /* receiving response from server */
        msgLen = recv(socketfd, msgBuff, BUFFER_SIZE, 0); /* receiving response/message from server */
        msgBuff[msgLen] = '\0';

        decodedMessage = (char * )malloc(BUFFER_SIZE);

        decodedMessage = netstring_decode(msgBuff, &size); 
        decodedMessage[size] = '\0';

        printf("Response: %s\n\n", decodedMessage);

        #ifdef DEBUG
            printf("%s\n\n", decodedMessage);
        #endif

        if (strcmp(sendMsgBuff, "end") == 0)
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
